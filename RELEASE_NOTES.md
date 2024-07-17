# FMI Library Release Notes

The release notes are typically a highlighting subset of all changes made. For full history, see resolved issues and commits.

Note that version 2.1 is the first version with release notes. Please see the commit history for older versions.

## 3.0a4

### Improvements

- Added include of `GNUInstallDirs`.

### Changes

- The CMAKE variable `CMAKE_INSTALL_PREFIX` is now utilized instead of `FMILIB_INSTALL_PREFIX`.
- Removed the build configuration for CentOS.
- Fixed an issue with parsing very large valueReferences.
- Fixed an incorrect error check that required a start attribute for variables with causality='independent'.

## 3.0a3

### Improvements

- Improved error handling.
- Attribute buffers are now properly cleared if an element fails to parse.
- Disabled jm_locale_test if locale testing is disabled.

### Changes

- Resolved reported vulnerability issues:
    - Updated to expat 2.5.0 (from version 2.4.8).
    - Updated to zlib 1.2.13 (from version 1.2.6).
- The following functions have been renamed:
    - `fmi3_xml_get_default_experiment_step` to `fmi3_xml_get_default_experiment_step_size`
    - `fmi3_import_get_default_experiment_has_step` to `fmi3_import_get_default_experiment_has_step_size`


## 3.0a2

### Improvements

- Minor improvements to build system, documentation and clean-up of obsolete code for FMI 3.0. In general this release contains no significant new user-related improvements compared to 3.0a1.
- Resolved an issue with missing headers in the FMIL installation, see issue #66.

### Changes

- Renamed several API methods for consistency between the different FMI versions. Primarily this concerns functions of names `fmi3_import_get_*` for retrieving variable properties, these are now named `fmi3_import_get_variable_*`, some examples are:
    - `fmi3_import_get_variability` has been changed to `fmi3_import_get_variable_variability`.
    - `fmi3_import_get_causality` has been changed to `fmi3_import_get_variable_causality`.
- In a similar manner as above, variable related functions of names `fmi3_xml_get_*` now are named according to `fmi3_xml_get_variable_*`.

## 3.0a1

### Improvements

- Added initial support for FMI 3.0. Limitations:
    - XML elements `Annotations`, `TerminalsAndIcons`, and `BuildConfiguration` are not yet supported.
    - Many error checks are missing.

### Changes

- `fmi_construct_dll_dir_name` and `fmi_construct_dll_file_name` have been removed.
- `fmi_import_get_dll_path` has been removed. Instead use `fmi<FMI_VERSION>_import_get_dll_path`.

## 2.4.1

### Changes

- Updated to use Expat 2.4.8.
- The required CMake version to build FMI Library is now 3.1.3 or later.

## 2.4

### Changes

- Added new functions `fmi[1/2]_import_get_options` for retrieving an options object.
- Added new function `fmi_import_set_option_loadlibrary_flag` to allow customization of platform-dependent flags passed to `dlopen`/`LoadLibraryEx` when loading the FMU binary.

### Bug fixes

- Fixed memory leak from parsing Enums.
- Fixed memory leaks in tests.

## 2.3

- Updated `fmi_import_get_fmi_version` to also work on unpacked FMUs.
- Bug fix: Fix compilation issue on OSX related to locale.

## 2.2.3

- Bug fix: Fix segfault during parsing of FMI1

## 2.2.2

- Bug fix: Fix build issues introduced in 2.2.1 for non-MSVC/Linux
- Bug fix: Correctly parse doubles when locale is not using decimal point, now also for FMI1

## 2.2.1

- Bug fix: Correctly parse doubles when locale is not using decimal point
- Check `variability != continuous` for non-Real variables

## 2.2

### Bug fixes

- Added missing getters for attributes defined on a variable:
    - Real: quantity, relativeQuantity, unbound
    - Integer: quantity
    - Enumeration: quantity
- `fmi2_import_get_type_quantity` now returns NULL for all types if 'quantity' is not specified (API documentation previously stated that an empty string would be returned for Real, Integer and Enumeration, but NULL was incorrectly returned - this means that only the API documentation is affected by this change)

## 2.1

- Added functions to API for determining if DefaultExperiment values are defined in the XML (typically used to check if a default value will be returned by the getters for DefaultExperiment attributes)
- Clarified that license is 3-Clause BSD
- FMI Library moved to GitHub
