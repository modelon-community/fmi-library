FIND_PACKAGE(Subversion REQUIRED)
Subversion_WC_INFO(${FMILIBRARYHOME} FMILibrary)

configure_file (
	"${FMILIBRARYHOME}/Config.cmake/config_fmilib.c.cmake"
	"${FMILIBRARYBUILD}/config_fmilib.c"
)
