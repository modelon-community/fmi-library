#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

include_directories(${FMIL_TEST_DIR}/FMI1)

add_executable(fmi1_capi_cs_test ${FMIL_TEST_DIR}/FMI1/fmi1_capi_cs_test.c)
target_link_libraries(fmi1_capi_cs_test ${FMICAPI_LIBRARIES})

add_executable (fmi1_capi_me_test ${FMIL_TEST_DIR}/FMI1/fmi1_capi_me_test.c)
target_link_libraries (fmi1_capi_me_test ${FMICAPI_LIBRARIES})

#Defines for the test FMUs
set(FMU_DUMMY_ME_MODEL_IDENTIFIER BouncingBall) #This must be the same as in the xml-file
set(FMU_DUMMY_CS_MODEL_IDENTIFIER BouncingBall) #This must be the same as in the xml-file
set(FMU_DUMMY_MF_MODEL_IDENTIFIER BouncingBall_malformed) #This must be the same as in the xml-file

set(FMU_DUMMY_FOLDER ${FMIL_TEST_DIR}/FMI1/fmu_dummy)

set(FMU_DUMMY_ME_SOURCE ${FMU_DUMMY_FOLDER}/fmu1_model_me.c)
set(FMU_DUMMY_CS_SOURCE ${FMU_DUMMY_FOLDER}/fmu1_model_cs.c)
set(FMU_DUMMY_HEADERS
  ${FMU_DUMMY_FOLDER}/fmu1_model.h
  ${FMU_DUMMY_FOLDER}/fmu1_model_defines.h
)

add_library(fmu1_dll_me SHARED ${FMU_DUMMY_ME_SOURCE} ${FMU_DUMMY_HEADERS})
add_library(fmu1_dll_cs SHARED ${FMU_DUMMY_CS_SOURCE} ${FMU_DUMMY_HEADERS})

set(XML_ME_PATH ${FMU_DUMMY_FOLDER}/modelDescription_me.xml)
set(XML_CS_PATH ${FMU_DUMMY_FOLDER}/modelDescription_cs.xml)
set(XML_CS_TC_PATH ${FMU_DUMMY_FOLDER}/modelDescription_cs_tc.xml)
set(XML_MF_PATH ${FMU_DUMMY_FOLDER}/modelDescription_malformed.xml)
set(TYPE_DEFINITIONS_MODEL_DESC_DIR ${FMIL_TEST_DIR}/FMI1/parser_test_xmls/type_definitions)
set(VARIABLE_BAD_TYPE_VARIABILITY_MODEL_DESC_DIR ${FMIL_TEST_DIR}/FMI1/parser_test_xmls/variable_bad_type_variability)

set(SHARED_LIBRARY_ME_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu1_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX})
set(SHARED_LIBRARY_CS_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu1_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX})

#Create FMU 1.0 ME/CS Model and generate library path to be used in test config

to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}fmu1_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX}\""
                FMU1_DLL_ME_PATH)
to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}fmu1_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX}\""
                FMU1_DLL_CS_PATH)

#function(compress_fmu OUTPUT_FOLDER MODEL_IDENTIFIER FILE_NAME_CS_ME_EXT TARGET_NAME XML_PATH SHARED_LIBRARY_PATH)
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU_DUMMY_MF_MODEL_IDENTIFIER}" "mf" "fmu1_dll_me" "${XML_MF_PATH}" "${SHARED_LIBRARY_ME_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU_DUMMY_ME_MODEL_IDENTIFIER}" "me" "fmu1_dll_me" "${XML_ME_PATH}" "${SHARED_LIBRARY_ME_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU_DUMMY_CS_MODEL_IDENTIFIER}" "cs" "fmu1_dll_cs" "${XML_CS_PATH}" "${SHARED_LIBRARY_CS_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU_DUMMY_CS_MODEL_IDENTIFIER}" "cs_tc" "fmu1_dll_cs" "${XML_CS_TC_PATH}" "${SHARED_LIBRARY_CS_PATH}")

add_executable(fmi1_variable_bad_type_variability_test ${FMIL_TEST_DIR}/FMI1/fmi1_variable_bad_type_variability_test.c)
target_link_libraries(fmi1_variable_bad_type_variability_test ${FMILIBFORTEST})

add_executable(fmi1_import_options_test ${FMIL_TEST_DIR}/FMI1/fmi1_import_options_test.c)
target_link_libraries(fmi1_import_options_test ${FMILIBFORTEST})

add_executable(fmi1_import_default_experiment_test ${FMIL_TEST_DIR}/FMI1/fmi1_import_default_experiment_test.c)
target_link_libraries(fmi1_import_default_experiment_test ${FMILIBFORTEST})
add_executable(fmi1_type_definitions_test ${FMIL_TEST_DIR}/FMI1/fmi1_import_type_definitions_test.c)
target_link_libraries(fmi1_type_definitions_test ${FMILIBFORTEST})

add_executable(fmi1_xml_parsing_test ${FMIL_TEST_DIR}/FMI1/fmi1_xml_parsing_test.c)
target_link_libraries(fmi1_xml_parsing_test ${FMILIBFORTEST}  )
if(FMILIB_TEST_LOCALE)
    target_compile_definitions(fmi1_xml_parsing_test PRIVATE -DFMILIB_TEST_LOCALE)
endif()

add_executable(fmi_import_xml_test ${FMIL_TEST_DIR}/FMI1/fmi_import_xml_test.c)
target_link_libraries(fmi_import_xml_test ${FMILIBFORTEST})

