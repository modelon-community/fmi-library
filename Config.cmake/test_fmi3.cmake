#    Copyright (C) 2012-2023 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

include_directories(${FMIL_TEST_DIR}/FMI3)

set(FMU3_DUMMY_FOLDER ${FMIL_TEST_DIR}/FMI3/fmu_dummy)
to_native_c_path(${TEST_OUTPUT_FOLDER}/tempfolder/FMI3 FMU3_TEMPFOLDER)

set(FMU3_DUMMY_ME_SOURCE
    ${FMU3_DUMMY_FOLDER}/fmu3_model_me.c
)
set(FMU3_DUMMY_CS_SOURCE
    ${FMU3_DUMMY_FOLDER}/fmu3_model_cs.c
)
set(FMU3_DUMMY_SE_SOURCE
    ${FMU3_DUMMY_FOLDER}/fmu3_model_se.c
)
set(FMU3_DUMMY_HEADERS
    ${FMU3_DUMMY_FOLDER}/fmu3_model.h
    ${FMU3_DUMMY_FOLDER}/fmu3_model_defines.h
)

add_library(fmu3_dll_me SHARED ${FMU3_DUMMY_ME_SOURCE} ${FMU3_DUMMY_HEADERS})
add_library(fmu3_dll_cs SHARED ${FMU3_DUMMY_CS_SOURCE} ${FMU3_DUMMY_HEADERS})
add_library(fmu3_dll_se SHARED ${FMU3_DUMMY_SE_SOURCE} ${FMU3_DUMMY_HEADERS})

set(XML_ME_PATH ${FMU3_DUMMY_FOLDER}/modelDescription_me.xml)
set(XML_CS_PATH ${FMU3_DUMMY_FOLDER}/modelDescription_cs.xml)
set(XML_SE_PATH ${FMU3_DUMMY_FOLDER}/modelDescription_se.xml)
set(XML_MF_PATH ${FMU3_DUMMY_FOLDER}/modelDescription_malformed.xml)

set(FMI3_TEST_XML_DIR ${FMIL_TEST_DIR}/FMI3/parser_test_xmls)
set(DEFAULT_EXPERIMENT_MODEL_DESC_DIR                 ${FMI3_TEST_XML_DIR}/default_experiment/) # Trailing '/' necessary (for building system independent path)
set(VARIABLE_BAD_VARIABILITY_CAUSALITY_MODEL_DESC_DIR ${FMI3_TEST_XML_DIR}/variable_bad_variability_causality)
set(VARIABLE_TYPES_TEST_MODEL_DESC_DIR                ${FMI3_TEST_XML_DIR}/variable_types/float)
set(ARRAYS_MODEL_DESC_DIR                             ${FMI3_TEST_XML_DIR}/arrays)

set(SHARED_LIBRARY_ME_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu3_dll_me${CMAKE_SHARED_LIBRARY_SUFFIX})
set(SHARED_LIBRARY_CS_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu3_dll_cs${CMAKE_SHARED_LIBRARY_SUFFIX})
set(SHARED_LIBRARY_SE_PATH ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}fmu3_dll_se${CMAKE_SHARED_LIBRARY_SUFFIX})

#Create FMU 3.0 ME/CS/SE Model and generate library path

