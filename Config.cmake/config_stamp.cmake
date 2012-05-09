FIND_PACKAGE(Subversion REQUIRED)
Subversion_WC_INFO(${FMILIBRARYHOME} FMILibrary)

message(STATUS "Generating build timestamp in ${FMILIBRARYBUILD}/config_fmilib.c")
configure_file (
	"${FMILIBRARYHOME}/Config.cmake/config_fmilib.c.cmake"
	"${FMILIBRARYBUILD}/config_fmilib.c"
)
