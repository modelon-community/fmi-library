/*
    Copyright (C) 2012 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <jm_types.h>
#include <jm_callbacks.h>
#include <fmi_zip_unzip.h>
#include "config.h"


void do_exit(int code)
{
	printf("Press any key to exit\n");
	/* getchar(); */
	exit(code);
}

/* Logger function */
void importlogger(jm_callbacks* c, jm_string module, jm_log_level_enu_t log_level, jm_string message)
{
        printf("module = %s, log level = %d: %s\n", module, log_level, message);
}

/**
 * \brief Unzip test. Tests the fmi_zip_unzip function by uncompressing some file.
 *
 */
int main(int argc, char *argv[])
{
	jm_callbacks callbacks;
	jm_status_enu_t status;	

	callbacks.malloc = malloc;
    callbacks.calloc = calloc;
    callbacks.realloc = realloc;
    callbacks.free = free;
    callbacks.logger = importlogger;
    callbacks.context = 0;

	status = fmi_zip_unzip(UNCOMPRESSED_DUMMY_FILE_PATH_SRC, UNCOMPRESSED_DUMMY_FOLDER_PATH_DIST, &callbacks);

	if (status == jm_status_error) {
		printf("Failed to uncompress the file\n");
		do_exit(CTEST_RETURN_FAIL);
	} else {
		printf("Succesfully uncompressed the file\n");
		do_exit(CTEST_RETURN_SUCCESS);
	}
}


