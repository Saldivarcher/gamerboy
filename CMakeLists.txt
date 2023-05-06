cmake_minimum_required(VERSION 3.8)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(gamerboy)

# Enable C++20 (Required)
set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED SDL2)
include_directories(SYSTEM ${SDL2_INCLUDE_DIR})

include_directories(include)

set(gamerboy_sources
	src/cartridge.cc
	src/memory.cc
	src/ppu.cc
	src/cpu.cc
	src/gameboy.cc
	src/main.cc)


add_executable(gamerboy ${gamerboy_sources})
target_link_libraries(gamerboy PRIVATE SDL2)

add_custom_command(TARGET gamerboy POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${CMAKE_CURRENT_SOURCE_DIR}/boot_rom/dmg_boot.bin
                       $<TARGET_FILE_DIR:gamerboy>
                   )

install(TARGETS gamerboy RUNTIME DESTINATION bin)
install(FILES boot_rom/dmg_boot.bin DESTINATION bin)
