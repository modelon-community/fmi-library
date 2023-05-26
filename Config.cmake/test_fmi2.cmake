#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

include_directories(${FMIL_TEST_DIR}/FMI2)


#Defines for the test FMUs
set(FMU2_DUMMY_ME_MODEL_IDENTIFIER BouncingBall2) #This must be the same as in the xml-file
set(FMU2_DUMMY_CS_MODEL_IDENTIFIER BouncingBall2) #This must be the same as in the xml-file
set(FMU2_DUMMY_MF_MODEL_IDENTIFIER BouncingBall2_malformed) #This must be the same as in the xml-file

set(FMU2_DUMMY_FOLDER ${FMIL_TEST_DIR}/FMI2/fmu_dummy)
to_native_c_path(${TEST_OUTPUT_FOLDER}/tempfolder/FMI2 FMU2_TEMPFOLDER)

set(FMU2_DUMMY_ME_SOURCE
    ${FMU2_DUMMY_FOLDER}/fmu2_model_me.c
)
set(FMU2_DUMMY_CS_SOURCE
    ${FMU2_DUMMY_FOLDER}/fmu2_model_cs.c
)
set(FMU2_DUMMY_HEADERS
    ${FMU2_DUMMY_FOLDER}/fmu2_model.h
    ${FMU2_DUMMY_FOLDER}/fmu2_model_defines.h
)

add_library(fmu2_dll_me SHARED ${FMU2_DUMMY_ME_SOURCE} ${FMU2_DUMMY_HEADERS})
add_library(fmu2_dll_cs SHARED ${FMU2_DUMMY_CS_SOURCE} ${FMU2_DUMMY_HEADERS})

set(XML_ME_PATH ${FMU2_DUMMY_FOLDER}/modelDescription_me.xml)
set(XML_CS_PATH ${FMU2_DUMMY_FOLDER}/modelDescription_cs.xml)
set(XML_MF_PATH ${FMU2_DUMMY_FOLDER}/modelDescription_malformed.xml)

set(VARIALBE_TEST_MODEL_DESC_DIR
    ${FMIL_TEST_DIR}/FMI2/parser_test_xmls/variable_test)
set(FMI2_PARSER_TEST_XMLS_DIR
    ${FMIL_TEST_DIR}/FMI2/parser_test_xmls)
set(DEFAULT_EXPERIMENT_MODEL_DESC_DIR
    ${FMIL_TEST_DIR}/FMI2/parser_test_xmls/default_experiment/) # Trailing '/' necessary (for building system independent path)
set(VARIABLE_NO_TYPE_MODEL_DESC_DIR
    ${FMIL_TEST_DIR}/FMI2/parser_test_xmls/variable_no_type)
set(VARIABLE_BAD_VARIABILITY_CAUSALITY_MODEL_DESC_DIR
    ${FMIL_TEST_DIR}/FMI2/parser_test_xmls/variable_bad_variability_causality)
set(VARIABLE_BAD_TYPE_VARIABILITY_MODEL_DESC_DIR
    ${FMIL_TEST_DIR}/FMI2/parser_test_xmls/variable_bad_type_variability)
set(TYPE_DEFINITIONS_MODEL_DESC_DIR
    ${FMIL_TEST_DIR}/FMI2/parser_test_xmls/type_definitions)

set(SHARED_LIBRARY_ME_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu2_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX})
set(SHARED_LIBRARY_CS_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu2_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX})

#Create FMU 2.0 ME/CS Model and generate library path

#function(compress_fmu OUTPUT_FOLDER MODEL_IDENTIFIER FILE_NAME_CS_ME_EXT TARGET_NAME XML_PATH SHARED_LIBRARY_PATH)
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU2_DUMMY_ME_MODEL_IDENTIFIER}" "me" "fmu2_dll_me" "${XML_ME_PATH}" "${SHARED_LIBRARY_ME_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU2_DUMMY_CS_MODEL_IDENTIFIER}" "cs" "fmu2_dll_cs" "${XML_CS_PATH}" "${SHARED_LIBRARY_CS_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU2_DUMMY_MF_MODEL_IDENTIFIER}" "mf" "fmu2_dll_cs" "${XML_MF_PATH}" "${SHARED_LIBRARY_CS_PATH}")

