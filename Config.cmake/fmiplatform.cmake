#    Copyright (C) 2017 Modelon AB
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

# fmi_platform(platform) sets platform to one of the by FMI defined platforms
# see https://fmi-standard.org/docs/3.0/
function(fmi_platform platform fmi_version)
    if(fmi_version EQUAL 1 OR fmi_version EQUAL 2)
        set(FMI_1_OR_2 TRUE)
    else()
        set(FMI_1_OR_2 FALSE)
    endif()

    # set os tag
    if(CMAKE_HOST_WIN32 AND FMI_1_OR_2)
        set(operatingsystem "win")
    else ()
        string(TOLOWER "${CMAKE_HOST_SYSTEM_NAME}" operatingsystem)
    endif ()

    # set arch tag
    if(FMI_1_OR_2)
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(machine "32")
        else ()
            set(machine "64")
        endif ()
    else ()
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
            set(machine "aarch64")
        elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm32")
            set(machine "aarch32")
        elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "IA64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "EM64T")
            set(machine "x86_64")
        else ()
            set(machine "${CMAKE_SYSTEM_PROCESSOR}")
            string(TOLOWER "${machine}" machine)
        endif ()
    endif ()

    # final tag
    if(FMI_1_OR_2)
        set(${platform} ${operatingsystem}${machine} PARENT_SCOPE)
    else()
        set(${platform} ${machine}-${operatingsystem} PARENT_SCOPE)
    endif()

endfunction()
