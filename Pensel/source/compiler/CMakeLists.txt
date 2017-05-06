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

# ------------------------------ FLAGS USED BY EVERYONE ------------------------------ #
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -flto")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -mcpu=cortex-m4")
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -Wall -Wdouble-promotion -Wextra")
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
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -DSTM32F302x8")

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
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MMD")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MP")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")

# ------------------------------ C LANGUAGE FLAGS - RELEASE ------------------------------ #
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
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker -defsym=__stack_size__=0xcc0")  # ~20% of ram
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker -defsym=__heap_size__=0x0")
# ------------------------------ LINKER FLAGS - RELEASE ------------------------------ #
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_GENERAL}")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Xlinker -Map=${CMAKE_MAP_PATH}")


# ------------------------------ LINKER FLAGS - DEBUG ------------------------------ #
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_GENERAL}")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -Xlinker -Map=${CMAKE_MAP_PATH}")


# ------------------------------ INCLUDE DIRECTORIES ------------------------------ #

include_directories(${ProjDirPath})
include_directories(${ProjDirPath}/CMSIS/Include/)
include_directories(${ProjDirPath}/CMSIS/Device/ST/STM32F3xx/Include/)

# ------------------------------ SOURCE FILES ------------------------------ #

add_executable(pensel.elf
	# user code
	"${ProjDirPath}/LSM303DLHC.c"
	"${ProjDirPath}/LSM303DLHC.h"
	"${ProjDirPath}/common.h"
	"${ProjDirPath}/main.c"
	"${ProjDirPath}/cli.c"
	"${ProjDirPath}/cli.h"
	"${ProjDirPath}/reports.c"
	"${ProjDirPath}/reports.h"
	"${ProjDirPath}/queue.c"
	"${ProjDirPath}/queue.h"
	"${ProjDirPath}/UART.c"
	"${ProjDirPath}/UART.h"
	"${ProjDirPath}/I2C.c"
	"${ProjDirPath}/I2C.h"
	"${ProjDirPath}/ADC.c"
	"${ProjDirPath}/ADC.h"
	"${ProjDirPath}/hardware.c"
	"${ProjDirPath}/hardware.h"
	"${ProjDirPath}/startup_stm32f302x8.s"

	# Driver configuration files
	"${ProjDirPath}/stm32f3xx_hal_conf.h"
	"${ProjDirPath}/stm32f3xx_it.h"
	"${ProjDirPath}/stm32f302x8.h"
	"${ProjDirPath}/stm32f3xx_hal_msp.c"
	"${ProjDirPath}/system_stm32f3xx.c"
	"${ProjDirPath}/stm32f3xx_it.c"
	"${ProjDirPath}/system_stm32f3xx.h"

	# --- CMSIS Stuff ----
	"${ProjDirPath}/CMSIS/Include/core_cm4.h"
	"${ProjDirPath}/CMSIS/Include/arm_common_tables.h"
	"${ProjDirPath}/CMSIS/Include/arm_const_structs.h"
	"${ProjDirPath}/CMSIS/Include/arm_math.h"
	"${ProjDirPath}/CMSIS/Include/cmsis_armcc.h"
	"${ProjDirPath}/CMSIS/Include/cmsis_armcc_V6.h"
	"${ProjDirPath}/CMSIS/Include/cmsis_gcc.h"
	"${ProjDirPath}/CMSIS/Include/core_cm4.h"
	"${ProjDirPath}/CMSIS/Include/core_cmFunc.h"
	"${ProjDirPath}/CMSIS/Include/core_cmInstr.h"
	"${ProjDirPath}/CMSIS/Include/core_cmSimd.h"
	"${ProjDirPath}/CMSIS/Include/core_sc000.h"
	"${ProjDirPath}/CMSIS/Include/core_sc300.h"
	"${ProjDirPath}/CMSIS/Device/ST/STM32F3xx/Include/stm32f302x8.h"
	"${ProjDirPath}/CMSIS/Device/ST/STM32F3xx/Include/stm32f3xx.h"
	# "${ProjDirPath}/CMSIS/Lib/GCC/libarm_cortexM4lf_math.a"

	# --- Drivers from vender -----
	# some basic definitions and startup code
	"${ProjDirPath}/Drivers/stm32_assert_template.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_def.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_cortex.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_cortex.h"
	# peripheral drivers
	"${ProjDirPath}/Drivers/stm32f3xx_hal_gpio.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_gpio.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_gpio_ex.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_hrtim.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_hrtim.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_i2c.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_i2c.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_i2c_ex.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_i2c_ex.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_uart.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_uart.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_uart_ex.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_uart_ex.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_dma.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_dma.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_rcc.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_rcc.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_rcc_ex.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_rcc_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_flash.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_flash.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_adc.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_adc.h"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_adc_ex.c"
	"${ProjDirPath}/Drivers/stm32f3xx_hal_adc_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_wwdg.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_wwdg.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_usart.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_usart.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_usart_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_can.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_can.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_cec.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_cec.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_comp.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_comp.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_comp_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_conf_template.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_crc.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_crc.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_crc_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_crc_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_dac.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_dac.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_dac_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_dac_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_dma_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_flash_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_flash_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_i2s.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_i2s.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_i2s_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_i2s_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_irda.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_irda.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_irda_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_iwdg.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_iwdg.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_msp_template.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_nand.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_nand.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_nor.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_nor.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_opamp.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_opamp.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_opamp_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_opamp_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pccard.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pccard.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pcd.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pcd.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pcd_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pcd_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pwr.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pwr.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pwr_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_pwr_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_rtc.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_rtc.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_rtc_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_rtc_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_sdadc.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_sdadc.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_smartcard.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_smartcard.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_smartcard_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_smartcard_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_smbus.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_smbus.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_spi.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_spi.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_spi_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_spi_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_sram.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_sram.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_tim.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_tim.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_tim_ex.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_tim_ex.h"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_timebase_rtc_alarm_template.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_timebase_rtc_wakeup_template.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_timebase_tim_template.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_tsc.c"
	# "${ProjDirPath}/Drivers/stm32f3xx_hal_tsc.h"

)


# ------------------------------ LINKER FILES ------------------------------ #
set(LINKER_LOCATION "${ProjDirPath}/compiler")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -T${LINKER_LOCATION}/pensel.ld -static")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -T${LINKER_LOCATION}/pensel.ld -static")



# --------------------- RESOLVING CIRCULAR DEPENDENCIES -------------------- #
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
