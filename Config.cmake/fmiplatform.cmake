#    Copyright (C) 2017 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

# fmi_platform(platform) sets platform to one of the by FMI defined platforms
# win32, win64, linux32, linux64, darwin32 or darwin64 depending on cmake
# generator
function(fmi_platform platform )
    if(CMAKE_HOST_WIN32) #Set to true when the host system is Windows and on cygwin.
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(${platform} win32 PARENT_SCOPE)
        else(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(${platform} win64 PARENT_SCOPE)
        endif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    elseif(CMAKE_HOST_APPLE) #Set to true when the host system is Apple OSX. 
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(${platform} darwin32 PARENT_SCOPE)
        else(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(${platform} darwin64 PARENT_SCOPE)
        endif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    elseif(CMAKE_HOST_UNIX) #Set to true when the host system is UNIX or UNIX like (i.e. APPLE and CYGWIN). 
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(${platform} linux32 PARENT_SCOPE)
        else(CMAKE_SIZEOF_VOID_P EQUAL 4)
            set(${platform} linux64 PARENT_SCOPE)
        endif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    else(CMAKE_HOST_WIN32)
        message(SEND_ERROR "Target ${platform} is not supported by the tests")
        set(${platform} not_supported)
    endif()
endfunction()
