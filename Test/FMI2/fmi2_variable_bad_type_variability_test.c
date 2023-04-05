#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

/* global used by logger - make sure to reset between different XML parsings */
int g_n_logger_found_err_msg = 0;

static void log_default(jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
    printf("module = %s, log level = %d: %s\n", module, log_level, message);
}

static void logger_verify(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message) {
    const char* msg_exp = "Only Real variables can have variability='continuous'";
    if (!strncmp(msg_exp, message, strlen(msg_exp))) {
        g_n_logger_found_err_msg++;
    }
    log_default(module, log_level, message);
}

int main(int argc, char **argv)
{
    fmi2_import_t *xml;
    jm_callbacks * cb = jm_get_default_callbacks();
    fmi_import_context_t *ctx;

    if (argc != 2) {
        printf("Usage: %s <variable_bad_causality_variability_dir>\n", argv[0]);
        return CTEST_RETURN_FAIL;
    }

    cb->logger = logger_verify;
    ctx = fmi_import_allocate_context(cb);
    if (ctx == NULL) {
        printf("Failed to allocate memory");
        return CTEST_RETURN_FAIL;
    }

    xml = fmi2_import_parse_xml(ctx, argv[1], NULL);
    fmi_import_free_context(ctx);
    if (xml == NULL) {
        return CTEST_RETURN_FAIL;
    }

    if (g_n_logger_found_err_msg != 4) {
        printf("incorrect number of variables found with invalid variability\n");
        return CTEST_RETURN_FAIL;
    }

    fmi2_import_free(xml);
    return CTEST_RETURN_SUCCESS;
}
