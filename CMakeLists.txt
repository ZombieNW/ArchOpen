cmake_minimum_required(VERSION 3.10)
project(ArchOpen LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
  src/main.cpp
  src/cli/commands.cpp
  src/config/configmanager.cpp
  src/config/configmigrator.cpp
  src/core/romlauncher.cpp
  src/cli/logger.cpp
)

if(WIN32)
  set(RES_FILE "${CMAKE_CURRENT_SOURCE_DIR}/assets/app.res")
  set_source_files_properties(${RES_FILE} PROPERTIES EXTERNAL_OBJECT true)
endif()

add_executable(${PROJECT_NAME} ${SOURCES} ${RES_FILE})

target_include_directories(${PROJECT_NAME} PRIVATE
  src
  include
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O2)

if(WIN32)
  target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -static -s)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "../dist"
    OUTPUT_NAME "ArchOpen"
)
