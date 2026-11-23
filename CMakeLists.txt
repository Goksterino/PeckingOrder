cmake_minimum_required(VERSION 3.28)

# NOTE: update EXECUTABLE_NAME in .github/workflows/cmake.yml when changing executable name in this file
set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}_main")
set(MAIN_LIBRARY_NAME "${MAIN_PROJECT_NAME}")
set(TESTS_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}_test")

option(CMAKE_EXPORT_COMPILE_COMMANDS "Export compilation database" ON)

project(${MAIN_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

include(CTest)
include(cmake/RegisterTests.cmake)

###############################################################################
# External dependencies with FetchContent
###############################################################################

include(FetchContent)

set(FETCHCONTENT_QUIET OFF CACHE INTERNAL "Override verbose output" FORCE)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE INTERNAL "Override update strategy" FORCE)

# (Stocare) nlohmann/json - citirea datelor din data/*.json
FetchContent_Declare(
    nlohmann_json
    SYSTEM
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    1
    GIT_PROGRESS   ON
)

# (Biblioteca externă #1) spdlog - log-uri pentru evenimentele din arenă
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    spdlog
    SYSTEM
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.1
    GIT_SHALLOW    1
    GIT_PROGRESS   ON
)

# (Biblioteca externă #2) tabulate - afișarea tabelelor în consolă
FetchContent_Declare(
    tabulate
    SYSTEM
    GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
    GIT_TAG        v1.5
    GIT_SHALLOW    1
    GIT_PROGRESS   ON
)

FetchContent_MakeAvailable(nlohmann_json spdlog tabulate)

###############################################################################
# Configure sources
###############################################################################

add_library(${MAIN_LIBRARY_NAME} STATIC)

add_executable(${MAIN_EXECUTABLE_NAME}
    app/main.cpp
)

target_sources(${MAIN_LIBRARY_NAME}
    PRIVATE
        src/Stats.cpp
        src/Upgrade.cpp
        src/Character.cpp
        src/Enemy.cpp
        src/Arena.cpp
        src/DataLoader.cpp
)

set_compiler_flags(RUN_SANITIZERS ${USE_ASAN} TARGET_NAMES ${MAIN_EXECUTABLE_NAME} ${MAIN_LIBRARY_NAME})

###############################################################################
# Configure includes
###############################################################################

target_include_directories(${MAIN_LIBRARY_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_include_directories(${MAIN_LIBRARY_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_include_directories(${MAIN_LIBRARY_NAME} SYSTEM PUBLIC
    ${EXT_PUBLIC_DIR}
)
target_include_directories(${MAIN_LIBRARY_NAME} SYSTEM PRIVATE
    ${EXT_PRIVATE_DIR}
)

###############################################################################
# Link libraries
###############################################################################

target_link_libraries(${MAIN_LIBRARY_NAME}
    PRIVATE
        nlohmann_json::nlohmann_json
        spdlog::spdlog_header_only
)

target_link_libraries(${MAIN_EXECUTABLE_NAME}
    PRIVATE
        ${MAIN_LIBRARY_NAME}
        tabulate::tabulate
)

###############################################################################
# Install & Copy helper
###############################################################################

install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
install(TARGETS ${MAIN_LIBRARY_NAME} ARCHIVE DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
)
if(APPLE)
    install(FILES ${CMAKE_SOURCE_DIR}/launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(FILES tastatura.txt DIRECTORY data COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})

###############################################################################
# Tests
###############################################################################

if(BUILD_TESTING)
    add_subdirectory(tests)
    add_custom_target(copy_test_data ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data
            ${CMAKE_CURRENT_BINARY_DIR}/data
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data
            $<TARGET_FILE_DIR:${TESTS_EXECUTABLE_NAME}>/data
    )
    add_dependencies(${TESTS_EXECUTABLE_NAME} copy_test_data)
endif()