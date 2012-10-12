#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

include_directories(${RTTESTDIR}/FMI1)

add_executable (fmi1_capi_cs_test ${RTTESTDIR}/FMI1/fmi1_capi_cs_test.c )
target_link_libraries (fmi1_capi_cs_test  ${FMICAPI_LIBRARIES})

add_executable (fmi1_capi_me_test ${RTTESTDIR}/FMI1/fmi1_capi_me_test.c )
target_link_libraries (fmi1_capi_me_test  ${FMICAPI_LIBRARIES})

#Defines for the test FMUs
set(FMU_DUMMY_ME_MODEL_IDENTIFIER BouncingBall) #This must be the same as in the xml-file
set(FMU_DUMMY_CS_MODEL_IDENTIFIER BouncingBall) #This must be the same as in the xml-file
set(FMU_DUMMY_MF_MODEL_IDENTIFIER BouncingBall_malformed) #This must be the same as in the xml-file

set(FMU_DUMMY_FOLDER ${RTTESTDIR}/FMI1/fmu_dummy)

set(FMU_DUMMY_ME_SOURCE
  ${FMU_DUMMY_FOLDER}/fmu1_model_me.c
)
set(FMU_DUMMY_CS_SOURCE
  ${FMU_DUMMY_FOLDER}/fmu1_model_cs.c
)
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

set(SHARED_LIBRARY_ME_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu1_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX})
set(SHARED_LIBRARY_CS_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu1_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX})

#Create FMU 1.0 ME/CS Model and generate library path to be used in test config

to_native_c_path("\"${SHARED_LIBRARY_ME_PATH}\"" DLL_OUTPUT_PATH_ME_DEFINE)
to_native_c_path("\"${SHARED_LIBRARY_CS_PATH}\"" DLL_OUTPUT_PATH_CS_DEFINE)

to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}fmu1_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX}\""
				FMU1_DLL_ME_PATH)
to_native_c_path("\"${CMAKE_CURRENT_BINARY_DIR}/\" CMAKE_INTDIR \"/${CMAKE_SHARED_LIBRARY_PREFIX}fmu1_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX}\""
				FMU1_DLL_CS_PATH)

#function(compress_fmu OUTPUT_FOLDER MODEL_IDENTIFIER FILE_NAME_CS_ME_EXT TARGET_NAME XML_PATH SHARED_LIBRARY_PATH)
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU_DUMMY_MF_MODEL_IDENTIFIER}" "mf" "fmu1_dll_me" "${XML_MF_PATH}" "${SHARED_LIBRARY_ME_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU_DUMMY_ME_MODEL_IDENTIFIER}" "me" "fmu1_dll_me" "${XML_ME_PATH}" "${SHARED_LIBRARY_ME_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU_DUMMY_CS_MODEL_IDENTIFIER}" "cs" "fmu1_dll_cs" "${XML_CS_PATH}" "${SHARED_LIBRARY_CS_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "${FMU_DUMMY_CS_MODEL_IDENTIFIER}" "cs_tc" "fmu1_dll_cs" "${XML_CS_TC_PATH}" "${SHARED_LIBRARY_CS_PATH}")

add_executable (fmi_import_xml_test ${RTTESTDIR}/FMI1/fmi_import_xml_test.cc )
target_link_libraries (fmi_import_xml_test  ${FMILIBFORTEST}  )

add_executable (fmi_import_me_test ${RTTESTDIR}/FMI1/fmi_import_me_test.c)
target_link_libraries (fmi_import_me_test  ${FMILIBFORTEST})
add_executable (fmi_import_cs_test ${RTTESTDIR}/FMI1/fmi_import_cs_test.c)
target_link_libraries (fmi_import_cs_test  ${FMILIBFORTEST})

to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU_DUMMY_ME_MODEL_IDENTIFIER}_me.fmu" FMU_ME_PATH)

to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU_DUMMY_CS_MODEL_IDENTIFIER}_cs.fmu" FMU_CS_PATH)

to_native_c_path("${TEST_OUTPUT_FOLDER}/${FMU_DUMMY_CS_MODEL_IDENTIFIER}_cs_tc.fmu" FMU_CS_TC_PATH)

# set(FMU_TEMPFOLDER ${TEST_OUTPUT_FOLDER}/tempfolder)
to_native_c_path(${TEST_OUTPUT_FOLDER}/tempfolder FMU_TEMPFOLDER)

ADD_TEST(ctest_fmi_import_me_test fmi_import_me_test ${FMU_ME_PATH} ${FMU_TEMPFOLDER})
ADD_TEST(ctest_fmi_import_cs_test fmi_import_cs_test ${FMU_CS_PATH} ${FMU_TEMPFOLDER})
ADD_TEST(ctest_fmi_import_cs_tc_test fmi_import_cs_test ${FMU_CS_TC_PATH} ${FMU_TEMPFOLDER})
# the next test relies on the output from the previous one.
ADD_TEST(ctest_fmi_import_xml_test_empty fmi_import_xml_test ${FMU_DUMMY_FOLDER})
ADD_TEST(ctest_fmi_import_xml_test fmi_import_xml_test ${FMU_TEMPFOLDER})
add_test(ctest_fmi_import_xml_test_mf fmi_import_xml_test ${TEST_OUTPUT_FOLDER}/${FMU_DUMMY_MF_MODEL_IDENTIFIER}_mf)
  set_tests_properties(ctest_fmi_import_xml_test_mf PROPERTIES WILL_FAIL TRUE)

ADD_TEST(ctest_fmi1_capi_cs_test fmi1_capi_cs_test)
ADD_TEST(ctest_fmi1_capi_me_test fmi1_capi_me_test)

##Add logger test
add_executable (fmi1_logger_test ${RTTESTDIR}/FMI1/fmi1_logger_test.c)
target_link_libraries (fmi1_logger_test  ${FMILIBFORTEST}) 

set(logger_output_file "${TEST_OUTPUT_FOLDER}/fmi1_logger_test_output.txt")
set(logger_reference_file "${RTTESTDIR}/FMI1/fmi1_logger_test_output.txt")

add_test(ctest_fmi1_logger_test_run fmi1_logger_test ${FMU_ME_PATH} ${FMU_TEMPFOLDER} ${logger_output_file})
add_test(ctest_fmi1_logger_test_check ${CMAKE_COMMAND} -E compare_files ${logger_output_file}  ${logger_reference_file})

set_target_properties(
	fmi_import_me_test 
	fmi_import_cs_test 
	fmi_import_xml_test
	fmi1_capi_cs_test
	fmi1_capi_me_test
	fmi1_logger_test
    PROPERTIES FOLDER "Test/FMI1")

SET_TESTS_PROPERTIES ( 
	ctest_fmi1_logger_test_check	
	PROPERTIES DEPENDS ctest_fmi1_logger_test_run 	
)

if(FMILIB_BUILD_BEFORE_TESTS)
	SET_TESTS_PROPERTIES ( 
		ctest_fmi_import_me_test
		ctest_fmi_import_cs_test 
		ctest_fmi_import_xml_test
		ctest_fmi_import_xml_test_empty
		ctest_fmi_import_xml_test_mf
		ctest_fmi1_capi_cs_test
		ctest_fmi1_capi_me_test
		ctest_fmi1_logger_test_run
		PROPERTIES DEPENDS ctest_build_all)
endif()

SET_TESTS_PROPERTIES ( 
	ctest_fmi_import_xml_test	
	PROPERTIES DEPENDS ctest_fmi_import_cs_test 	
)
