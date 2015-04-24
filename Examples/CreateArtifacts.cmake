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



function(produce_CC_Artifacts FMU_ID FMU_NAME FMUEXPORTDIR STOPT STEPS INPUTFLAG)
		
	foreach(variants ${FMU_VARIANTS})
		set(WRKDIR ${FMIExamples_BINARY_DIR}/${FMI_PLATFORM}/${FMU_NAME}_${variants})
		file(MAKE_DIRECTORY ${WRKDIR})

		message(STATUS "Generating CC Artifacts for ${FMI_PLATFORM}/${FMU_NAME}_${variants}")
			
		add_custom_target("CreateCrossCheckRef_${FMU_NAME}_${variants}" ALL
				DEPENDS ${WRKDIR}/${FMU_NAME}_${variants}_ref.csv
				SOURCES ${WRKDIR}/${FMU_NAME}_${variants}${CCBATCHEXT})

		IF(INPUTFLAG)
			message(STATUS "Detected input!")
			set(INPUTOPT "-i ${FMU_NAME}_${variants}_in.csv")
			Execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${FMU_LOC}/${FMU_NAME}_${variants}/resources/${FMU_NAME}_${variants}_in.csv" "${WRKDIR}/${FMU_NAME}_${variants}_in.csv")
			INSTALL (FILES
			${WRKDIR}/${FMU_NAME}_${variants}_in.csv
			DESTINATION CrossCheck/${FMI_PLATFORM}/${FMU_NAME}_${variants}
		)			
		ELSE()
			set(INPUTOPT "")
		ENDIF()

		file(WRITE ${WRKDIR}/${FMU_NAME}_${variants}${CCBATCHEXT} "${fmuCheck} -o ${FMU_NAME}_${variants}_ref.csv -e ${FMU_NAME}_${variants}_ref.log -s ${STOPT} -h ${STEPS} ${INPUTOPT} ${FMU_NAME}_${variants}.fmu ")

		file(WRITE ${WRKDIR}/${FMU_NAME}_${variants}_ref.opt "StartTime, 0.0
StopTime, ${STOPT}
StepSize, ${STEPS}")

				
		if(WIN32 OR WIN64)
		
		ADD_CUSTOM_COMMAND(
			OUTPUT ${WRKDIR}/${FMU_NAME}_${variants}_ref.csv
			COMMAND "${CMAKE_COMMAND}" -E copy "${FMU_LOC}/${FMU_NAME}_${variants}.fmu" "${WRKDIR}/${FMU_NAME}_${variants}.fmu"
			COMMAND "${CMAKE_COMMAND}" -E time "${FMU_NAME}_${variants}${CCBATCHEXT}"
			WORKING_DIRECTORY ${WRKDIR}
			DEPENDS ${FMU_LOC}/${FMU_NAME}_${variants}.fmu)
			
		else()
		
		ADD_CUSTOM_COMMAND(
			OUTPUT ${WRKDIR}/${FMU_NAME}_${variants}_ref.csv
			COMMAND "${CMAKE_COMMAND}" -E copy "${FMU_LOC}/${FMU_NAME}_${variants}.fmu" "${WRKDIR}/${FMU_NAME}_${variants}.fmu"
			COMMAND "${CMAKE_COMMAND}" -E time "chmod +x ${FMU_NAME}_${variants}${CCBATCHEXT}"
			COMMAND "${CMAKE_COMMAND}" -E time ./${FMU_NAME}_${variants}${CCBATCHEXT}
			WORKING_DIRECTORY ${WRKDIR}
			DEPENDS ${FMU_LOC}/${FMU_NAME}_${variants}.fmu)
		
		endif()
			
		INSTALL (FILES
			${WRKDIR}/${FMU_NAME}_${variants}${CCBATCHEXT}
			${WRKDIR}/${FMU_NAME}_${variants}_ref.csv
			${WRKDIR}/${FMU_NAME}_${variants}_ref.log
			${WRKDIR}/${FMU_NAME}_${variants}_ref.opt
			${WRKDIR}/${FMU_NAME}_${variants}.fmu
			DESTINATION CrossCheck/${FMI_PLATFORM}/${FMU_NAME}_${variants}
		)
	endforeach(variants)
endfunction(produce_CC_Artifacts)
