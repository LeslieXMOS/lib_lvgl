set(LIB_NAME lib_lvgl)
set(LIB_VERSION 9.2.0)

set(LVGL_PATH       ${CMAKE_CURRENT_LIST_DIR}/submodules/lvgl)
set(LIB_INCLUDES    api submodules/lvgl)

file(GLOB_RECURSE LIB_C_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/src/*.c" "${LVGL_PATH}/src/*.c" "${LVGL_PATH}/demos/*.c" "${LVGL_PATH}/examples/*.c")
file(GLOB_RECURSE LIB_CXX_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} "${LVGL_PATH}/src/*.cpp")

set(LIB_COMPILER_FLAGS)

execute_process(
    COMMAND git reset --hard
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/submodules/lvgl
)
execute_process(
    COMMAND git apply ../lvgl_xmos.patch
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/submodules/lvgl
)

XMOS_REGISTER_MODULE()