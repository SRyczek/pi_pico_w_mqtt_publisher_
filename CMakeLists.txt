
cmake_minimum_required(VERSION 3.18)

set(PROGRAM_NAME mqtt_lib)

# Pull in PICO SDK
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(mqtt_lib C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD pico_w)

pico_sdk_init()

add_library(lwipopts INTERFACE)


target_include_directories(lwipopts INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/lwipopts
        ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(mqtt_lib
               mqtt_lib.c
               )

target_link_libraries(mqtt_lib 
                      pico_lwip
                      pico_lwip_nosys
                      pico_lwip_mqtt
                      pico_stdlib
                      pico_lwip_mbedtls
                      pico_mbedtls
                      pico_cyw43_arch_lwip_poll
                      lwipopts) 

pico_enable_stdio_usb(mqtt_lib 1)
pico_enable_stdio_uart(mqtt_lib 0)
                        
pico_add_extra_outputs(mqtt_lib)
