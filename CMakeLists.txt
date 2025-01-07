project(tip-function-passes)

# minimum required CMAKE version
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

# clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Required packages
find_package(PkgConfig REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LLVM_DIR}")

# compiler must be 17 
set(CMAKE_CXX_STANDARD 17)

# compile with debug flags
set(CMAKE_BUILD_TYPE Debug)

# cleanup the code with clang-tidy
#set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)

######## LLVM rules for installed libraries ###########
# We want to select the latest stable release even if others are installed
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(LLVM 14.0 REQUIRED CONFIG)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  find_package(LLVM 14.0 REQUIRED CONFIG)
else()
  message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} is not supported")
endif()

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

message("LLVM STATUS:
  Definitions ${LLVM_DEFINITIONS}
  Includes    ${LLVM_INCLUDE_DIRS}
  Libraries   ${LLVM_LIBRARY_DIRS}"
)

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

######## Tool specific rules ###########
add_subdirectory(src)
