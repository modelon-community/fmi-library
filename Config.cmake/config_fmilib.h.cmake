#ifndef WIN32
#cmakedefine WIN32
#endif

#cmakedefine MINGW

#ifndef CMAKE_INTDIR
#define CMAKE_INTDIR "."
#endif

/* Platform folder sepperator */
#define FMI_FILE_SEP "@FMI_FILE_SEP@"	

/* DLL file name suffix(.so, .dll, ...) */
#define FMI_DLL_EXT "@CMAKE_SHARED_MODULE_SUFFIX@" 

/* Folder name in which the DLL is found for this platform */
#ifdef _WIN64
#define FMI_PLATFORM "win64"
#elif defined _WIN32
#define FMI_PLATFORM "win32"
#elif (defined __LP64__ || defined __LLP64__)
#define FMI_PLATFORM "linux64"
#else
#define FMI_PLATFORM "linux32"
#endif

#define FMI_BINARIES "binaries"
#define FMI_MODEL_DESCRIPTION_XML "modelDescription.xml"


