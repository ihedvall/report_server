#set(Iconv_ROOT "k:/expat/master" CACHE PATH "The Expat library is required for the build")
if (NOT Iconv_FOUND)
    find_package(Iconv REQUIRED)
endif()

message(STATUS "ICONV Found: "  ${Iconv_FOUND})
message(STATUS "ICONV Include Dirs: "  ${Iconv_INCLUDE_DIRS})
message(STATUS "ICONV Libraries: " ${Iconv_LIBRARIES})
message(STATUS "ICONV Version: " ${Iconv_VERSION})