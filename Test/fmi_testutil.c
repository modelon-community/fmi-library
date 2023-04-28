#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "config_test.h"
#include "fmi_testutil.h"

void fmi_testutil_build_xml_path(char* buf, size_t bufSize, const char* basePath, const char* appendPath) {
    strncpy(buf, basePath,   bufSize);
    strncat(buf, appendPath, bufSize);
}

/**
 * This function is called before a test fails via macro, so you can put a 
 * breakpoint on it to make debugging easier
 */
void fmi_testutil_enter_breakpoint()
{
    /* You can put a breakpoint on this line to break on test failure. */
}

fmi3_import_t* fmi3_testutil_parse_xml(const char* xmldir) {
    jm_callbacks* cb = jm_get_default_callbacks();
    fmi_import_context_t* ctx = fmi_import_allocate_context(cb);
    ASSERT_MSG(ctx != NULL, "Context was NULL");

    fmi3_import_t* xml;
    xml = fmi3_import_parse_xml(ctx, xmldir, NULL);
    ASSERT_MSG(xml != NULL, "Failed to parse XML");

    fmi_import_free_context(ctx);

    return xml;
}

void fail(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("Test failure: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);

    exit(CTEST_RETURN_FAIL);
}

char* concat(char *s1, char *s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    /* +1 for the zero-terminator */
    char *result = (char *) malloc((len1 + len2 + 1) * sizeof(char));
    if (result == NULL) {
        exit(CTEST_RETURN_FAIL);
    }
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); /* +1 to copy the null-terminator */
    return result;
}