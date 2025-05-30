#    Copyright (C) 2012-2023 Modelon AB
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.
#
#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

set(FMIL_TEST_DIR ${CMAKE_SOURCE_DIR}/Test)
set(TEST_OUTPUT_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/Testing)

# Only the directories that are specific to tests.
set(FMIL_TEST_INCLUDE_DIRS
    ${FMILIB_TESTCONFIG_INCLUDE_DIR}
    ${FMIL_TEST_DIR} 
    ${FMIL_TEST_DIR}/FMI1
    ${FMIL_TEST_DIR}/FMI2
    ${FMIL_TEST_DIR}/FMI3
)

# XXX:
# Workaround to allow testing with sublibs on MinGW (unclear why it works with MSVC).
#
# The problem is that in config_fmilib.h we define FMILIB_EXPORT as __declspec(import),
# when building the tests. This makes the test expect to link against an import
# library. This is no problem when testing against fmilib_shared because it's a shared
# library, but we never compile the sublibs to shared libraries - only static libraries.
#
# This workaround makes FMILIB_EXPORT expand to nothing.
SET(FMIL_LINK_WITH_SUBLIBS FMILIB_STATIC_LIB_ONLY)

# ===============================================================================
# fmi_testutil
# ===============================================================================

add_library(fmi_testutil STATIC ${FMIL_TEST_DIR}/fmi_testutil.c)

target_link_libraries(fmi_testutil PRIVATE jmutils)  # Uses jm_vector
if(FMILIB_BUILD_SHARED_LIB AND (FMILIB_LINK_TEST_TO_SHAREDLIB OR NOT FMILIB_BUILD_STATIC_LIB))
    set(FMILIBFORTEST fmilib_shared fmi_testutil)
    target_link_libraries(fmi_testutil PRIVATE fmilib_shared)
else()
    set(FMILIBFORTEST fmilib fmi_testutil)
    target_link_libraries(fmi_testutil PRIVATE fmilib)
endif()

# Convenience: By setting this to PUBLIC, all tests that link with fmi_testutil
# will inherit the include directories.
target_include_directories(fmi_testutil PUBLIC ${FMIL_TEST_INCLUDE_DIRS})

# ===============================================================================

# Test: jm_vector
add_executable(jm_vector_test ${FMIL_TEST_DIR}/jm_vector_test.c)
target_link_libraries(jm_vector_test PRIVATE jmutils)
target_include_directories(jm_vector_test PRIVATE ${FMIL_TEST_INCLUDE_DIRS})

# Test: jm locale
if(FMILIB_TEST_LOCALE)
    add_executable(jm_locale_test ${FMIL_TEST_DIR}/jm_locale_test.c)
    target_link_libraries(jm_locale_test jmutils)
    target_include_directories(jm_locale_test PRIVATE ${FMIL_TEST_INCLUDE_DIRS})
    target_compile_definitions(jm_locale_test PRIVATE ${FMIL_LINK_WITH_SUBLIBS})
    add_test(NAME ctest_jm_locale_test COMMAND jm_locale_test)
    set_tests_properties(ctest_jm_locale_test PROPERTIES DEPENDS ctest_build_all)
endif()

#Create function that zips the dummy FMUs
add_executable(compress_test_fmu_zip ${FMIL_TEST_DIR}/compress_test_fmu_zip.c)
target_link_libraries(compress_test_fmu_zip fmizip)
target_include_directories(compress_test_fmu_zip PRIVATE ${FMIL_TEST_INCLUDE_DIRS})

if(MSVC)
    # default in MSVC for Debug build is incremental linking,
    # but dlls linked with this flag cannot be loaded with LoadLibrary
    foreach(flag_var
            CMAKE_MODULE_LINKER_FLAGS_DEBUG CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO
            CMAKE_SHARED_LINKER_FLAGS_DEBUG CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO)
            debug_message("Checking:  ${flag_var}=${${flag_var}}")
            string(REPLACE "/INCREMENTAL:NO" " " tmp "${${flag_var}}")
            string(REPLACE "/INCREMENTAL:YES" " " tmp "${tmp}")
            string(REPLACE "/INCREMENTAL" " " tmp "${tmp}")
            set(${flag_var} "${tmp} /INCREMENTAL:NO" CACHE STRING "linker flags" FORCE)
            debug_message("After replace: ${flag_var}=${${flag_var}}")
    endforeach()
endif()

function(to_native_c_path path native_c_path)
    if(WIN32)
        string(REPLACE "/" "\\\\" tmp "${path}")
    else()
        set(tmp "${path}")
    endif()
    set (${native_c_path} ${tmp} PARENT_SCOPE)
endfunction()

