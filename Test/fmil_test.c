#include <string.h>

#include "fmil_test.h"

void fmi_testutil_build_xml_path(char* buf, size_t bufSize, const char* basePath, const char* appendPath) {
    strncpy(buf, basePath,   bufSize);
    strncat(buf, appendPath, bufSize);
}

