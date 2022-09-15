# FMI Library Release Notes

The release notes are typically a highlighting subset of all changes made. For full history, see resolved issues and commits.

Note that version 2.1 is the first version with release notes. Please see the commit history for older versions.


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

