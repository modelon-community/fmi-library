#include <stdio.h>

#include <fmilib.h>
#include "config_test.h"
#include "fmi_testutil.h"

/**
    Try to parse, and verify that we get a fatal error.
*/
static int test_xml_parse_fatal(const char* xmlPath) {
    fmi3_import_t* xml;
    jm_callbacks* cb = jm_get_default_callbacks();
    fmi_import_context_t* ctx;

    /* update these if you change xml: */
    int nBadTds = 20;
    int nBadVars = 20;

    ctx = fmi_import_allocate_context(cb);
    if (ctx == NULL) {
        return TEST_FAIL;
    }

    /* parse the XML (should result in fatal error) */
    xml = fmi3_import_parse_xml(ctx, xmlPath, NULL);
    fmi_import_free_context(ctx);
    if (xml != NULL) {
        fmi3_import_free(xml);
        return TEST_FAIL;
    }

    return TEST_OK;
}

int main(int argc, char **argv)
{
    char xmlPath[1000];
    size_t sizeXmlPath = sizeof(xmlPath) / sizeof(char);

    char dirPath[1000];
    size_t sizeDirPath = sizeof(dirPath) / sizeof(char);
    const char* dirs[] = { "boolean", "enum", "float64", "int64", "string" };
    size_t nDirs = sizeof(dirs) / sizeof(char*);

    int ret = 1;
    size_t i;

    if (argc != 2) {
        printf("Usage: %s <Test/FMI3/parser_test_xmls>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    /* build dirPath */
    strncpy(dirPath, argv[1], sizeDirPath);
    strncat(dirPath, "/fatal/no_vr/", sizeDirPath);

    /* run tests */
    for (i = 0; i < nDirs; i++) {
        fmi_testutil_build_xml_path(xmlPath, sizeXmlPath, dirPath, dirs[i]);
        ret &= test_xml_parse_fatal(xmlPath);
    }
    return ret == 0 ? CTEST_RETURN_FAIL : CTEST_RETURN_SUCCESS;
}

