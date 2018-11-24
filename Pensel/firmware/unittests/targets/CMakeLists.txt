SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

SET(CLICOLOR_FORCE ON)

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)


# CURRENT DIRECTORY up a level
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR}/../)
SET(MainProjectPath ${CMAKE_CURRENT_SOURCE_DIR}/../../)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)

# -------------------------- FLAGS USED BY EVERYONE -------------------------- #

SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -Wall -Wdouble-promotion -Wextra")
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
SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -DGIT_TAG_SHORT=UNITTESTS")
if(${VERBOSE_OUTPUT} EQUAL 1)
	message("hello")
	SET(CMAKE_GENERAL_FLAGS "${CMAKE_GENERAL_FLAGS} -DVERBOSE_OUTPUT")
endif()


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

SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker --gc-sections")
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker -static")
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker -z")
SET(CMAKE_EXE_LINKER_FLAGS_GENERAL "${CMAKE_EXE_LINKER_FLAGS_GENERAL} -Xlinker muldefs")

# --------------------------- LINKER FLAGS - RELEASE ------------------------- #
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${CMAKE_GENERAL_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${CMAKE_EXE_LINKER_FLAGS_GENERAL}")

# ---------------------------- LINKER FLAGS - DEBUG -------------------------- #
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_GENERAL_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CMAKE_EXE_LINKER_FLAGS_GENERAL}")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g")

# ------------- DEBUG BUILD FLAGS (no optimizations) ------------- #

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_CLANG_FLAGS} ${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "c compiler flags debug")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE INTERNAL "cxx compiler flags debug")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}" CACHE INTERNAL "linker flags debug")

# ------------- RELEASE BUILD FLAGS (use optimizations) ------------- #

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_CLANG_FLAGS} ${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "c compiler flags release")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE INTERNAL "cxx compiler flags release")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE INTERNAL "linker flags release")


# ---------------------------- INCLUDE DIRECTORIES --------------------------- #

# TODO: allow this to be a define passed in
SET(UNITY_PATH  "/usr/local/resources/tools/Unity/src/")

include_directories(${ProjDirPath}/stubs/)
include_directories(${MainProjectPath})
include_directories(${UNITY_PATH})

# ---------------------------- SOURCE FILE LISTS ----------------------------- #

list(APPEND unity_files
	"${UNITY_PATH}/unity.c"
)

# ------------------------------ EXECTUABLES --------------------------------- #

add_executable(test_queue
	"${ProjDirPath}/targets/test_queue.c"
	"${MainProjectPath}/modules/utilities/queue.c"

	"${unity_files}"
)

add_executable(test_newqueue
	"${ProjDirPath}/targets/test_newqueue.c"
	"${MainProjectPath}/modules/utilities/newqueue.c"

	"${unity_files}"
)

add_executable(test_matrixmath
	"${ProjDirPath}/targets/test_matrixmath.c"
	"${MainProjectPath}/modules/orientation/matrixmath.c"

	"${unity_files}"
)

add_executable(test_FIR
	"${ProjDirPath}/targets/test_FIR.c"
	"${ProjDirPath}/targets/FIR_sine_array.c"
	"${MainProjectPath}/modules/utilities/FIR.c"

	"${unity_files}"
)

add_executable(test_logging
	"${ProjDirPath}/targets/test_logging.c"
	"${MainProjectPath}/modules/utilities/logging.c"

	"${unity_files}"
)