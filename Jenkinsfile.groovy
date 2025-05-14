// Node requirements:
//  VisualStudio2017: C compiler
//  OCT-SDK-1.6.2: CMake, msys
//  docker: docker

def Configs = [
    'msvc_md': [
        name: 'msvc_md',
        os: 'windows',
        node: 'VisualStudio2022 && OCT-SDK-1.6.2', // Test later MSVC
        target_install: 'install',
        target_test: 'test'
    ],
    'msvc_md_debug': [
        name: 'msvc_md_debug',
        os: 'windows',
        node: 'VisualStudio2022 && OCT-SDK-1.6.2', // Same MSVC as _md
        target_install: 'install',
        target_test: 'test'
    ],
    'msvc_mt': [
        name: 'msvc_mt',
        os: 'windows',
        node: 'VisualStudio2017 && OCT-SDK-1.6.2', // Test earlier MSVC
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
            def testLogDir = "build_${conf.name}/Testing/Temporary"
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
            archiveArtifacts(artifacts: "build_${conf.name}/doc/html/**")
        }
    }
}

// Currently getting cygwin heap error when parallellizing msvc_md and msvc_mt, so not doing that for now
def parallellTasks = [
    'ubuntu64':      tasks.ubuntu64,
    'mingw_w64':     tasks.mingw_w64,
    'msvc_md':       tasks.msvc_md,
    'msvc_md_debug': tasks.msvc_md_debug,
    'documentation': tasks.documentation,
]
parallel parallellTasks

tasks.msvc_mt()


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
        unstable("Test failure. Exit code: ${res}")
    }

    dir(testLogDir) {
        if (fileExists("LastTestsFailed.log")) {
            unstable("Failing tests: ${testLogDir}/LastTestsFailed.log)")
        }
        // The test log has different names if we run memcheck or not, but this file should
        // always exist if we run tests:
        if (!fileExists("CTestCostData.txt")) {
            unstable('File CTestCostData.txt is missing - perhaps tests were not run?')
        }
    }
}