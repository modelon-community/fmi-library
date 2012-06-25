/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    FMILIB_License.txt file for more details.

    You should have received a copy of the FMILIB_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include "JM/jm_vector.h"

#define JM_TEMPLATE_INSTANCE_TYPE char
#include "JM/jm_vector_template.h"

#undef JM_TEMPLATE_INSTANCE_TYPE
#define JM_TEMPLATE_INSTANCE_TYPE int
#include "JM/jm_vector_template.h"

#undef JM_TEMPLATE_INSTANCE_TYPE
#define JM_TEMPLATE_INSTANCE_TYPE double
#include "JM/jm_vector_template.h"

#undef JM_TEMPLATE_INSTANCE_TYPE
#define JM_TEMPLATE_INSTANCE_TYPE size_t
#include "JM/jm_vector_template.h"

#undef JM_TEMPLATE_INSTANCE_TYPE
#define JM_TEMPLATE_INSTANCE_TYPE jm_voidp
#include "JM/jm_vector_template.h"

#undef JM_TEMPLATE_INSTANCE_TYPE
/*#undef JM_COMPAR_OP
#define JM_COMPAR_OP(f,s) strcmp(f,s)*/ 
#define JM_TEMPLATE_INSTANCE_TYPE jm_string 
#include "JM/jm_vector_template.h"

#undef JM_TEMPLATE_INSTANCE_TYPE
#define JM_TEMPLATE_INSTANCE_TYPE jm_name_ID_map_t
#include "JM/jm_vector_template.h"

