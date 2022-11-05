# dbf-tools: CMakeLists.txt
# Copyright (c) 2022 José Miguel (Josemi) Rodríguez M.
# https://github.com/josemirm/dbf-tools

cmake_minimum_required(VERSION 3.4)

project(
    dbf-tools
    VERSION     0.1.0
    DESCRIPTION "dbf-tools"
    LANGUAGES   C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_BUILD_TYPE Debug)

# Configure libdbf library. This is only valid for Unix-like systems
execute_process(COMMAND /bin/bash "./autogen.sh"
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/libdbf
    COMMAND_ERROR_IS_FATAL ANY)

execute_process(COMMAND /bin/bash "./configure"
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/libdbf
    COMMAND_ERROR_IS_FATAL ANY)

# Set the source files
set(DBF-INFO-SRC src/dbf-info.c src/utils.c libdbf/src/dbf.c
    libdbf/src/dbf_endian.c)

set(DBF-CSV-SRC src/dbf-csv.c src/utils.c libdbf/src/dbf.c
    libdbf/src/dbf_endian.c)

# Set include folders
set(INC src libdbf/include/libdbf libdbf/src libdbf)


# Show more warnings and analyze the code looking for common mistakes
# (Uncomment it only for debugging)
# if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /analyze")
# elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra -fanalyzer")
# elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
# endif()

# Configure the output executable files
add_executable(dbf-info ${DBF-INFO-SRC})
target_include_directories(dbf-info PUBLIC ${INC})
target_link_libraries(dbf-info)

add_executable(dbf-csv ${DBF-CSV-SRC})
target_include_directories(dbf-csv PUBLIC ${INC})
target_link_libraries(dbf-csv)