#function(compress_fmu OUTPUT_FOLDER MODEL_IDENTIFIER FILE_NAME_CS_ME_EXT TARGET_NAME XML_PATH SHARED_LIBRARY_PATH)
compress_fmu("${TEST_OUTPUT_FOLDER}" "BouncingBall3"           "me" "fmu3_dll_me" "${XML_ME_PATH}" "${SHARED_LIBRARY_ME_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "BouncingBall3"           "cs" "fmu3_dll_cs" "${XML_CS_PATH}" "${SHARED_LIBRARY_CS_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "BouncingBall3"           "se" "fmu3_dll_se" "${XML_SE_PATH}" "${SHARED_LIBRARY_SE_PATH}")
compress_fmu("${TEST_OUTPUT_FOLDER}" "BouncingBall3_malformed" "mf" "fmu3_dll_cs" "${XML_MF_PATH}" "${SHARED_LIBRARY_CS_PATH}")

to_native_c_path("${TEST_OUTPUT_FOLDER}/BouncingBall3_me.fmu" FMU3_ME_PATH)
to_native_c_path("${TEST_OUTPUT_FOLDER}/BouncingBall3_cs.fmu" FMU3_CS_PATH)
to_native_c_path("${TEST_OUTPUT_FOLDER}/BouncingBall3_se.fmu" FMU3_SE_PATH)
to_native_c_path("${TEST_OUTPUT_FOLDER}/BouncingBall3_mf.fmu" FMU3_MF_PATH)

# The path to the directory where the FMU will be unzipped.
# FIXME: Using the same unpack dir for multiple tests prevents test parallelization.
to_native_c_path("${TEST_OUTPUT_FOLDER}/Temporary" FMU_TEMPORARY_TEST_DIR)


# General functionality tests
add_executable (fmi3_xml_parsing_test ${FMIL_TEST_DIR}/FMI3/fmi3_xml_parsing_test.c)
target_link_libraries (fmi3_xml_parsing_test  ${FMILIBFORTEST})
if(FMILIB_TEST_LOCALE)
    target_compile_definitions(fmi3_xml_parsing_test PRIVATE -DFMILIB_TEST_LOCALE)
endif()

add_executable(fmi3_import_sim_me_test ${FMIL_TEST_DIR}/FMI3/fmi3_import_sim_me_test.c)
target_link_libraries(fmi3_import_sim_me_test  ${FMILIBFORTEST})

add_executable(fmi3_import_sim_cs_test ${FMIL_TEST_DIR}/FMI3/fmi3_import_sim_cs_test.c)
target_link_libraries(fmi3_import_sim_cs_test ${FMILIBFORTEST})

add_executable(fmi3_import_sim_se_test ${FMIL_TEST_DIR}/FMI3/fmi3_import_sim_se_test.c)
target_link_libraries(fmi3_import_sim_se_test ${FMILIBFORTEST})

add_executable(fmi3_import_fatal_test ${FMIL_TEST_DIR}/FMI3/fmi3_import_fatal_test.c)
target_link_libraries(fmi3_import_fatal_test ${FMILIBFORTEST})

add_executable(fmi3_import_arrays_test ${FMIL_TEST_DIR}/FMI3/fmi3_import_arrays_test.c)
target_link_libraries(fmi3_import_arrays_test ${FMILIBFORTEST})

add_executable(fmi3_import_variable_types_test ${FMIL_TEST_DIR}/FMI3/fmi3_import_variable_types_test.c)
target_link_libraries(fmi3_import_variable_types_test ${FMILIBFORTEST})

add_executable(fmi3_import_default_experiment_test ${FMIL_TEST_DIR}/FMI3/fmi3_import_default_experiment_test.c)
target_link_libraries(fmi3_import_default_experiment_test ${FMILIBFORTEST})

set_target_properties(
    fmi3_xml_parsing_test
    fmi3_import_sim_me_test
    fmi3_import_sim_cs_test
    fmi3_import_sim_se_test
    fmi3_import_variable_types_test
    fmi3_import_default_experiment_test
    PROPERTIES FOLDER "Test/FMI3"
)
set(FAIL_NAME_CHECK 0)
set(PASS_NAME_CHECK 1)

add_catch2_test(fmi3_capi_basic_test                     FMI3)
add_catch2_test(fmi3_import_start_arrays_test            FMI3)
add_catch2_test(fmi3_import_variable_test                FMI3)
add_catch2_test(fmi3_import_convenience_test             FMI3)
add_catch2_test(fmi3_import_model_description_test       FMI3)
add_catch2_test(fmi3_import_type_definitions_test        FMI3)
add_catch2_test(fmi3_import_model_structure_test         FMI3)
add_catch2_test(fmi3_variability_causality_initial_test  FMI3)
add_catch2_test(fmi3_import_bouncingball_xml_test        FMI3)
add_catch2_test(fmi3_import_interface_types_test         FMI3)
add_catch2_test(fmi3_import_unit_test                    FMI3)

add_test(ctest_fmi3_xml_parsing_test fmi3_xml_parsing_test ${FMIL_TEST_DIR}/FMI3/parser_test_xmls/)

# FIXME: Missing dependency on building the FMU
add_test(ctest_fmi3_import_sim_me_test fmi3_import_sim_me_test)
add_test(ctest_fmi3_import_sim_cs_test fmi3_import_sim_cs_test)
add_test(ctest_fmi3_import_sim_se_test fmi3_import_sim_se_test)

add_test(ctest_fmi3_import_variable_types_test
         fmi3_import_variable_types_test
         ${VARIABLE_TYPES_TEST_MODEL_DESC_DIR})
add_test(ctest_fmi3_import_fatal_test
         fmi3_import_fatal_test
         ${FMI3_TEST_XML_DIR})
add_test(ctest_fmi3_import_arrays_test
         fmi3_import_arrays_test
         ${ARRAYS_MODEL_DESC_DIR})
add_test(ctest_fmi3_import_default_experiment_test
         fmi3_import_default_experiment_test
         ${DEFAULT_EXPERIMENT_MODEL_DESC_DIR})

if(FMILIB_BUILD_BEFORE_TESTS)
    set_tests_properties(
        ctest_fmi3_xml_parsing_test
        ctest_fmi3_import_sim_me_test
        ctest_fmi3_import_sim_cs_test
        ctest_fmi3_import_sim_se_test
        ctest_fmi3_import_fatal_test
        ctest_fmi3_import_arrays_test
        ctest_fmi3_import_default_experiment_test
        PROPERTIES DEPENDS ctest_build_all)
endif()
