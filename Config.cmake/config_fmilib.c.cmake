#include "config_fmilib.h"

const char * fmilib_build_stamp = 
	"Built on: " __DATE__ " " __TIME__ "\n"
	"Last changed on: @FMILibrary_WC_LAST_CHANGED_DATE@\n"
	"Revision: @FMILibrary_WC_REVISION@";

	
const char* fmilib_get_build_stamp() {
	return fmilib_build_stamp;
}