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

set(RTTESTDIR ${FMILIBRARYHOME}/Test)

add_library(fmi_testutil STATIC ${RTTESTDIR}/fmi_testutil.c)
set(FMILIBFORTEST fmilib fmi_testutil)
if(FMILIB_BUILD_SHARED_LIB AND (FMILIB_LINK_TEST_TO_SHAREDLIB OR NOT FMILIB_BUILD_STATIC_LIB))
    set(FMILIBFORTEST fmilib_shared fmi_testutil)
    if(CMAKE_COMPILER_IS_GNUCC)
        set(FMILIBFORTEST ${FMILIBFORTEST})
    endif()
endif()
message(STATUS "Tests will be linked with ${FMILIBFORTEST}")

set(CATCH2_INCLUDE_DIR "${FMILIB_THIRDPARTYLIBS}/Catch2/single_include/catch2/")

# Test: jm_vector
add_executable(jm_vector_test ${RTTESTDIR}/jm_vector_test.c)
target_link_libraries(jm_vector_test ${JMUTIL_LIBRARIES})

# Test: jm_vector C++ with Catch2
set(CMAKE_CXX_STANDARD 11) # Required by Catch2
add_library(Catch INTERFACE)
target_include_directories(Catch
    INTERFACE
    ${CATCH2_INCLUDE_DIR})
add_executable(jm_vector_test_cpp
    ${RTTESTDIR}/jm_vector_test.cpp)
set_source_files_properties(${RTTESTDIR}/jm_vector_test.cpp PROPERTIES LANGUAGE CXX)
target_include_directories(jm_vector_test_cpp
    PUBLIC
    ${CATCH2_INCLUDE_DIR})
target_link_libraries(jm_vector_test_cpp
    Catch
    ${JMUTIL_LIBRARIES})
add_test(ctest_jm_vector_test_cpp jm_vector_test_cpp)

# Test: fmi3_basic_capi_test C++ with Catch2
add_executable(fmi3_basic_capi_test
    ${RTTESTDIR}/FMI3/fmi3_basic_capi_test.cpp)
target_include_directories(fmi3_basic_capi_test
    PUBLIC
    ${CATCH2_INCLUDE_DIR}
    ${FMIIMPORTDIR}/src)
target_link_libraries(fmi3_basic_capi_test
    PUBLIC
    Catch
    ${FMILIBFORTEST})
add_test(ctest_fmi3_basic_capi_test_cpp fmi3_basic_capi_test)

# Test: jm locale
add_executable(jm_locale_test ${RTTESTDIR}/jm_locale_test.c)
target_link_libraries(jm_locale_test ${JMUTIL_LIBRARIES})
if(FMILIB_TEST_LOCALE)
    target_compile_definitions(jm_locale_test PRIVATE -DFMILIB_TEST_LOCALE)
endif()

#Create function that zips the dummy FMUs
add_executable(compress_test_fmu_zip ${RTTESTDIR}/compress_test_fmu_zip.c)
target_link_libraries(compress_test_fmu_zip ${FMIZIP_LIBRARIES})

set_target_properties(
    jm_vector_test
    jm_vector_test_cpp
    fmi3_basic_capi_test
    jm_locale_test
    compress_test_fmu_zip
    PROPERTIES FOLDER "Test")

#Path to the executable
get_property(COMPRESS_EXECUTABLE TARGET compress_test_fmu_zip PROPERTY LOCATION)

set(TEST_OUTPUT_FOLDER "${CMAKE_CURRENT_BINARY_DIR}/Testing")

include_directories(${RTTESTDIR})
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
    endforeach(flag_var)
endif()

function(to_native_c_path path native_c_path)
    if(WIN32)
        STRING(REPLACE "/" "\\\\" tmp "${path}")
    else()
        set(tmp "${path}")
    endif()
    set (${native_c_path} ${tmp} PARENT_SCOPE)
endfunction()

