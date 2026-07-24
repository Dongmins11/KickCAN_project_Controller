cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME F411_Project)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

file(GLOB_RECURSE SRC_FILES "Controller/*.c")
# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${SRC_FILES}
)

# # Add include paths
# target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
#     # Add user defined include paths
# )
# 1. MyApp 내부의 모든 .h 헤더 파일의 위치(경로)를 찾아서 변수에 저장
file(GLOB_RECURSE HEADER_FILES "Controller/*.h")

# 2. 찾은 헤더 파일들의 경로에서 파일명을 제외한 '폴더 경로'만 추출하여 리스트 생성
set(INCLUDE_DIRS "")
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(DIR_PATH ${HEADER_FILE} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${DIR_PATH})
endforeach()

# 3. 중복된 폴더 경로 제거 (하나의 폴더에 헤더가 여러 개 있을 수 있으므로)
list(REMOVE_DUPLICATES INCLUDE_DIRS)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${INCLUDE_DIRS}
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