add_executable(fmi_import_me_test ${FMIL_TEST_DIR}/FMI1/fmi_import_me_test.c)
target_link_libraries(fmi_import_me_test ${FMILIBFORTEST})
add_executable(fmi_import_cs_test ${FMIL_TEST_DIR}/FMI1/fmi_import_cs_test.c)
target_link_libraries(fmi_import_cs_test ${FMILIBFORTEST})

to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU_DUMMY_ME_MODEL_IDENTIFIER}_me.fmu" FMU_ME_PATH)

to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU_DUMMY_CS_MODEL_IDENTIFIER}_cs.fmu" FMU_CS_PATH)

to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU_DUMMY_CS_MODEL_IDENTIFIER}_cs_tc.fmu" FMU_CS_TC_PATH)

# set(FMU_TEMPFOLDER ${TEST_OUTPUT_FOLDER}/tempfolder)
to_native_c_path(${TEST_OUTPUT_FOLDER}/tempfolder FMU_TEMPFOLDER)

add_test(ctest_fmi1_variable_bad_type_variability_test fmi1_variable_bad_type_variability_test ${VARIABLE_BAD_TYPE_VARIABILITY_MODEL_DESC_DIR})
add_test(ctest_fmi1_xml_parsing_test fmi1_import_default_experiment_test ${FMIL_TEST_DIR}/FMI1/parser_test_xmls/default_experiment/)
add_test(ctest_fmi1_xml_parsing_test fmi1_xml_parsing_test ${FMIL_TEST_DIR}/FMI1/parser_test_xmls/)
add_test(ctest_fmi1_type_definitions_test fmi1_type_definitions_test ${TYPE_DEFINITIONS_MODEL_DESC_DIR})
add_test(ctest_fmi_import_me_test fmi_import_me_test ${FMU_ME_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi_import_cs_test fmi_import_cs_test ${FMU_CS_PATH} ${FMU_TEMPFOLDER} "modelDescription_cs.xml")
add_test(ctest_fmi_import_cs_tc_test fmi_import_cs_test ${FMU_CS_TC_PATH} ${FMU_TEMPFOLDER} "modelDescription_cs_tc.xml")
# the next test relies on the output from the previous one.
add_test(ctest_fmi_import_xml_test_empty fmi_import_xml_test ${FMU_DUMMY_FOLDER})
add_test(ctest_fmi_import_xml_test fmi_import_xml_test ${FMU_TEMPFOLDER})
add_test(ctest_fmi_import_xml_test_mf fmi_import_xml_test ${TEST_OUTPUT_FOLDER}/${FMU_DUMMY_MF_MODEL_IDENTIFIER}_mf)
  set_tests_properties(ctest_fmi_import_xml_test_mf PROPERTIES WILL_FAIL TRUE)
add_test(ctest_fmi1_import_test_options fmi1_import_options_test ${FMU_ME_PATH} ${FMU_TEMPFOLDER})

add_test(ctest_fmi1_capi_cs_test fmi1_capi_cs_test)
add_test(ctest_fmi1_capi_me_test fmi1_capi_me_test)

##Add logger test
add_executable (fmi1_logger_test ${FMIL_TEST_DIR}/FMI1/fmi1_logger_test.c)
target_link_libraries (fmi1_logger_test  ${FMILIBFORTEST})

set(logger_output_file "${TEST_OUTPUT_FOLDER}/fmi1_logger_test_output.txt")
set(logger_reference_file "${FMIL_TEST_DIR}/FMI1/fmi1_logger_test_output.txt")

add_test(ctest_fmi1_logger_test_run fmi1_logger_test ${FMU_ME_PATH} ${FMU_TEMPFOLDER} ${logger_output_file})

if(NOT CMAKE_GENERATOR STREQUAL "MSYS Makefiles")
    # Skip test for MinGW, since we know it won't pass due to issues with long log messages and vsnprintf.
    add_test(ctest_fmi1_logger_test_check ${CMAKE_COMMAND} -E compare_files ${logger_output_file}  ${logger_reference_file})
    set_tests_properties(
        ctest_fmi1_logger_test_check
        PROPERTIES DEPENDS ctest_fmi1_logger_test_run
    )

    # 'ctest_build_all' must also skip memcheck, but it needs to run before all
    # other tests, so just calling it first by name in the linux makefile wrapper
    set_tests_properties(
        ctest_fmi1_logger_test_check
        PROPERTIES LABELS skip_memcheck
    )

    set_tests_properties(
        ctest_fmi1_logger_test_check
        PROPERTIES DEPENDS ctest_fmi1_logger_test_run
    )
endif()

set_target_properties(
    fmi_import_me_test
    fmi_import_cs_test
    fmi_import_xml_test
    fmi1_capi_cs_test
    fmi1_capi_me_test
    fmi1_logger_test
    fmi1_xml_parsing_test
    fmi1_import_default_experiment_test
    fmi1_import_options_test
    PROPERTIES FOLDER "Test/FMI1")


if(FMILIB_BUILD_BEFORE_TESTS)
    set_tests_properties(
        ctest_fmi_import_me_test
        ctest_fmi_import_cs_test
        ctest_fmi_import_xml_test
        ctest_fmi_import_xml_test_empty
        ctest_fmi_import_xml_test_mf
        ctest_fmi1_capi_cs_test
        ctest_fmi1_capi_me_test
        ctest_fmi1_logger_test_run
        ctest_fmi1_xml_parsing_test
        ctest_fmi1_type_definitions_test
        ctest_fmi1_variable_bad_type_variability_test
        PROPERTIES DEPENDS ctest_build_all)
endif()

set_tests_properties(
    ctest_fmi_import_xml_test
    PROPERTIES DEPENDS ctest_fmi_import_cs_test
)
