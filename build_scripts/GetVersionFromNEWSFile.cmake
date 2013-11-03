# This macro extract the last version from the new file.
macro(get_version_from_file version_file output_variable)
    file(STRINGS ${version_file} raw_version REGEX "^# [0-9]+\\.[0-9]+\\.[0-9]+ *$")
    list(GET raw_version 0 raw_version)
    string(REGEX REPLACE "^# ([0-9]+\\.[0-9]+\\.[0-9]+)" "\\1"
        ${output_variable}
        ${raw_version})
endmacro()

# This macro generate cpack version variables from
# the version extracted from NEWS file.
macro(generate_cpack_version_variables)
    get_version_from_file("${CMAKE_CURRENT_SOURCE_DIR}/NEWS" CPACK_PACKAGE_VERSION)

    string(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+" "\\1" 
        CPACK_PACKAGE_VERSION_MAJOR 
        ${CPACK_PACKAGE_VERSION})
    string(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+" "\\1" 
        CPACK_PACKAGE_VERSION_MINOR
        ${CPACK_PACKAGE_VERSION})
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1" 
        CPACK_PACKAGE_VERSION_PATCH
        ${CPACK_PACKAGE_VERSION})
endmacro()

