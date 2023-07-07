//   Copyright (C) 2023 Modelon AB
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the BSD style license.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   FMILIB_License.txt file for more details.
//
//   You should have received a copy of the FMILIB_License.txt file
//   along with this program. If not, contact Modelon AB <http://www.modelon.com>.

#include <stdio.h>

#include "fmilib.h"

int main() {
    jm_callbacks* cb = jm_get_default_callbacks();
    fmi_import_context_t* ctx = fmi_import_allocate_context(cb);
    if (!ctx) {
        printf("main: Context was NULL\n");
        return 1;
    }
    fmi_import_free_context(ctx);
    return 0;
}