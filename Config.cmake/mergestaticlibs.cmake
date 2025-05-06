#    Copyright (C) 2012 Modelon AB
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

# Merge_static_libs(outlib lib1 lib2 ... libn) merges a number of static
# libs into a single static library.
function(merge_static_libs outlib)
    set(libs ${ARGV})
    list(REMOVE_AT libs 0)
    # Create a dummy file that the target will depend on
    set(dummyfile ${CMAKE_CURRENT_BINARY_DIR}/${outlib}_dummy.c)
    file(WRITE ${dummyfile} "const char* dummy = \"${dummyfile}\";")

    add_library(${outlib} STATIC ${dummyfile})

    # First get the file names of the libraries to be merged
    foreach(lib ${libs})
        get_target_property(libtype ${lib} TYPE)
        if(NOT libtype STREQUAL "STATIC_LIBRARY")
            message(FATAL_ERROR "Merge_static_libs can only process static libraries\n\tlibraries: ${lib}\n\tlibtype ${libtype}")
        endif()
        list(APPEND libfiles $<TARGET_FILE:${lib}>)
    endforeach()
    message(STATUS "will be merging ${libfiles}")
    list(REMOVE_DUPLICATES libfiles)

    # Now the easy part for MSVC and for MAC
    if(MSVC)
        # lib.exe does the merging of libraries just need to conver the list into string
        set_target_properties(${outlib} PROPERTIES STATIC_LIBRARY_OPTIONS "${libfiles}")
    elseif(APPLE)
        # Use OSX's libtool to merge archives
        find_program(CMAKE_LIBTOOL NAMES libtool)
        if (NOT CMAKE_LIBTOOL)
            message(FATAL_ERROR "Cannot find libtool")
        endif ()
        add_custom_command(TARGET ${outlib} POST_BUILD
            COMMAND rm
                    "$<TARGET_FILE:${outlib}>"
            COMMAND ${CMAKE_LIBTOOL} -static -o
                    "$<TARGET_FILE:${outlib}>"
                    ${libfiles}
            COMMAND_EXPAND_LISTS
            VERBATIM
        )
    else() # general UNIX - need to "ar -x" and then "ar -ru"
        foreach(libtarget ${libs})
            set(objlistfile  ${CMAKE_CURRENT_BINARY_DIR}/${libtarget}.objlist)  # Contains a list of the object files
            set(objdir       ${CMAKE_CURRENT_BINARY_DIR}/${libtarget}.objdir)   # Directory where to extract object files
            set(objlistcmake ${objlistfile}.cmake)                              # Script that extracts object files and creates the listing file
            # we only need to extract files once
            if(${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cmake.check_cache IS_NEWER_THAN ${objlistcmake})
#-------------------------------------------------------------------------------
                file(WRITE ${objlistcmake}
"# Extract object files from the library
message(STATUS \"Extracting object files from \${libpath}\")
EXECUTE_PROCESS(COMMAND ${CMAKE_AR} -x \${libpath}
                WORKING_DIRECTORY ${objdir})
# Save the list of object files
EXECUTE_PROCESS(COMMAND ls .
                OUTPUT_FILE ${objlistfile}
                WORKING_DIRECTORY ${objdir})")
#-------------------------------------------------------------------------------
                file(MAKE_DIRECTORY ${objdir})
                add_custom_command(
                    OUTPUT ${objlistfile}
                    COMMAND ${CMAKE_COMMAND} -Dlibpath="$<TARGET_FILE:${libtarget}>" -P ${objlistcmake}
                    DEPENDS ${libtarget})
            endif()
            list(APPEND extrafiles "${objlistfile}")
            # relative path is needed by ar under MSYS
            file(RELATIVE_PATH objlistfilerpath ${objdir} ${objlistfile})
            add_custom_command(TARGET ${outlib} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Running: ${CMAKE_AR} cruU $<TARGET_FILE:${outlib}> @${objlistfilerpath}"
                COMMAND ${CMAKE_AR} cruU "$<TARGET_FILE:${outlib}>" @"${objlistfilerpath}"
                WORKING_DIRECTORY ${objdir})
        endforeach()
        add_custom_command(TARGET ${outlib} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Running: ${CMAKE_RANLIB} $<TARGET_FILE:${outlib}>"
            COMMAND ${CMAKE_RANLIB} $<TARGET_FILE:${outlib}>)
        add_custom_command(TARGET ${outlib} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Running: ${CMAKE_RANLIB} $<TARGET_FILE:${outlib}>"
            COMMAND ${CMAKE_RANLIB} $<TARGET_FILE:${outlib}>)
    endif()
    file(WRITE ${dummyfile}.base "const char* ${outlib}_sublibs=\"${libs}\";")
    add_custom_command(
        OUTPUT  ${dummyfile}
        COMMAND ${CMAKE_COMMAND} -E copy ${dummyfile}.base ${dummyfile}
        DEPENDS ${libs} ${extrafiles})
endfunction()
