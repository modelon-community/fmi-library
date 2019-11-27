#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

include_directories(${RTTESTDIR}/FMI3)


#Defines for the test FMUs
set(FMU3_DUMMY_ME_MODEL_IDENTIFIER BouncingBall3) #This must be the same as in the xml-file
set(FMU3_DUMMY_CS_MODEL_IDENTIFIER BouncingBall3) #This must be the same as in the xml-file
set(FMU3_DUMMY_MF_MODEL_IDENTIFIER BouncingBall3_malformed) #This must be the same as in the xml-file

set(FMU3_DUMMY_FOLDER ${RTTESTDIR}/FMI3/fmu_dummy)
to_native_c_path(${TEST_OUTPUT_FOLDER}/tempfolder/FMI3 FMU3_TEMPFOLDER)

set(FMU3_DUMMY_ME_SOURCE
    ${FMU3_DUMMY_FOLDER}/fmu3_model_me.c
)
set(FMU3_DUMMY_CS_SOURCE
    ${FMU3_DUMMY_FOLDER}/fmu3_model_cs.c
)
set(FMU3_DUMMY_HEADERS
    ${FMU3_DUMMY_FOLDER}/fmu3_model.h
    ${FMU3_DUMMY_FOLDER}/fmu3_model_defines.h
)

add_library(fmu3_dll_me SHARED ${FMU3_DUMMY_ME_SOURCE} ${FMU3_DUMMY_HEADERS})
add_library(fmu3_dll_cs SHARED ${FMU3_DUMMY_CS_SOURCE} ${FMU3_DUMMY_HEADERS})

set(XML_ME_PATH ${FMU3_DUMMY_FOLDER}/modelDescription_me.xml)
set(XML_CS_PATH ${FMU3_DUMMY_FOLDER}/modelDescription_cs.xml)
set(XML_MF_PATH ${FMU3_DUMMY_FOLDER}/modelDescription_malformed.xml)

set(VARIABLE_TEST_MODEL_DESC_DIR
    ${RTTESTDIR}/FMI3/parser_test_xmls/variable_test)
set(VARIABLE_BAD_VARIABILITY_CAUSALITY_MODEL_DESC_DIR
    ${RTTESTDIR}/FMI3/parser_test_xmls/variable_bad_variability_causality)
set(VAR_TYPE_TEST_MODEL_DESC_DIR
    ${RTTESTDIR}/FMI3/parser_test_xmls/variable_types/float)

set(SHARED_LIBRARY_ME_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu3_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX})
set(SHARED_LIBRARY_CS_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu3_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX})

#Create FMU 3.0 ME/CS Model and generate library path

to_native_c_path("\"${SHARED_LIBRARY_ME_PATH}\"" DLL_OUTPUT_PATH_ME_DEFINE)
to_native_c_path("\"${SHARED_LIBRARY_CS_PATH}\"" DLL_OUTPUT_PATH_CS_DEFINE)

to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}fmu3_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX}\""
                 fmu3_DLL_ME_PATH)
to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}fmu3_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX}\""
                 fmu3_DLL_CS_PATH)

#function(compress_fmu OUTPUT_FOLDER MODEL_IDENTIFIER FILE_NAME_CS_ME_EXT TARGET_NAME XML_PATH SHARED_LIBRARY_PATH)
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU3_DUMMY_ME_MODEL_IDENTIFIER}" "me" "fmu3_dll_me" "${XML_ME_PATH}" "${SHARED_LIBRARY_ME_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU3_DUMMY_CS_MODEL_IDENTIFIER}" "cs" "fmu3_dll_cs" "${XML_CS_PATH}" "${SHARED_LIBRARY_CS_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU3_DUMMY_MF_MODEL_IDENTIFIER}" "mf" "fmu3_dll_cs" "${XML_MF_PATH}" "${SHARED_LIBRARY_CS_PATH}")

to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU3_DUMMY_ME_MODEL_IDENTIFIER}_me.fmu" FMU3_ME_PATH)
to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU3_DUMMY_CS_MODEL_IDENTIFIER}_cs.fmu" FMU3_CS_PATH)
to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU3_DUMMY_CS_MODEL_IDENTIFIER}_mf.fmu" FMU3_MF_PATH)

