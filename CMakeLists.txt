cmake_minimum_required(VERSION 3.15)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX "-debug")
set(CMAKE_RELWITHDEBINFO_POSTFIX "-reldbg")
set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG "_DEBUG")

if (WIN32)
    set(VCPKG_TARGET_TRIPLET x64-windows-static-md)
endif()

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

set(APP_NAME ${PROJECT_NAME})
set(REZONALITY_ROOT ${CMAKE_CURRENT_LIST_DIR})

project(Morny VERSION 0.1.0.0)

find_package(clipp CONFIG REQUIRED) # Commandline parser for command line arguments
find_package(fmt CONFIG REQUIRED) # String formatting library
find_path(TSL_ORDERED_MAP_INCLUDE_DIRS "tsl/ordered_hash.h")

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG -DDEBUG" )
endif()

set(MORNY_SOURCES
    src/lexer.cpp
    src/parser.cpp
    src/main.cpp

    include/morny/lexer.h
    include/morny/parser.h
)

add_executable(${PROJECT_NAME} ${OS_BUNDLE} ${MORNY_SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_BINARY_DIR}
    include
    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    PRIVATE
        clipp::clipp
        fmt::fmt-header-only
    )

