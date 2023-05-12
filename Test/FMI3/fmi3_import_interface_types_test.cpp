#include <stdio.h>
#include <stdint.h>

#include "fmilib.h"
#include "config_test.h"
#include "fmi_testutil.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("FMU interface types: Default attributes") {
    const char* xmldir = FMI3_TEST_XML_DIR "/interface_types/valid/default_attrs";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(tfmu != nullptr);
    REQUIRE(fmu  != nullptr);
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);

    SECTION("ME") {
        REQUIRE_STREQ(fmi3_import_get_model_identifier_ME(fmu), "id_me");
        // Common
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_needsExecutionTool)                  == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_canBeInstantiatedOnlyOncePerProcess) == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_canGetAndSetFMUState)                == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_canSerializeFMUState)                == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_providesDirectionalDerivatives)      == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_providesAdjointDerivatives)          == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_providesPerElementDependencies)      == 0);
        // ME
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_needsCompletedIntegratorStep)   == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_providesEvaluateDiscreteStates) == 0);
    }
    SECTION("CS") {
        REQUIRE_STREQ(fmi3_import_get_model_identifier_CS(fmu), "id_cs");
        // Common
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_needsExecutionTool)                  == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_canBeInstantiatedOnlyOncePerProcess) == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_canGetAndSetFMUState)                == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_canSerializeFMUState)                == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_providesDirectionalDerivatives)      == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_providesAdjointDerivatives)          == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_providesPerElementDependencies)      == 0);
        // CS
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_canHandleVariableCommunicationStepSize) == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_maxOutputDerivativeOrder)               == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_providesIntermediateUpdate)             == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_mightReturnEarlyFromDoStep)             == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_canReturnEarlyAfterIntermediateUpdate)  == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_hasEventMode)                           == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_providesEvaluateDiscreteStates)         == 0);

        REQUIRE(fmi3_import_get_cs_has_fixed_internal_step_size(fmu) == 0);
        REQUIRE(fmi3_import_get_cs_fixed_internal_step_size(fmu)     == 0.0);
        REQUIRE(fmi3_import_get_cs_recommended_intermediate_input_smoothness(fmu) == 0.0);
    }
    SECTION("SE") {
        REQUIRE_STREQ(fmi3_import_get_model_identifier_SE(fmu), "id_se");
        // Common
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_needsExecutionTool)                  == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_canBeInstantiatedOnlyOncePerProcess) == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_canGetAndSetFMUState)                == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_canSerializeFMUState)                == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_providesDirectionalDerivatives)      == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_providesAdjointDerivatives)          == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_providesPerElementDependencies)      == 0);
    }
    fmi3_testutil_import_free(tfmu);
}

TEST_CASE("FMU interface types: All attributes") {
    const char* xmldir = FMI3_TEST_XML_DIR "/interface_types/valid/non_default_attrs";

    fmi3_testutil_import_t* tfmu = fmi3_testutil_parse_xml_with_log(xmldir);
    fmi3_import_t* fmu = tfmu->fmu;
    REQUIRE(tfmu != nullptr);
    REQUIRE(fmu  != nullptr);
    REQUIRE(fmi3_testutil_get_num_problems(tfmu) == 0);

    SECTION("ME") {
        REQUIRE_STREQ(fmi3_import_get_model_identifier_ME(fmu), "id_me");
        // Common
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_needsExecutionTool)                  == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_canBeInstantiatedOnlyOncePerProcess) == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_canGetAndSetFMUState)                == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_canSerializeFMUState)                == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_providesDirectionalDerivatives)      == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_providesAdjointDerivatives)          == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_providesPerElementDependencies)      == 1);
        // ME
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_needsCompletedIntegratorStep)   == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_me_providesEvaluateDiscreteStates) == 1);
    }
    SECTION("CS") {
        REQUIRE_STREQ(fmi3_import_get_model_identifier_CS(fmu), "id_cs");
        // Common
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_needsExecutionTool)                  == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_canBeInstantiatedOnlyOncePerProcess) == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_canGetAndSetFMUState)                == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_canSerializeFMUState)                == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_providesDirectionalDerivatives)      == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_providesAdjointDerivatives)          == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_providesPerElementDependencies)      == 1);
        // CS
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_canHandleVariableCommunicationStepSize) == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_providesIntermediateUpdate)             == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_mightReturnEarlyFromDoStep)             == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_canReturnEarlyAfterIntermediateUpdate)  == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_hasEventMode)                           == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_providesEvaluateDiscreteStates)         == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_cs_maxOutputDerivativeOrder)               == 5);

        REQUIRE(fmi3_import_get_cs_has_fixed_internal_step_size(fmu) == 1);
        REQUIRE(fmi3_import_get_cs_fixed_internal_step_size(fmu)     == 3.0);
        REQUIRE(fmi3_import_get_cs_recommended_intermediate_input_smoothness(fmu) == -2);
        
    }
    SECTION("SE") {
        REQUIRE_STREQ(fmi3_import_get_model_identifier_SE(fmu), "id_se");
        // Common
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_needsExecutionTool)                  == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_canBeInstantiatedOnlyOncePerProcess) == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_canGetAndSetFMUState)                == 1);
        /* Intentionally have one value off to verify they're not connected */
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_canSerializeFMUState)                == 0);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_providesDirectionalDerivatives)      == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_providesAdjointDerivatives)          == 1);
        REQUIRE(fmi3_import_get_capability(fmu, fmi3_se_providesPerElementDependencies)      == 1);
    }
    fmi3_testutil_import_free(tfmu);
}