# Compresses files to an FMU.
#
# FIXME:
# This function also creates an uncompressed FMU directory which some tests
# (fmiXX_import_xml_test_XX) point at just for testing an XML. They should
# be fixed to point at XML_PATH_T.
#
# @OUTPUT_FOLDER_T:        The directory which the FMU will be placed in. (The uncompressed directory is also created here.)
# @MODEL_IDENTIFIER_T:     The model identifier of the FMU. Will be used as stem for the FMU name.
# @FILE_NAME_CS_ME_EXT_T:  A suffix added to the stem of the FMU base name.
# @TARGET_NAME_T:          The target for building the shared library.
# @XML_PATH_T:             The path to the modelDescription that will be zipped.
# @SHARED_LIBRARY_PATH_T:  The path to the shared library produced by TARGET_NAME_T.
function(compress_fmu OUTPUT_FOLDER_T MODEL_IDENTIFIER_T FILE_NAME_CS_ME_EXT_T TARGET_NAME_T XML_PATH_T SHARED_LIBRARY_PATH_T FMI_PLATFORM_T)
    set(FMU_FILE_NAME_T ${MODEL_IDENTIFIER_T}_${FILE_NAME_CS_ME_EXT_T})
    set(FMU_OUTPUT_FOLDER_T ${OUTPUT_FOLDER_T}/${FMU_FILE_NAME_T})
    set(FMU_OUTPUT_SHARED_LIBRARY_PATH_T ${FMU_OUTPUT_FOLDER_T}/binaries/${FMI_PLATFORM_T}/${MODEL_IDENTIFIER_T}${CMAKE_SHARED_LIBRARY_SUFFIX})

    #Must create the FMU directory in a separate command..
    if(NOT EXISTS ${FMU_OUTPUT_FOLDER_T}/binaries/${FMI_PLATFORM_T})
        file(MAKE_DIRECTORY ${FMU_OUTPUT_FOLDER_T}/binaries/${FMI_PLATFORM_T})
    endif()

    file(TO_NATIVE_PATH binaries/${FMI_PLATFORM_T}/${MODEL_IDENTIFIER_T}${CMAKE_SHARED_LIBRARY_SUFFIX} FMU_OUTPUT_SHARED_LIBRARY_PATH_OUT_T)

    #Move files to the FMU directories and compress  # TODO: Why not compress to correct location right away?
    add_custom_command(
        OUTPUT ${OUTPUT_FOLDER_T}/${FMU_FILE_NAME_T}.fmu
        DEPENDS "${XML_PATH_T}" ${TARGET_NAME_T} compress_test_fmu_zip
        COMMAND "${CMAKE_COMMAND}" -E remove -f "${OUTPUT_FOLDER_T}/${FMU_FILE_NAME_T}.fmu"
        COMMAND "${CMAKE_COMMAND}" -E copy "${XML_PATH_T}" "${FMU_OUTPUT_FOLDER_T}/modelDescription.xml"
        COMMAND "${CMAKE_COMMAND}" -E copy "${SHARED_LIBRARY_PATH_T}" "${FMU_OUTPUT_SHARED_LIBRARY_PATH_T}"
        COMMAND ${CMAKE_CROSSCOMPILING_EMULATOR} $<TARGET_FILE:compress_test_fmu_zip> "${MODEL_IDENTIFIER_T}.fmu" "modelDescription.xml" "${FMU_OUTPUT_SHARED_LIBRARY_PATH_OUT_T}" WORKING_DIRECTORY "${FMU_OUTPUT_FOLDER_T}"
        COMMAND "${CMAKE_COMMAND}" -E copy "${FMU_OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}.fmu" "${OUTPUT_FOLDER_T}/${FMU_FILE_NAME_T}.fmu"
    )

    get_target_property(DLL_SOURCES ${TARGET_NAME_T} SOURCES)

    set(tname ${FMU_FILE_NAME_T}_FMU)
    add_custom_target(${tname} ALL
        DEPENDS ${OUTPUT_FOLDER_T}/${FMU_FILE_NAME_T}.fmu
        SOURCES "${XML_PATH_T}" ${DLL_SOURCES})
    set_target_properties(${tname} ${TARGET_NAME_T}
                        PROPERTIES FOLDER "TestFMUs")

endfunction(compress_fmu)

add_executable(fmi_zip_zip_test ${FMIL_TEST_DIR}/fmi_zip_zip_test.c )
target_link_libraries(fmi_zip_zip_test fmizip)
target_include_directories(fmi_zip_zip_test PRIVATE ${FMIL_TEST_INCLUDE_DIRS})

add_executable(fmi_zip_unzip_test ${FMIL_TEST_DIR}/fmi_zip_unzip_test.c )
target_link_libraries(fmi_zip_unzip_test fmizip)
target_include_directories(fmi_zip_unzip_test PRIVATE ${FMIL_TEST_INCLUDE_DIRS})

add_executable(fmi_import_test
                    ${FMIL_TEST_DIR}/fmi_import_test.c
                    ${FMIL_TEST_DIR}/FMI1/fmi1_import_test.c
                    ${FMIL_TEST_DIR}/FMI2/fmi2_import_test.c
                    ${FMIL_TEST_DIR}/FMI3/fmi3_import_test.c)
target_link_libraries(fmi_import_test ${FMILIBFORTEST})
target_include_directories(fmi_import_test PRIVATE ${FMIL_TEST_INCLUDE_DIRS})

