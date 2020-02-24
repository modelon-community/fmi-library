# Windows dependencies:
- docker
- msys 1
    - make (tested with GNU Make 3.81) 

# Overview
This directory contains the scripts for preprocessing the out-commented macros that generate functions.

The workflow is currently to use these scripts to generate the functions, and then paste them in the source files.

Ideally this should be a build target, but since I don't want to add docker dependencies or create Windows port of the script, I'm doing this for now.

# How to run
1. copy the macros (definitions + call) to some file: mount_me/input/<your_file>
2. run 'make' in this directory
3. paste the result from the mount_me/output/<your_file> file to the source files

On Linux, the run.sh script in dir 'mount_me' can probably be called directly, given that the dependencies specified in the Dockerfile exist.
