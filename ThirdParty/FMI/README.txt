The fmi*Functions.h and fmi*FunctionTypes.h are only used when compiling the test-FMUs. In FMIL, we instead use the
<repo>/src/Util/include/FMI** headers. Those files depend on normal fmi3PlatformTypes.h, but the functions are mapped
to the FMIL namespace instead.

This means that if you're updating the C-API, you have to change both the namespace mapper headers and the regular
fmi3*.h files.

