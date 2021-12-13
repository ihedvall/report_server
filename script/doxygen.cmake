if (NOT DOXYGEN_FOUND)
    find_package(Doxygen COMPONENTS dot mscgen )
endif()

message(STATUS "Doxygen Found: " ${DOXYGEN_FOUND})
message(STATUS "Doxygen Version: " ${DOXYGEN_VERSION})
