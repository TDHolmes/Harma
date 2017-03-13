INCLUDE(CMakeForceCompiler)
# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

SET(CLICOLOR_FORCE ON)

# THE VERSION NUMBER
SET (PENSEL_VERSION_MAJOR 0)
SET (PENSEL_VERSION_MINOR 1)

# ENABLE ASM
ENABLE_LANGUAGE(ASM)
SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)


# CURRENT DIRECTORY up a level
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR}/../)


# ARMGCC DIRECTORY, if null string will use environmental variables
SET(TOOLCHAIN_DIR "" CACHE STRING "Directory to use for ARM GCC")

# ------------------------------ FLAGS USED BY EVERYONE ------------------------------ #
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -flto")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mcpu=cortex-m4")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -Wall -Wdouble-promotion -Wextra")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mfloat-abi=hard")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mthumb")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -fno-common")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -ffunction-sections")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -fdata-sections")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -ffreestanding")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -fno-builtin")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -std=gnu99")

# ------------------------------ ASSEMBLY LANGUAGE FLAGS - DEBUG ------------------------------ #
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -D__STARTUP_CLEAR_BSS")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")


# ------------------------------ ASSEMBLY LANGUAGE FLAGS - DEBUG ------------------------------ #
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -D__STARTUP_CLEAR_BSS")
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -flto")
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")


# ------------------------------ C LANGUAGE FLAGS - DEBUG ------------------------------ #
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DCPU_MKL03Z32VFK4")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MMD")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MP")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")


# ------------------------------ C LANGUAGE FLAGS - RELEASE ------------------------------ #
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DCPU_MKL03Z32VFK4")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MMD")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MP")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")


# ------------------------------ LINKER FLAGS - RELEASE ------------------------------ #
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} --specs=nano.specs")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker --gc-sections")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker -static")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker -z")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker muldefs")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker -Map=$(ProjDirPath)/build/release/motor_driver.map")


# ------------------------------ LINKER FLAGS - DEBUG ------------------------------ #
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --specs=nano.specs")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker --gc-sections")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker -static")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker -z")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker muldefs")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker -Map=$(ProjDirPath)/build/debug/motor_driver.map")


# ------------------------------ INCLUDE DIRECTORIES ------------------------------ #

include_directories(${ProjDirPath})

# ------------------------------ SOURCE FILES ------------------------------ #

add_executable(pensel.elf
	# user code
	"${ProjDirPath}accel_MMA8452Q.c"
	"${ProjDirPath}accel_MMA8452Q.h"
	"${ProjDirPath}common.h"
	"${ProjDirPath}I2C.h"
	# Drivers from vender
)


# ------------------------------ LINKER FILES ------------------------------ #
# set(LINKER_LOCATION "${ProjDirPath}MKL03Z4_drivers/devices/MKL03Z4/gcc/")
# set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${LINKER_LOCATION}MKL03Z32xxx4_flash.ld -static")
# set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${LINKER_LOCATION}MKL03Z32xxx4_flash.ld -static")



# --------------------- RESOLVING CIRCULAR DEPENDENCIES -------------------- #
# http://stackoverflow.com/questions/5651869/gcc-what-are-the-start-group-and-end-group-command-line-options

TARGET_LINK_LIBRARIES(motor_driver.elf -Wl,--start-group)

target_link_libraries(motor_driver.elf debug m)
target_link_libraries(motor_driver.elf debug c)
target_link_libraries(motor_driver.elf debug gcc)
target_link_libraries(motor_driver.elf debug nosys)
target_link_libraries(motor_driver.elf optimized m)
target_link_libraries(motor_driver.elf optimized c)
target_link_libraries(motor_driver.elf optimized gcc)
target_link_libraries(motor_driver.elf optimized nosys)

TARGET_LINK_LIBRARIES(motor_driver.elf -Wl,--end-group)