to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU2_DUMMY_ME_MODEL_IDENTIFIER}_me.fmu" FMU2_ME_PATH)
to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU2_DUMMY_CS_MODEL_IDENTIFIER}_cs.fmu" FMU2_CS_PATH)
to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU2_DUMMY_CS_MODEL_IDENTIFIER}_mf.fmu" FMU2_MF_PATH)

add_executable(fmi2_xml_parsing_test ${FMIL_TEST_DIR}/FMI2/fmi2_xml_parsing_test.c)
target_link_libraries(fmi2_xml_parsing_test  ${FMILIBFORTEST})
if(FMILIB_TEST_LOCALE)
    target_compile_definitions(fmi2_xml_parsing_test PRIVATE -DFMILIB_TEST_LOCALE)
endif()

add_executable(fmi2_import_xml_test ${FMIL_TEST_DIR}/FMI2/fmi2_import_xml_test.cc)
target_link_libraries(fmi2_import_xml_test ${FMILIBFORTEST})
add_executable(fmi2_import_me_test ${FMIL_TEST_DIR}/FMI2/fmi2_import_me_test.c)
target_link_libraries(fmi2_import_me_test ${FMILIBFORTEST})
add_executable(fmi2_import_cs_test ${FMIL_TEST_DIR}/FMI2/fmi2_import_cs_test.c)
target_link_libraries(fmi2_import_cs_test ${FMILIBFORTEST})
add_executable(fmi2_import_variable_test ${FMIL_TEST_DIR}/FMI2/fmi2_import_variable_test.c)
target_link_libraries(fmi2_import_variable_test ${FMILIBFORTEST})
add_executable(fmi2_import_model_structure_test ${FMIL_TEST_DIR}/FMI2/fmi2_import_model_structure_test.c)
target_link_libraries(fmi2_import_model_structure_test ${FMILIBFORTEST})
add_executable(fmi2_import_default_experiment_test ${FMIL_TEST_DIR}/FMI2/fmi2_import_default_experiment_test.c)
target_link_libraries(fmi2_import_default_experiment_test ${FMILIBFORTEST})
add_executable(fmi2_type_definitions_test ${FMIL_TEST_DIR}/FMI2/fmi2_import_type_definitions_test.c)
target_link_libraries(fmi2_type_definitions_test ${FMILIBFORTEST})
add_executable(fmi2_variable_no_type_test
               ${FMIL_TEST_DIR}/FMI2/fmi2_variable_no_type_test.c)
target_link_libraries(fmi2_variable_no_type_test ${FMILIBFORTEST})
add_executable(fmi2_variable_bad_variability_causality_test
               ${FMIL_TEST_DIR}/FMI2/fmi2_variable_bad_variability_causality_test.c)
target_link_libraries(fmi2_variable_bad_variability_causality_test ${FMILIBFORTEST})
add_executable(fmi2_variable_bad_type_variability_test
               ${FMIL_TEST_DIR}/FMI2/fmi2_variable_bad_type_variability_test.c)
target_link_libraries(fmi2_variable_bad_type_variability_test ${FMILIBFORTEST})
add_executable(fmi2_enum_test ${FMIL_TEST_DIR}/FMI2/fmi2_enum_test.c)
target_link_libraries(fmi2_enum_test ${FMILIBFORTEST})
add_executable(fmi2_import_options_test ${FMIL_TEST_DIR}/FMI2/fmi2_import_options_test.c)
target_link_libraries (fmi2_import_options_test ${FMILIBFORTEST})

