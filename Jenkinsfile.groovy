// Node requirements:
//  VisualStudio2017: C compiler
//  OCT-SDK-1.6.2: CMake, msys
//  docker: docker

def Configs = [
    'win64': [
        name: 'win64',
        os: 'windows',
        node: 'VisualStudio2017 && OCT-SDK-1.6.2',
        target_install: 'install',
        target_test: 'test'
    ],
    'win64_static_runtime': [
        name: 'win64_static_runtime',
        os: 'windows',
        node: 'VisualStudio2017 && OCT-SDK-1.6.2',
        target_install: 'install',
        target_test: 'test'
    ],
    'mingw_w64': [
        name: 'mingw_w64',
        os: 'windows',
        node: 'VisualStudio2017 && OCT-SDK-1.6.2',
        target_install: 'install',
        target_test: 'test'
    ],
    'centos64': [
        name: 'centos64',
        os: 'linux',
        node: 'docker',
        target_install: '-C . -f build/docker/Makefile install',
        target_test: '-C . -f build/docker/Makefile test'
    ],
    'ubuntu64': [
        name: 'ubuntu64',
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
            def testLogDir = "build_${conf.name}/Testing/TestResults"
            def installDir = "install_${conf.name}"

            stage("Checkout: ${conf.name}") {
                checkout scm
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
        }

        stage("Clean artifacts: ${conf.name}") {
            clean(conf)
        }

        stage("Build: ${conf.name}") {
            build(conf)
        }

        stage("Archive: ${conf.name}") {
            archiveArtifacts(artifacts: "install_${conf.name}/doc/html/**")
        }
    }
}

// Currently getting cygwin heap error when parallellizing win64 and win64_static_runtime, so not doing that for now
def parallellTasks = [
    'centos64': tasks.centos64,
    'ubuntu64': tasks.ubuntu64,
    'mingw_w64': tasks.mingw_w64,
    'win64': tasks.win64,
    'documentation': tasks.documentation,
]
parallel parallellTasks

tasks.win64_static_runtime()


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
    def res // exit code
    if (conf.os == 'windows') {
        res = bat(returnStatus: true, script: """
            call build\\setenv.bat
            make ${conf.target_test} CONFIG_FILE=build/config/${conf.name}
        """)
    } else if (conf.os == 'linux') {
        res = sh(returnStatus: true, script: """
            make ${conf.target_test} CONFIG_FILE=build/config/${conf.name}
        """)
    } else {
        error(message: "Invalid config operating system: ${conf.os}")
    }
    if (res) {
        setBuildStatus('UNSTABLE', "Test failure. Exit code: ${res}")
    }

    dir(testLogDir) {
        if (fileExists("LastTestsFailed.log")) {
            setBuildStatus('UNSTABLE', "Failing tests: ${testLogDir}/LastTestsFailed.log)")
        }
        // The test log has different names if we run memcheck or not, but this file should
        // always exist if we run tests:
        if (!fileExists("CTestCostData.txt")) {
            setBuildStatus('UNSTABLE', 'File CTestCostData.txt is missing - perhaps tests were not run?')
        }
    }
}


def setBuildStatus(status, msg) {
    currentBuild.result = status
    println("Build result manually set to ${status}. Reason:\n${msg}")
}