set(UNCOMPRESSED_DUMMY_FILE_PATH_SRC "${RTTESTDIR}/try_to_uncompress_this_file.zip")
set(UNCOMPRESSED_DUMMY_FOLDER_PATH_DIST "${TEST_OUTPUT_FOLDER}")
file(COPY "${UNCOMPRESSED_DUMMY_FILE_PATH_SRC}" DESTINATION "${UNCOMPRESSED_DUMMY_FOLDER_PATH_DIST}")

set(COMPRESS_DUMMY_FILE_PATH_SRC "${RTTESTDIR}/try_to_compress_this_file.xml")
set(COMPRESS_DUMMY_FOLDER_PATH_DIST "${TEST_OUTPUT_FOLDER}")
set(COMPRESS_DUMMY_FILE_PATH_DIST "${TEST_OUTPUT_FOLDER}/successfully_compressed_this_file.zip")
file(COPY "${COMPRESS_DUMMY_FILE_PATH_SRC}" DESTINATION "${COMPRESS_DUMMY_FOLDER_PATH_DIST}")

#Create paths for the config_test.h
if(WIN32)
    set(DLL_OUTPUT_PATH "${TEST_OUTPUT_FOLDER}")

    string(REPLACE "/" "\\\\" UNCOMPRESSED_DUMMY_FILE_PATH_SRC "${UNCOMPRESSED_DUMMY_FILE_PATH_SRC}")
    string(REPLACE "/" "\\\\" UNCOMPRESSED_DUMMY_FILE_PATH_DIST "${UNCOMPRESSED_DUMMY_FILE_PATH_DIST}")
    string(REPLACE "/" "\\\\" UNCOMPRESSED_DUMMY_FOLDER_PATH_DIST "${UNCOMPRESSED_DUMMY_FOLDER_PATH_DIST}")
    string(REPLACE "/" "\\\\" COMPRESS_DUMMY_FILE_PATH_SRC "${COMPRESS_DUMMY_FILE_PATH_SRC}")
    string(REPLACE "/" "\\\\" COMPRESS_DUMMY_FILE_PATH_DIST "${COMPRESS_DUMMY_FILE_PATH_DIST}")
endif(WIN32)

#Move files and compress them to an FMU
function(compress_fmu OUTPUT_FOLDER_T MODEL_IDENTIFIER_T FILE_NAME_CS_ME_EXT_T TARGET_NAME_T XML_PATH_T SHARED_LIBRARY_PATH_T)
    set(FMU_OUTPUT_FOLDER_T ${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}_${FILE_NAME_CS_ME_EXT_T})
    set(FMU_OUTPUT_SHARED_LIBRARY_PATH_T ${FMU_OUTPUT_FOLDER_T}/binaries/${FMI_PLATFORM}/${MODEL_IDENTIFIER_T}${CMAKE_SHARED_LIBRARY_SUFFIX})

    #Must create the FMU directory in a separate command..
    if(NOT EXISTS ${FMU_OUTPUT_FOLDER_T}/binaries/${FMI_PLATFORM})
        file(MAKE_DIRECTORY ${FMU_OUTPUT_FOLDER_T}/binaries/${FMI_PLATFORM})
    endif()

    file(TO_NATIVE_PATH binaries/${FMI_PLATFORM}/${MODEL_IDENTIFIER_T}${CMAKE_SHARED_LIBRARY_SUFFIX} FMU_OUTPUT_SHARED_LIBRARY_PATH_OUT_T)

    #Move files to the FMU directories and compress
    add_custom_command(
       OUTPUT ${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}_${FILE_NAME_CS_ME_EXT_T}.fmu
       DEPENDS "${XML_PATH_T}" ${TARGET_NAME_T} compress_test_fmu_zip
       COMMAND "${CMAKE_COMMAND}" -E remove -f "${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}_${FILE_NAME_CS_ME_EXT_T}.fmu"
       COMMAND "${CMAKE_COMMAND}" -E copy "${XML_PATH_T}" "${FMU_OUTPUT_FOLDER_T}/modelDescription.xml"
       COMMAND "${CMAKE_COMMAND}" -E copy "${SHARED_LIBRARY_PATH_T}" "${FMU_OUTPUT_SHARED_LIBRARY_PATH_T}"
       COMMAND "${COMPRESS_EXECUTABLE}" "${MODEL_IDENTIFIER_T}.fmu" "modelDescription.xml" "${FMU_OUTPUT_SHARED_LIBRARY_PATH_OUT_T}" WORKING_DIRECTORY "${FMU_OUTPUT_FOLDER_T}"
       COMMAND "${CMAKE_COMMAND}" -E copy "${FMU_OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}.fmu" "${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}_${FILE_NAME_CS_ME_EXT_T}.fmu"
    )

    get_target_property(DLL_SOURCES ${TARGET_NAME_T} SOURCES)

    set(tname ${MODEL_IDENTIFIER_T}_${FILE_NAME_CS_ME_EXT_T}_FMU)
    add_custom_target(${tname} ALL
        DEPENDS ${OUTPUT_FOLDER_T}/${MODEL_IDENTIFIER_T}_${FILE_NAME_CS_ME_EXT_T}.fmu
        SOURCES "${XML_PATH_T}" ${DLL_SOURCES})
    set_target_properties(${tname} ${TARGET_NAME_T}
                        PROPERTIES FOLDER "TestFMUs")

