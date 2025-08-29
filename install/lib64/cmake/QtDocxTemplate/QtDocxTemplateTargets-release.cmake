#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "QtDocxTemplate::QtDocxTemplate" for configuration "Release"
set_property(TARGET QtDocxTemplate::QtDocxTemplate APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(QtDocxTemplate::QtDocxTemplate PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib64/libQtDocxTemplate.a"
  )

list(APPEND _cmake_import_check_targets QtDocxTemplate::QtDocxTemplate )
list(APPEND _cmake_import_check_files_for_QtDocxTemplate::QtDocxTemplate "${_IMPORT_PREFIX}/lib64/libQtDocxTemplate.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