set_target_properties(
    fmi_zip_zip_test
    fmi_zip_unzip_test
    fmi_import_test
    PROPERTIES FOLDER "Test")
# include CTest gives more options (such as running valgrind automatically)
include(CTest)

#Define values in the "config.h"
set(CTEST_RETURN_SUCCESS 0)
set(CTEST_RETURN_FAIL 1)

to_native_c_path(${TEST_OUTPUT_FOLDER}/Temporary TEST_RESULTS_DIR)
file(MAKE_DIRECTORY ${TEST_RESULTS_DIR})

if(FMILIB_BUILD_BEFORE_TESTS)
    add_test(
        NAME ctest_build_all
        COMMAND "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --config $<CONFIGURATION>)
endif()

add_test(NAME ctest_fmi_zip_unzip_test COMMAND fmi_zip_unzip_test)
add_test(NAME ctest_fmi_zip_zip_test COMMAND fmi_zip_zip_test)


# ------------------------------------------------------------------------------
# Catch2
# ------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 11) # Required by Catch2
add_library(Catch INTERFACE)

# Catch2 consists of only headers, but we still want to compile its main function
# in a separate object file so we don't need to do it for each test.
add_library(catch2_main STATIC ${FMIL_TEST_DIR}/catch2_main.cpp)
target_link_libraries(catch2_main PUBLIC Catch)
target_include_directories(catch2_main PUBLIC ${FMILIB_THIRDPARTYLIBS}/Catch2/single_include/catch2)

# Creates a Catch2 test.
#
# @TEST_NAME: Name of the test file, without any suffix.
# @TEST_DIR:  The basename of parent directory of the test file.
function(add_catch2_test TEST_NAME TEST_DIR)
    add_executable(${TEST_NAME} ${FMIL_TEST_DIR}/${TEST_DIR}/${TEST_NAME}.cpp)
    set_source_files_properties(${FMIL_TEST_DIR}/${TEST_DIR}/${TEST_NAME}.cpp PROPERTIES LANGUAGE CXX)
    target_link_libraries(${TEST_NAME} PRIVATE catch2_main ${FMILIBFORTEST})
    add_test(NAME ctest_${TEST_NAME} COMMAND ${TEST_NAME})
    set_target_properties(${TEST_NAME} PROPERTIES FOLDER "Test/${TEST_DIR}")
    if(FMILIB_BUILD_BEFORE_TESTS)
        set_tests_properties(ctest_${TEST_NAME} PROPERTIES DEPENDS ctest_build_all)
    endif()
endfunction()

# Creates a Catch2 test with compile definitions.
#
# @TEST_NAME: Name of the test file, without any suffix.
# @TEST_DIR:  The basename of parent directory of the test file.
# @SCOPE:     INTERFACE|PUBLIC|PRIVATE
# @DEFS:      Definitions
function(add_catch2_test_with_compile_def TEST_NAME TEST_DIR SCOPE DEFS)
    add_catch2_test(${TEST_NAME} ${TEST_DIR})
    target_compile_definitions(${TEST_NAME} ${SCOPE} ${DEFS})
endfunction()
# ------------------------------------------------------------------------------

include(test_fmi1)
include(test_fmi2)
include(test_fmi3)

add_test(NAME ctest_fmi_import_test_no_xml COMMAND fmi_import_test ${UNCOMPRESSED_DUMMY_FILE_PATH_SRC})
  set_tests_properties(ctest_fmi_import_test_no_xml PROPERTIES WILL_FAIL TRUE)
add_test(NAME ctest_fmi_import_test_me_1 COMMAND fmi_import_test ${FMU1_ME_PATH})
add_test(NAME ctest_fmi_import_test_cs_1 COMMAND fmi_import_test ${FMU1_CS_PATH})
add_test(NAME ctest_fmi_import_test_me_2 COMMAND fmi_import_test ${FMU2_ME_PATH})
add_test(NAME ctest_fmi_import_test_cs_2 COMMAND fmi_import_test ${FMU2_CS_PATH})
add_test(NAME ctest_fmi_import_test_me_3 COMMAND fmi_import_test ${FMU3_ME_PATH})
add_test(NAME ctest_fmi_import_test_cs_3 COMMAND fmi_import_test ${FMU3_CS_PATH})

if(FMILIB_BUILD_BEFORE_TESTS)
    set_tests_properties(
        ctest_fmi_import_test_no_xml
        ctest_fmi_import_test_me_1
        ctest_fmi_import_test_cs_1
        ctest_fmi_import_test_me_2
        ctest_fmi_import_test_cs_2
        ctest_fmi_import_test_me_3
        ctest_fmi_import_test_cs_3
        ctest_fmi_zip_unzip_test
        ctest_fmi_zip_zip_test
        PROPERTIES DEPENDS ctest_build_all)
endif()
set_tests_properties(ctest_fmi_import_test_no_xml PROPERTIES DEPENDS ctest_fmi_zip_unzip_test)