endfunction(compress_fmu)

add_executable(fmi_zip_zip_test ${RTTESTDIR}/FMI1/fmi_zip_zip_test.c )
target_link_libraries (fmi_zip_zip_test ${FMIZIP_LIBRARIES})

add_executable(fmi_zip_unzip_test ${RTTESTDIR}/FMI1/fmi_zip_unzip_test.c )
target_link_libraries(fmi_zip_unzip_test ${FMIZIP_LIBRARIES})

add_executable(fmi_import_test
                    ${RTTESTDIR}/fmi_import_test.c
                    ${RTTESTDIR}/FMI1/fmi1_import_test.c
                    ${RTTESTDIR}/FMI2/fmi2_import_test.c
                    ${RTTESTDIR}/FMI3/fmi3_import_test.c)
target_link_libraries (fmi_import_test  ${FMILIBFORTEST})

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

# set(FMU_TEMPFOLDER ${TEST_OUTPUT_FOLDER}/tempfolder)
to_native_c_path(${TEST_OUTPUT_FOLDER}/tempfolder FMU_TEMPFOLDER)

file(MAKE_DIRECTORY ${TEST_OUTPUT_FOLDER}/tempfolder)

if(FMILIB_BUILD_BEFORE_TESTS)
    add_test(
        NAME ctest_build_all
        COMMAND "${CMAKE_COMMAND}" --build ${FMILIBRARYBUILD} --config $<CONFIGURATION>)
endif()

add_test(ctest_jm_locale_test jm_locale_test)

add_test(ctest_fmi_zip_unzip_test fmi_zip_unzip_test)
add_test(ctest_fmi_zip_zip_test fmi_zip_zip_test)

include(test_fmi1)
include(test_fmi2)
include(test_fmi3)

add_test(ctest_fmi_import_test_no_xml fmi_import_test ${UNCOMPRESSED_DUMMY_FILE_PATH_SRC} ${TEST_OUTPUT_FOLDER})
  set_tests_properties(ctest_fmi_import_test_no_xml PROPERTIES WILL_FAIL TRUE)
add_test(ctest_fmi_import_test_me_1 fmi_import_test ${FMU_ME_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi_import_test_cs_1 fmi_import_test ${FMU_CS_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi_import_test_me_2 fmi_import_test ${FMU2_ME_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi_import_test_cs_2 fmi_import_test ${FMU2_CS_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi_import_test_me_3 fmi_import_test ${FMU3_ME_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi_import_test_cs_3 fmi_import_test ${FMU3_CS_PATH} ${FMU_TEMPFOLDER})

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
        ctest_jm_vector_test_cpp
        ctest_fmi3_basic_capi_test_cpp
        ctest_jm_locale_test
        PROPERTIES DEPENDS ctest_build_all)
endif()
set_tests_properties(ctest_fmi_import_test_no_xml PROPERTIES DEPENDS ctest_fmi_zip_unzip_test)
