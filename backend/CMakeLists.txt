cmake_minimum_required(VERSION 3.16)
project(OrganizeXBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(HTTPLIB_USE_OPENSSL_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_ZLIB_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_BROTLI_IF_AVAILABLE OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.18.3
)
FetchContent_MakeAvailable(httplib)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

find_package(Threads REQUIRED)

add_executable(organizeX_server
    main.cpp
    Category.cpp
    FileManager.cpp
    Organizer.cpp
)

target_link_libraries(organizeX_server
    PRIVATE
        httplib::httplib
        nlohmann_json::nlohmann_json
        Threads::Threads
)

if(WIN32)
    target_link_libraries(organizeX_server PRIVATE ws2_32)
endif()
