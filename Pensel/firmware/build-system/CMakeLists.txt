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

# clang doesn't support link time optimizations with existing ARM GCC tools...
if (COMPILER_TYPE MATCHES GCC)
	SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -flto")  # link time optimization
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
# SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -DWATCHDOG_ENABLE")  # Flag to enable/disable use of the watchdog
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -DSTM32F302x8")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -DGIT_TAG_SHORT=${GIT_TAG_SHORT}")

# ---------------------- ASSEMBLY LANGUAGE FLAGS - DEBUG --------------------- #
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g")
SET(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")


# --------------------- ASSEMBLY LANGUAGE FLAGS - DEBUG ---------------------- #
SET(CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")


# ------------------------- C LANGUAGE FLAGS - DEBUG ------------------------- #
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DWATCHDOG_CAPTURE")  # we will while 1 loop on reboot from watchdog
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

SET(CMAKE_MAP_PATH "${ProjDirPath}/build-system/build/${CMAKE_BUILD_TYPE}/pensel.map")
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
include_directories(${ProjDirPath}/peripherals/CMSIS/Include/)
include_directories(${ProjDirPath}/peripherals/stm32f3)
include_directories(${ProjDirPath}/peripherals/CMSIS/Device/ST/STM32F3xx/Include/)

# ------------------------------- SOURCE FILES ------------------------------- #

list(APPEND STM32_HAL_sources

	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_rcc.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_spi_ex.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_rcc_ex.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_i2c.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_i2c_ex.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_uart.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_uart_ex.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_usart.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_spi.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_gpio.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_adc_ex.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_iwdg.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_dma.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_cortex.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_flash.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_flash_ex.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_pcd.c"
	"${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_pcd_ex.c"

		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_msp_template.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_can.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_tsc.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_crc.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_i2s.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_wwdg.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_adc.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_opamp_ex.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_dac.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_nand.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_pccard.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_rtc_ex.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_cec.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_smbus.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_pwr.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_i2s_ex.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_pwr_ex.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_hrtim.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_crc_ex.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_dac_ex.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_comp.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_sram.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_tim_ex.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_opamp.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_sdadc.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_rtc.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_tim.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_nor.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_irda.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_smartcard.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_smartcard_ex.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_timebase_tim_template.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_timebase_rtc_alarm_template.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_hal_timebase_rtc_wakeup_template.c"

		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_tim.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_gpio.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_spi.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_rtc.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_hrtim.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_usart.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_utils.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_comp.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_fmc.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_i2c.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_opamp.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_rcc.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_pwr.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_dac.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_dma.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_adc.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_exti.c"
		# "${ProjDirPath}/peripherals/stm32f3/stm32f3xx_ll_crc.c"
)

list(APPEND common_peripherals

	"${ProjDirPath}/peripherals/hardware/hardware.c"
	"${ProjDirPath}/peripherals/I2C/I2C.c"
	"${ProjDirPath}/peripherals/UART/UART.c"
	"${ProjDirPath}/peripherals/ADC/ADC.c"
	"${ProjDirPath}/peripherals/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f302x8.s"
	"${ProjDirPath}/peripherals/stm32f3-configuration/stm32f3xx_it.c"
	"${ProjDirPath}/peripherals/stm32f3-configuration/stm32f3xx_hal_msp.c"
	"${ProjDirPath}/peripherals/stm32f3-configuration/system_stm32f3xx.c"
)

list(APPEND stm32_usb_lib

	# USB library
	"${ProjDirPath}/peripherals/stm32-usb/usb_sil.c"
	"${ProjDirPath}/peripherals/stm32-usb/usb_init.c"
	"${ProjDirPath}/peripherals/stm32-usb/usb_int.c"
	"${ProjDirPath}/peripherals/stm32-usb/usb_regs.c"
	"${ProjDirPath}/peripherals/stm32-usb/usb_mem.c"
	"${ProjDirPath}/peripherals/stm32-usb/usb_core.c"
)

list(APPEND pensel_usb_files
	"${ProjDirPath}/peripherals/USB/hw_config.c"
	"${ProjDirPath}/peripherals/USB/usb_desc.c"
	"${ProjDirPath}/peripherals/USB/usb_endp.c"
	"${ProjDirPath}/peripherals/USB/usb_istr.c"
	"${ProjDirPath}/peripherals/USB/usb_prop.c"
	"${ProjDirPath}/peripherals/USB/usb_pwr.c"

	"${ProjDirPath}/modules/CDC/cdc.c"
)


add_executable(pensel_v2.elf

	"${ProjDirPath}/applications/pensel_v2.c"

	# "${ProjDirPath}/modules/orientation/movement.c"
	# "${ProjDirPath}/modules/orientation/quanternions.c"
	# "${ProjDirPath}/modules/orientation/orientation.c"
	# "${ProjDirPath}/modules/orientation/matrixmath.c"
	# "${ProjDirPath}/modules/calibration/cal.c"
	# "${ProjDirPath}/modules/utilities/FIR.c"
	"${ProjDirPath}/modules/LSM9DS1/LSM9DS1.c"
	"${ProjDirPath}/modules/utilities/queue.c"
	"${ProjDirPath}/modules/utilities/newqueue.c"
	"${ProjDirPath}/modules/utilities/scheduler.c"

	"${common_peripherals}"
	"${STM32_HAL_sources}"
	"${stm32_usb_lib}"
	"${pensel_usb_files}"
)

add_executable(pensel_unittests.elf

	"${ProjDirPath}/applications/pensel_unittests.c"

	"${ProjDirPath}/modules/utilities/queue.c"
	"${ProjDirPath}/modules/utilities/newqueue.c"

	"${common_peripherals}"
	"${STM32_HAL_sources}"
	# "${stm32_usb_lib}"
)

# ----- target specific defines
target_compile_definitions(pensel_v2.elf PUBLIC PENSEL_V2)
target_compile_definitions(pensel_unittests.elf PUBLIC PENSEL_UNITTESTS)


# ---------------------- RESOLVING CIRCULAR DEPENDENCIES --------------------- #
# http://stackoverflow.com/questions/5651869/gcc-what-are-the-start-group-and-end-group-command-line-options

# ---- V2

TARGET_LINK_LIBRARIES(pensel_v2.elf -Wl,--start-group)

target_link_libraries(pensel_v2.elf debug m)
target_link_libraries(pensel_v2.elf debug c)
target_link_libraries(pensel_v2.elf debug gcc)
target_link_libraries(pensel_v2.elf debug nosys)
target_link_libraries(pensel_v2.elf optimized m)
target_link_libraries(pensel_v2.elf optimized c)
target_link_libraries(pensel_v2.elf optimized gcc)
target_link_libraries(pensel_v2.elf optimized nosys)

TARGET_LINK_LIBRARIES(pensel_v2.elf -Wl,--end-group)

# --- unittests

TARGET_LINK_LIBRARIES(pensel_unittests.elf -Wl,--start-group)

target_link_libraries(pensel_unittests.elf debug m)
target_link_libraries(pensel_unittests.elf debug c)
target_link_libraries(pensel_unittests.elf debug gcc)
target_link_libraries(pensel_unittests.elf debug nosys)
target_link_libraries(pensel_unittests.elf optimized m)
target_link_libraries(pensel_unittests.elf optimized c)
target_link_libraries(pensel_unittests.elf optimized gcc)
target_link_libraries(pensel_unittests.elf optimized nosys)

TARGET_LINK_LIBRARIES(pensel_unittests.elf -Wl,--end-group)

# ------------------------------- LINKER FILES ------------------------------- #
set(LINKER_LOCATION "${ProjDirPath}/build-system")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${LINKER_LOCATION}/pensel.ld -static")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${LINKER_LOCATION}/pensel.ld -static")
