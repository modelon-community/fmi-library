#ifndef FMILIB_TEST_H
#define FMILIB_TEST_H

#include <stdio.h>

#define TEST_FAILED(msg)                                                    \
    do {                                                                    \
        printf("  %s: %s\n", __FUNCTION__, msg);                            \
        return 0;                                                           \
    } while (0)

#define ASSERT_MSG(cond, msg)                                               \
    do {                                                                    \
        if (!(cond)) {                                                      \
            TEST_FAILED(msg);                                               \
        }                                                                   \
    } while (0)

#define TEST_OK (1)

#endif /* FMILIB_TEST_H */
