# FMI Library Release Notes

The release notes are typically a highlighting subset of all changes made. For full history, see resolved issues and commits.

Note that version 2.1 is the first version with release notes. Please see the commit history for older versions.

## 2.2

# Bug fixes

- Added missing getters for attributes defined on a variable:
    - Real: quantity, relativeQuantity, unbound
    - Integer: quantity
    - Enumeration: quantity
- `fmi2_import_get_type_quantity` now returns NULL for all types if 'quantity' is not specified (API documentation previously stated that an empty string would be returned for Real, Integer and Enumeration, but NULL was incorrectly returned - this means that only the API documentation is affected by this change)

## 2.1

- Added functions to API for determining if DefaultExperiment values are defined in the XML (typically used to check if a default value will be returned by the getters for DefaultExperiment attributes)
- Clarified that license is 3-Clause BSD 
- FMI Library moved to GitHub

