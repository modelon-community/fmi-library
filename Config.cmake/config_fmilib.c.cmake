#include "config_fmilib.h"

const char * fmilib_build_stamp = 
	"Built on: " __DATE__ " " __TIME__ "\n"
	"Last changed on: @FMILibrary_WC_LAST_CHANGED_DATE@\n"
	"Revision: @FMILibrary_WC_REVISION@";

	
const char* fmilib_get_build_stamp(void) {
	FMILIB_TRACE("Inside fmilib_get_build_stamp\n");
	return fmilib_build_stamp;
}