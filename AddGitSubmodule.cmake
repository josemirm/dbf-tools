# Thanks @scivision for this snippet :-)
# File downloaded from https://gist.github.com/scivision/bb1d47a9529e153617414e91ff5390af

cmake_minimum_required(VERSION 3.4)

function(add_git_submodule dir)
# add a Git submodule directory to CMake, assuming the
# Git submodule directory is a CMake project.
#
# Usage: in CMakeLists.txt
#
# include(AddGitSubmodule.cmake)
# add_git_submodule(mysubmod_dir)

find_package(Git REQUIRED)

if(NOT EXISTS ${dir}/autogen.sh)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${dir}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND_ERROR_IS_FATAL ANY)
endif()

# add_subdirectory(${dir})

endfunction()
