include (FetchContent)
FetchContent_Declare(odslib
        GIT_REPOSITORY https://github.com/ihedvall/odslib.git
        GIT_TAG HEAD)
FetchContent_MakeAvailable(odslib)
message(STATUS "ODSLIB Populated: " ${odslib_POPULATED})
message(STATUS "ODSLIB Source Dir: " ${odslib_SOURCE_DIR})
message(STATUS "ODSLIB Binary Dir: " ${odslib_BINARY_DIR})