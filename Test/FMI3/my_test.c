#include <stdlib.h>
#include <fmilib.h>
#include <stdio.h>
#include "config_test.h"

static const int SHOULD_NOT_LOG_EXPECTED_MSG = 0;
static const int SHOULD_LOG_EXPECTED_MSG = 1;

static int did_not_log_expected_msg;
static char *expected_message = "Invalid structured ScalarVariable name";
static char *name_check_test_directory;

char *concat(char *s1, char *s2)
{
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

void importlogger(jm_callbacks* c, jm_string module,
        jm_log_level_enu_t log_level, jm_string message)
{
    printf("module = %s, log level = %d: %s\n", module, log_level, message);
    if (!strncmp(expected_message, message, strlen(expected_message))) {
        did_not_log_expected_msg = 0;
    }
}

void test_parser(char *xml_dir, int should_not_log_expected_msg, int configuration)
{
    jm_callbacks *callbacks;
    fmi_import_context_t *context;
    fmi3_import_t *fmu;
    char *full_path;

    callbacks = (jm_callbacks *) malloc(sizeof(jm_callbacks));
    callbacks->malloc = malloc;
    callbacks->calloc = calloc;
    callbacks->realloc = realloc;
    callbacks->free = free;
    callbacks->logger = importlogger;
    callbacks->log_level = jm_log_level_all;
    callbacks->context = 0;
    context = fmi_import_allocate_context(callbacks);
    if (configuration != 0) {
        fmi_import_set_configuration(context, configuration);
    }

    did_not_log_expected_msg = 1;
    full_path = concat(name_check_test_directory, xml_dir);
    fmu = fmi3_import_parse_xml(context, full_path, NULL);

    free(full_path);
    fmi_import_free_context(context);
    if (fmu == NULL) {
        exit(CTEST_RETURN_FAIL);
    }
    if (!should_not_log_expected_msg && did_not_log_expected_msg ||
            did_not_log_expected_msg && !should_not_log_expected_msg) {
        exit(CTEST_RETURN_FAIL);
    }
}

void test_fmi_version(char* path_xml) {
	fmi3_xml_model_description_t* md = fmi3_xml_parse(path_xml);
	fmi3_fmu_kind_enu_t fmu_kind = fmi3_xml_get_fmu_kind(md);
    assert_equals("3.0", fmu_kind);
}

void test_basic(void)
{
    test_fmi_version();
}

int main(int argc, char *argv[])
{
    if (argc == 2) {
        name_check_test_directory = argv[1];
    } else {
        printf("Usage: %s <path to folder naming_conventions_xmls>\n", argv[0]);
        exit(CTEST_RETURN_FAIL);
    }

    test_basic();

    return 0;
}
