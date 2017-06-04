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
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR}/..)


# ARMGCC DIRECTORY, if null string will use environmental variables
SET(TOOLCHAIN_DIR "" CACHE STRING "Directory to use for ARM GCC")

# -------------------------- FLAGS USED BY EVERYONE -------------------------- #

# clang doesn't support link time optimizations...
# TODO: Fix this?
if (COMPILER_TYPE MATCHES GCC)
	SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -flto")
ELSE()
	SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -fshort-enums")
	# SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -flto=thin") #  -fuse-ld=gold
ENDIF()

SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mcpu=cortex-m4")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -Wall -Wdouble-promotion -Wextra")
# SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mfloat-abi=soft")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mfloat-abi=hard")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mfpu=fpv4-sp-d16")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mthumb")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -fno-common")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -Wno-unused-parameter")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -ffunction-sections")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -fdata-sections")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -ffreestanding")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -fno-builtin")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -std=gnu99")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -DUSE_HAL_DRIVER")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -DUSE_FULL_ASSERT")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -DWATCHDOG_ENABLE")  # Flag to enable/disable use of the watchdog
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -DSTM32F302x8")

# ---------------------- ASSEMBLY LANGUAGE FLAGS - DEBUG --------------------- #
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -D__STARTUP_CLEAR_BSS")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")


# --------------------- ASSEMBLY LANGUAGE FLAGS - DEBUG ---------------------- #
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} -D__STARTUP_CLEAR_BSS")
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")


# ------------------------- C LANGUAGE FLAGS - DEBUG ------------------------- #
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MMD")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MP")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")

# ------------------------ C LANGUAGE FLAGS - RELEASE ------------------------ #
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MMD")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MP")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")

SET(CMAKE_MAP_PATH "${ProjDirPath}/compiler/build/${CMAKE_BUILD_TYPE}/pensel.map")
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "--specs=nano.specs")
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker --gc-sections")
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker -static")
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker -z")
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker muldefs")
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker -defsym=__stack_size__=0xcc0")
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker -defsym=__heap_size__=0x800")

# --------------------------- LINKER FLAGS - RELEASE ------------------------- #
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${CMAKE_EXE_LINKER_FLAGS_GENERAL}")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker -Map=${CMAKE_MAP_PATH}")

# ---------------------------- LINKER FLAGS - DEBUG -------------------------- #
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_EXE_LINKER_FLAGS_GENERAL}")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g")
# SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -u _printf_float")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker -Map=${CMAKE_MAP_PATH}")


# ---------------------------- INCLUDE DIRECTORIES --------------------------- #

include_directories(${ProjDirPath})
include_directories(${ProjDirPath}/include/)
include_directories(${ProjDirPath}/DriverConf/inc/)
include_directories(${ProjDirPath}/CMSIS/Include/)
include_directories(${ProjDirPath}/CMSIS/Device/ST/STM32F3xx/Include/)

# ------------------------------- SOURCE FILES ------------------------------- #

add_executable(pensel.elf
	# user code
	"${ProjDirPath}/source/LSM303DLHC.c"
	"${ProjDirPath}/source/main.c"
	"${ProjDirPath}/source/reports.c"
	"${ProjDirPath}/source/queue.c"
	"${ProjDirPath}/source/UART.c"
	"${ProjDirPath}/source/I2C.c"
	"${ProjDirPath}/source/ADC.c"
	"${ProjDirPath}/source/hardware.c"
	"${ProjDirPath}/source/startup_stm32f302x8.s"

	# --- Driver configuration files ----
	"${ProjDirPath}/DriverConf/src/stm32f3xx_hal_msp.c"
	"${ProjDirPath}/DriverConf/src/system_stm32f3xx.c"
	"${ProjDirPath}/DriverConf/src/stm32f3xx_it.c"

	# --- CMSIS Stuff ----

	# --- Drivers from vender -----
	# some basic definitions and startup code
	"${ProjDirPath}/Drivers/stm32f3xx_hal.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_cortex.c"

	# peripheral drivers
	"${ProjDirPath}/Drivers/stm32f3xx_hal_gpio.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_hrtim.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_i2c.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_i2c_ex.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_uart.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_uart_ex.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_dma.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_rcc.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_rcc_ex.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_adc.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_adc_ex.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_iwdg.c"  # TODO: add watchdog support
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_flash.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_wwdg.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_usart.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_can.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_cec.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_comp.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_crc.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_crc_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_dac.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_dac_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_flash_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_i2s.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_i2s_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_irda.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_msp_template.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_nand.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_nor.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_opamp.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_opamp_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pccard.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pcd.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pcd_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pwr.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pwr_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_rtc.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_rtc_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_sdadc.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_smartcard.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_smartcard_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_smbus.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_spi.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_spi_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_sram.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_tim.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_tim_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_timebase_rtc_alarm_template.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_timebase_rtc_wakeup_template.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_timebase_tim_template.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_tsc.c"

)


# ------------------------------- LINKER FILES ------------------------------- #
set(LINKER_LOCATION "${ProjDirPath}/compiler")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${LINKER_LOCATION}/pensel.ld -static")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${LINKER_LOCATION}/pensel.ld -static")


# ---------------------- RESOLVING CIRCULAR DEPENDENCIES --------------------- #
# http://stackoverflow.com/questions/5651869/gcc-what-are-the-start-group-and-end-group-command-line-options

TARGET_LINK_LIBRARIES(pensel.elf -Wl,--start-group)

target_link_libraries(pensel.elf debug m)
target_link_libraries(pensel.elf debug c)
target_link_libraries(pensel.elf debug gcc)
target_link_libraries(pensel.elf debug nosys)
target_link_libraries(pensel.elf optimized m)
target_link_libraries(pensel.elf optimized c)
target_link_libraries(pensel.elf optimized gcc)
target_link_libraries(pensel.elf optimized nosys)

TARGET_LINK_LIBRARIES(pensel.elf -Wl,--end-group)