set_target_properties(
    fmi2_xml_parsing_test
    fmi2_import_xml_test
    fmi2_import_me_test fmi2_import_cs_test
    fmi2_import_variable_test
    fmi2_import_default_experiment_test
    fmi2_import_options_test
    PROPERTIES FOLDER "Test/FMI2"
)
set(FAIL_NAME_CHECK 0)
set(PASS_NAME_CHECK 1)

add_test(ctest_fmi2_xml_parsing_test fmi2_xml_parsing_test ${FMIL_TEST_DIR}/FMI2/parser_test_xmls/)
add_test(ctest_fmi2_import_xml_test_empty fmi2_import_xml_test ${FMU2_DUMMY_FOLDER})
add_test(ctest_fmi2_import_xml_test_me fmi2_import_xml_test ${TEST_OUTPUT_FOLDER}/${FMU2_DUMMY_ME_MODEL_IDENTIFIER}_me)
add_test(ctest_fmi2_import_xml_test_cs fmi2_import_xml_test ${TEST_OUTPUT_FOLDER}/${FMU2_DUMMY_CS_MODEL_IDENTIFIER}_cs)
add_test(ctest_fmi2_import_xml_test_mf fmi2_import_xml_test ${TEST_OUTPUT_FOLDER}/${FMU2_DUMMY_MF_MODEL_IDENTIFIER}_mf)
set_tests_properties(ctest_fmi2_import_xml_test_mf PROPERTIES WILL_FAIL TRUE)
add_test(ctest_fmi2_import_test_me fmi2_import_me_test ${FMU2_ME_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi2_import_test_cs fmi2_import_cs_test ${FMU2_CS_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi2_import_test_options fmi2_import_options_test ${FMU2_ME_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi2_import_variable_test
         fmi2_import_variable_test
         ${VARIALBE_TEST_MODEL_DESC_DIR})
add_test(ctest_fmi2_import_model_structure_test
         fmi2_import_model_structure_test
         ${FMI2_PARSER_TEST_XMLS_DIR})
add_test(ctest_fmi2_import_default_experiment_test
         fmi2_import_default_experiment_test
         ${DEFAULT_EXPERIMENT_MODEL_DESC_DIR})
add_test(ctest_fmi2_variable_no_type_test
         fmi2_variable_no_type_test
         ${VARIABLE_NO_TYPE_MODEL_DESC_DIR})
add_test(ctest_fmi2_variable_bad_variability_causality_test
         fmi2_variable_bad_variability_causality_test
         ${VARIABLE_BAD_VARIABILITY_CAUSALITY_MODEL_DESC_DIR})
add_test(ctest_fmi2_variable_bad_type_variability_test
         fmi2_variable_bad_type_variability_test
         ${VARIABLE_BAD_TYPE_VARIABILITY_MODEL_DESC_DIR})
add_test(ctest_fmi2_type_definitions_test
         fmi2_type_definitions_test
         ${TYPE_DEFINITIONS_MODEL_DESC_DIR})
add_test(ctest_fmi2_enum_test
         fmi2_enum_test)

if(FMILIB_BUILD_BEFORE_TESTS)
    set_tests_properties(
        ctest_fmi2_xml_parsing_test
        ctest_fmi2_import_xml_test_me
        ctest_fmi2_import_xml_test_cs
        ctest_fmi2_import_xml_test_mf
        ctest_fmi2_import_xml_test_empty
        ctest_fmi2_import_test_me
        ctest_fmi2_import_test_cs
        ctest_fmi2_import_variable_test
        ctest_fmi2_import_model_structure_test
        ctest_fmi2_import_default_experiment_test
        ctest_fmi2_variable_no_type_test
        ctest_fmi2_type_definitions_test
        ctest_fmi2_enum_test
        ctest_fmi2_variable_bad_variability_causality_test
        ctest_fmi2_variable_bad_type_variability_test
        PROPERTIES DEPENDS ctest_build_all)
endif()
