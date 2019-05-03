// Node requirements:
//  VisualStudio2010: C compiler
//  OCT-SDK-1.4: CMake, msys
//  docker: docker

def Config = [
    'win64': [
        node: 'VisualStudio2010 && OCT-SDK-1.4'
    ],
    'win64_static_runtime': [
        node: 'VisualStudio2010 && OCT-SDK-1.4'
    ], 
    'linux64': [
        node: 'docker'
    ], 
    'documentation': [
        node: 'docker'
    ]
]

def version = '2.0.3-SNAPSHOT'

// Loads the 'signBinaries' function
library 'ModelonCommon@trunk'

def tasks = [:]
for (target_temp in ['win64', 'win64_static_runtime', 'linux64']) {
    def target = target_temp // bind variable before use in closure
    tasks[target] = {
        node(Config[target].node) {
            def testLogDir = "testlogs-${target}"
            def installDir = "install-${target}"

            stage("Checkout: ${target}") {
                checkout scm
            }

            stage("Build: ${target}") {
                build(target)
            }

            stage("Test: ${target}") {
                test(target, testLogDir)
            }

            stage("Sign: ${target}") {
                dir("${installDir}/lib") {
                    // Seems like .so files are not allowed to be signed,
                    // so just signing the .dll for now.
                    signFiles(target, "*.dll")
                }
            }

            stage("Archive: ${target}") {
                archiveArtifacts(artifacts: "${installDir}/**")
                archiveArtifacts(artifacts: "${testLogDir}/**")
            }
        }
    }
}
tasks['documentation'] = {
    def target = 'documentation'
    node(Config[target].node) {

        stage("Checkout: doc") {
            checkout scm
        }

        stage("Build: doc") {
            build(target)
        }

        stage("Archive: doc") {
            archiveArtifacts(artifacts: "doc/**")
        }
    }
}

parallel tasks

def build(target) {
    if (target == 'win64' || target == 'win64_static_runtime') {
        bat """
            :: Add msys to path
            set PATH=%DARWIN_SDK_HOME%\\MinGW\\msys\\1.0\\bin;%PATH%
            :: Add cmake to path
            set PATH=%DARWIN_SDK_HOME%\\CMake\\bin;%PATH%

            :: Call bash which now is on top of PATH
            call bash ./build.sh ${target} install
        """
    } else if (target == 'linux64' || target == 'documentation') {
        sh """
            # Fix permissions
            chmod u+x ./Dockerfiles/build_images.sh
            chmod u+x ./Dockerfiles/run.sh

            # Build images
            ./Dockerfiles/build_images.sh

            # Run the build-script inside the image, and copy artifacts to
            # repo root.
            ./Dockerfiles/run.sh ${target} install
        """
    } else {
        error(message: "Invalid target: ${target}")
    }
}

def test(target, testLogDir) {
    def cmd_test = "ctest -C MinSizeRel"
    def returnStatus
    if (target == 'win64' || target == 'win64_static_runtime') {
        returnStatus = bat(returnStatus: true, script: """
            :: Add msys to path
            set PATH=%DARWIN_SDK_HOME%\\MinGW\\msys\\1.0\\bin;%PATH%
            :: Add cmake to path
            set PATH=%DARWIN_SDK_HOME%\\CMake\\bin;%PATH%
            
            :: Call bash which now is on top of PATH
            call bash ./build.sh ${target} test
        """)
    } else if (target == 'linux64') {
        returnStatus = sh(returnStatus: true, script: """
            # Fix permissions
            chmod u+x ./Dockerfiles/build_images.sh
            chmod u+x ./Dockerfiles/run.sh

            # Build images
            ./Dockerfiles/build_images.sh

            # Run the build-script inside the image, and copy artifacts to
            # repo root
            ./Dockerfiles/run.sh ${target} test
        """)
    } else {
        error(message: "Invalid target: ${target}")
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

/**
 * Signs the files that match the glob expressions.
 *
 * param globExpressions:
 *      vararg of globExpressions that are evaluated from the current directory
 * param prefix:
 *      used to create a unique intermediate stash (needed for signing),
 *      and to be displayed in the signBinaries step description
 */
 def signFiles(prefix, Object... globExpressions) {
    for (glob in globExpressions) {
        for (file in findFiles(glob: glob)) {
            def fname = file.toString()
            def nameUnsigned = "${prefix}_${fname}_unsigned"
            def nameSigned = "${prefix}_${fname}_signed"
            stash(name: nameUnsigned, includes: file.toString())
            signBinaries(nameUnsigned, nameSigned, "Sign binaries: ${fname} (${prefix})")

            // Replace the old file with the signed one
            deleteFile(fname)
            unstash(nameSigned)
        }
    }
 }

 def deleteFile(filename) {
    bat "del ${filename}"
 }

 def setBuildStatus(status, msg) {
     currentBuild.result = status
     println("Build result manually set to ${status}. Reason:\n${msg}")
 }