add_executable (fmi3_xml_parsing_test ${RTTESTDIR}/FMI3/fmi3_xml_parsing_test.c)
target_link_libraries (fmi3_xml_parsing_test  ${FMILIBFORTEST})
add_executable (fmi3_import_xml_test ${RTTESTDIR}/FMI3/fmi3_import_xml_test.c)
target_link_libraries (fmi3_import_xml_test  ${FMILIBFORTEST})
add_executable (fmi3_import_me_test ${RTTESTDIR}/FMI3/fmi3_import_me_test.c)
target_link_libraries (fmi3_import_me_test  ${FMILIBFORTEST})
add_executable (fmi3_import_cs_test ${RTTESTDIR}/FMI3/fmi3_import_cs_test.c)
target_link_libraries (fmi3_import_cs_test  ${FMILIBFORTEST})
add_executable(fmi3_import_variable_test ${RTTESTDIR}/FMI3/fmi3_import_variable_test.c)
target_link_libraries(fmi3_import_variable_test ${FMILIBFORTEST})
add_executable(fmi3_import_variable_types_test ${RTTESTDIR}/FMI3/fmi3_import_variable_types_test.c)
target_link_libraries(fmi3_import_variable_types_test ${FMILIBFORTEST})
add_executable(fmi3_variable_bad_variability_causality_test
               ${RTTESTDIR}/FMI3/fmi3_variable_bad_variability_causality_test.c)
target_link_libraries(fmi3_variable_bad_variability_causality_test ${FMILIBFORTEST})
add_executable(fmi3_enum_test ${RTTESTDIR}/FMI3/fmi3_enum_test.c)
target_link_libraries(fmi3_enum_test ${FMILIBFORTEST})

set_target_properties(
    fmi3_xml_parsing_test
    fmi3_import_xml_test
    fmi3_import_me_test fmi3_import_cs_test
    fmi3_import_variable_test
    fmi3_import_variable_types_test
    PROPERTIES FOLDER "Test/FMI3"
)
set(FAIL_NAME_CHECK 0)
set(PASS_NAME_CHECK 1)

set(EXPECT_ERROR 1)
add_test(ctest_fmi3_xml_parsing_test fmi3_xml_parsing_test ${RTTESTDIR}/FMI3/parser_test_xmls/)
ADD_TEST(ctest_fmi3_import_xml_test_empty fmi3_import_xml_test ${FMU3_DUMMY_FOLDER} ${EXPECT_ERROR})
add_test(ctest_fmi3_import_xml_test_me fmi3_import_xml_test ${TEST_OUTPUT_FOLDER}/${FMU3_DUMMY_ME_MODEL_IDENTIFIER}_me)
add_test(ctest_fmi3_import_xml_test_cs fmi3_import_xml_test ${TEST_OUTPUT_FOLDER}/${FMU3_DUMMY_CS_MODEL_IDENTIFIER}_cs)
add_test(ctest_fmi3_import_xml_test_mf fmi3_import_xml_test ${TEST_OUTPUT_FOLDER}/${FMU3_DUMMY_MF_MODEL_IDENTIFIER}_mf)
set_tests_properties(ctest_fmi3_import_xml_test_mf PROPERTIES WILL_FAIL TRUE)
add_test(ctest_fmi3_import_test_me fmi3_import_me_test ${FMU3_ME_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi3_import_test_cs fmi3_import_cs_test ${FMU3_CS_PATH} ${FMU_TEMPFOLDER})
add_test(ctest_fmi3_import_variable_test
         fmi3_import_variable_test
         ${VARIABLE_TEST_MODEL_DESC_DIR})
add_test(ctest_fmi3_import_var_type_test
         fmi3_import_variable_types_test
         ${VAR_TYPE_TEST_MODEL_DESC_DIR})
add_test(ctest_fmi3_variable_bad_variability_causality_test
         fmi3_variable_bad_variability_causality_test
         ${VARIABLE_BAD_VARIABILITY_CAUSALITY_MODEL_DESC_DIR})
add_test(ctest_fmi3_enum_test
         fmi3_enum_test)

if(FMILIB_BUILD_BEFORE_TESTS)
    SET_TESTS_PROPERTIES (
        ctest_fmi3_xml_parsing_test
        ctest_fmi3_import_xml_test_me
        ctest_fmi3_import_xml_test_cs
        ctest_fmi3_import_xml_test_mf
        ctest_fmi3_import_xml_test_empty
        ctest_fmi3_import_test_me
        ctest_fmi3_import_test_cs
        ctest_fmi3_import_variable_test
        ctest_fmi3_enum_test
        ctest_fmi3_variable_bad_variability_causality_test
        PROPERTIES DEPENDS ctest_build_all)
endif()
