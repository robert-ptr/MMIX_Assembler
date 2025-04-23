cmake_minimum_required(VERSION 3.10)
project(mmix_tools C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -fsanitize=address")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

# === Assembler sources ===
set(ASSEMBLER_SRCS
    main.c
    debug.c
    parser.c
    scanner.c
    table.c
    trie.c
    common.c
)

add_executable(mmix_assembler ${ASSEMBLER_SRCS})


# === Simulator sources ===
#set(SIMULATOR_SRCS
#    vm.c
#    table.c
#    common.c
#)
#
#add_executable(mmix_sim ${SIMULATOR_SRCS})

