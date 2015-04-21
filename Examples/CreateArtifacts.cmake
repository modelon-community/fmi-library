#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

set(FMUEXAMPLE_CC_DIR ${FMILIB_INSTALL_PREFIX}/CrossCheck)

set(FMI_COMPLIANCE_CHECKER  ${FMILIB_THIRDPARTYLIBS}/Checker CACHE PATH "Path to the FMI ComplianceChecker executable(s)" )

set(fmuCheck ${FMI_COMPLIANCE_CHECKER}/fmuCheck.${FMI_PLATFORM})
set(FMU_LOC "${EXPORT_OUTPUT_FOLDER}")

if(WIN32 OR WIN64)
	set(CCBATCHEXT "_ref.bat")
else()
	set(CCBATCHEXT "_ref.sh")
endif()

set(FMU_VARIANTS
		fmi1_me
		fmi1_cs
		fmi2
	)



function(produce_CC_Artifacts FMU_ID FMU_NAME FMUEXPORTDIR CCOPTS)
		
	foreach(variants ${FMU_VARIANTS})
		set(WRKDIR ${FMIExamples_BINARY_DIR}/${FMI_PLATFORM}/${FMU_NAME}_${variants})
		file(MAKE_DIRECTORY ${WRKDIR})
		file(WRITE ${WRKDIR}/${FMU_NAME}_${variants}${CCBATCHEXT} "${fmuCheck} -o ${FMU_NAME}_${variants}_ref.csv -e ${FMU_NAME}_${variants}_ref.log ${CCOPTS} ${FMU_NAME}_${variants}.fmu ")
		
		message(STATUS "Generating CC Artifacts for ${FMI_PLATFORM}/${FMU_NAME}_${variants}")
		#Execute_process(
		#	COMMAND "${CMAKE_COMMAND}" -E ${fmuCheck} -o result.csv -e log.txt ${CCOPTS} ${FMU_LOC}/${FMU_NAME}_${variants}.fmu
		#	WORKING_DIRECTORY ${WRKDIR}
		#	RESULT_VARIABLE passed)

			
		add_custom_target("CreateCrossCheckRef_${FMU_NAME}_${variants}" ALL
				DEPENDS ${WRKDIR}/${FMU_NAME}_${variants}_ref.csv
			#	COMMAND "${CMAKE_COMMAND}" -E time ${CCBATCHEXT}				
			#	WORKING_DIRECTORY ${WRKDIR}
				SOURCES ${WRKDIR}/${FMU_NAME}_${variants}${CCBATCHEXT})
		
		ADD_CUSTOM_COMMAND(
			OUTPUT ${WRKDIR}/${FMU_NAME}_${variants}_ref.csv
			COMMAND "${CMAKE_COMMAND}" -E copy "${FMU_LOC}/${FMU_NAME}_${variants}.fmu" "${WRKDIR}/${FMU_NAME}_${variants}.fmu"
			COMMAND "${CMAKE_COMMAND}" -E time ${FMU_NAME}_${variants}${CCBATCHEXT}
			WORKING_DIRECTORY ${WRKDIR}
			#RESULT_VARIABLE passed
			#if(passed EQUAL 0)
			#	file(WRITE passed "")
			#else()
			#	file(WRITE failed "")
			#message(FATAL_ERROR "Simulation of ${FMU_NAME}_${variants}.fmu failed")
			#endif()
			DEPENDS ${FMU_LOC}/${FMU_NAME}_${variants}.fmu)
			
		INSTALL (FILES
			${WRKDIR}/${FMU_NAME}_${variants}${CCBATCHEXT}
			${WRKDIR}/${FMU_NAME}_${variants}_ref.csv
			${WRKDIR}/${FMU_NAME}_${variants}_ref.log
			${WRKDIR}/${FMU_NAME}_${variants}.fmu
			DESTINATION CrossCheck/${FMI_PLATFORM}/${FMU_NAME}_${variants}
		)
		#Execute_process(
		#	COMMAND simulate.bat
		#	RESULT_VARIABLE passed)
		#Execute_process(
		#	COMMAND ${CHECKER} -o result.csv -e log.txt -h 1e0 -s 25 ${TESTFMUSDIR}/
		#	RESULT_VARIABLE passed)
		#if(passed EQUAL 0)
		#	file(WRITE passed "")
		#else()
		#	file(WRITE failed "")
		#	message(FATAL_ERROR "Simulation of ${FMU_NAME}_${variants}.fmu failed")
		#endif()
		#ADD_TEST(CCArtifacts_${FMU_NAME}_${variants} ${WRKDIR}/${CCBATCHEXT})
	endforeach(variants)
endfunction(produce_CC_Artifacts)
