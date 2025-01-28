# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Nome do projeto
project(Contador_LED C CXX ASM)

# Inicializa o Raspberry Pi Pico SDK
pico_sdk_init()

# Define o executável
add_executable(Contador_LED Contador_LED.c)

# Configurações de nome e versão do programa
pico_set_program_name(Contador_LED "Contador_LED")
pico_set_program_version(Contador_LED "0.1")

# Gera o cabeçalho PIO a partir do arquivo .pio
pico_generate_pio_header(Contador_LED ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

# Habilita entrada/saída por UART/USB
pico_enable_stdio_uart(Contador_LED 1)
pico_enable_stdio_usb(Contador_LED 1)

# Vincula as bibliotecas necessárias ao programa
target_link_libraries(Contador_LED
    pico_stdlib
    hardware_pio  # Biblioteca de suporte para PIO
)

# Inclui os diretórios padrão e adicionais no projeto
target_include_directories(Contador_LED PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Gera os arquivos de saída adicionais (como .uf2)
pico_add_extra_outputs(Contador_LED)


