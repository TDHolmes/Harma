set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# specify what type of compilation we're doing
SET(COMPILER_TYPE "clang")

if(MINGW OR CYGWIN OR WIN32)
    set(UTIL_SEARCH_CMD where)
elseif(UNIX OR APPLE)
    set(UTIL_SEARCH_CMD which)
endif()

execute_process(
        COMMAND ${UTIL_SEARCH_CMD} arm-none-eabi-gcc
        OUTPUT_VARIABLE BINUTILS_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)

set(triple arm-none-eabi)

set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER_TARGET ${triple})

set(CMAKE_C_FLAGS_INIT "-B${ARM_TOOLCHAIN_DIR}")
set(CMAKE_CXX_FLAGS_INIT "-B${ARM_TOOLCHAIN_DIR}")
# only for successful compilation of CMake test
set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs")
# provide clang with ARM GCC toolchain include directory info
include_directories(${ARM_TOOLCHAIN_DIR}/../arm-none-eabi/include)

set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/arm-none-eabi-objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${ARM_TOOLCHAIN_DIR}/arm-none-eabi-size CACHE INTERNAL "size tool")

set(CMAKE_FIND_ROOT_PATH ${ARM_TOOLCHAIN_DIR})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

macro(_generate_object target suffix type)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O${type}
        "${CMAKE_CURRENT_BINARY_DIR}/${target}${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_BINARY_DIR}/${target}${suffix}"
    )
endmacro()

macro(_firmware_size target)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} -B
        "${CMAKE_CURRENT_BINARY_DIR}/${target}${CMAKE_EXECUTABLE_SUFFIX}"
    )
endmacro()

# there are some source differences between clang and arm-none-eabi-gcc
SET(CMAKE_CLANG_FLAGS "${CMAKE_CLANG_FLAGS} -D__clang__")

# ------------- DEBUG BUILD FLAGS (no optimizations) ------------- #

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_CLANG_FLAGS} ${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "c compiler flags debug")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE INTERNAL "cxx compiler flags debug")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}" CACHE INTERNAL "asm compiler flags debug")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}" CACHE INTERNAL "linker flags debug")


# ------------- RELEASE BUILD FLAGS (use optimizations) ------------- #

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_CLANG_FLAGS} ${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "c compiler flags release")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE INTERNAL "cxx compiler flags release")
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE}" CACHE INTERNAL "asm compiler flags release")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE INTERNAL "linker flags release")

SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR} ${EXTRA_FIND_PATH})
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

IF(CMAKE_BUILD_TYPE MATCHES Release)
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build/release)
    SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build/release)
ELSEIF(CMAKE_BUILD_TYPE MATCHES Debug)
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build/debug)
    SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build/debug)
ENDIF()

MESSAGE(STATUS "BUILD_TYPE: " ${CMAKE_BUILD_TYPE})



# -----------------------------------------------------------------------------#
