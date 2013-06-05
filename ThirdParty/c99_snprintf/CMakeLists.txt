#    Copyright (C) 2012 Modelon AB

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.

# #    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    FMILIB_License.txt file for more details.

#    You should have received a copy of the FMILIB_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.

if(NOT C99SNPRINTFDIR)
    set(C99SNPRINTFDIR ${FMILIBRARYHOME}/ThirdParty/c99_snprintf)
    set(C99SNPRINTF_LIBRARIES c99snprintf)

add_library(c99snprintf c99-snprintf_1.1/snprintf.c)

if(CYGWIN) 
message("not tested")
endif(CYGWIN)

endif()

