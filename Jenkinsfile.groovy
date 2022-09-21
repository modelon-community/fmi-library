// Node requirements:
//  VisualStudio2017: C compiler
//  OCT-SDK-1.5.4: CMake, msys
//  docker: docker

def Configs = [
    'win64': [
        name: 'win64',
        os: 'windows',
        node: 'VisualStudio2017 && OCT-SDK-1.5.4',
        target_install: 'install',
        target_test: 'test'
    ],
    'win64_static_runtime': [
        name: 'win64_static_runtime',
        os: 'windows',
        node: 'VisualStudio2017 && OCT-SDK-1.5.4',
        target_install: 'install',
        target_test: 'test'
    ],
    'mingw_w64': [
        name: 'mingw_w64',
        os: 'windows',
        node: 'OCT-SDK-1.5.4',
        target_install: 'install',
        target_test: 'test'
    ], 
    'linux64': [
        name: 'linux64',
        os: 'linux',
        node: 'docker',
        target_install: '-C . -f build/docker/Makefile install',
        target_test: '-C . -f build/docker/Makefile test'
    ], 
    'documentation': [
        name: 'documentation',
        os: 'linux',
        node: 'docker',
        target_install: '-C . -f build/docker/Makefile documentation',
        target_test: 'N/A'
    ]
]

def tasks = [:]
// for (conf_entry in Configs) // This doesn't work, causes SerializationError for the Map
Configs.each { conf_entry ->
    def conf = conf_entry.value // bind variable before use in closure
    tasks[conf.name] = {
        node(conf.node) {
            def testLogDir = "build-${conf.name}/Testing/Temporary"
            def installDir = "install-${conf.name}"

            stage("Checkout: ${conf.name}") {
                checkout scm
                fixFilePermissions(conf.os)
            }

            stage("Clean artifacts: ${conf.name}") {
                clean(conf)
            }

            stage("Build: ${conf.name}") {
                build(conf)
            }

            stage("Test: ${conf.name}") {
                test(conf, testLogDir)
            }

            stage("Archive: ${conf.name}") {
                archiveArtifacts(artifacts: "${installDir}/**")
                archiveArtifacts(artifacts: "${testLogDir}/**")
            }
        }
    }
}
def conf = Configs['documentation']
tasks[conf.name] = {
    node(conf.node) {

        stage("Checkout: ${conf.name}") {
            checkout scm
            fixFilePermissions(conf.os)
        }

        stage("Clean artifacts: ${conf.name}") {
            clean(conf)
        }

        stage("Build: ${conf.name}") {
            build(conf)
        }

        stage("Archive: ${conf.name}") {
            archiveArtifacts(artifacts: "install-${conf.name}/doc/html/**")
        }
    }
}

parallel tasks

def clean(conf) {
    if (conf.os == 'windows') {
        bat """
            call build\\setenv.bat
            make clean
        """
    } else if (conf.os == 'linux') {
        sh """
            make clean
        """
    } else {
        error(message: "Invalid configuration operating system: ${conf.os}")
    }

}

def build(conf) {
    if (conf.os == 'windows') {
        bat """
            call build\\setenv.bat
            make ${conf.target_install} CONFIG_FILE=build/config/${conf.name}
        """
    } else if (conf.os == 'linux') {
        sh """
            make ${conf.target_install} CONFIG_FILE=build/config/${conf.name}
        """
    } else {
        error(message: "Invalid configuration operating system: ${conf.os}")
    }
}

def test(conf, testLogDir) {
    def returnStatus
    if (conf.os == 'windows') {
        returnStatus = bat(returnStatus: true, script: """
            call build\\setenv.bat
            make ${conf.target_test} CONFIG_FILE=build/config/${conf.name}
        """)
    } else if (conf.os == 'linux') {
        returnStatus = sh(returnStatus: true, script: """
            make ${conf.target_test} CONFIG_FILE=build/config/${conf.name}
        """)
    } else {
        error(message: "Invalid config operating system: ${conf.os}")
    }
    if (returnStatus != 0) {
        setBuildStatus('UNSTABLE', "Test failure. Exit code from ctest: ${returnStatus}")
    }

    dir(testLogDir) {
        if (fileExists("LastTestsFailed.log")) {
            setBuildStatus('UNSTABLE', "Failing tests: ${testLogDir}/LastTestsFailed.log)")
        }
        if (!fileExists("LastTest.log")) {
            setBuildStatus('UNSTABLE', 'Test log is missing')
        }
    }
}


def setBuildStatus(status, msg) {
    currentBuild.result = status
    println("Build result manually set to ${status}. Reason:\n${msg}")
}

def fixFilePermissions(os) {
    if (os == 'linux') {
        sh """
            # Allow internal docker script to run
            chmod a+x build/docker/build.sh

            # Allow copying artifacts to host
            chmod 777 .
        """
    }
}

