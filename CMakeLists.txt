cmake_minimum_required(VERSION 3.10)
project(TXSEX C CXX)

# Force cross-compile paths for Akai Force (armhf)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 2. Add the Optimizations (The rules for the tools)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=vfpv4 -O2 -funsigned-char")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=vfpv4 -O2 -funsigned-char")

# if there are errors running add
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie")

include_directories(/usr/include/arm-linux-gnueabihf)
link_directories(/usr/lib/arm-linux-gnueabihf)

# ... (Previous compiler and flag settings) ...

# 1. Define the source files (Mirroring the original *.cpp)
file(GLOB SOURCES "*.cpp")

# 2. CREATE THE TARGET (This must come first)
add_executable(txsex_force ${SOURCES})

# 3. LINK THE LIBRARIES
target_link_libraries(txsex_force asound ncurses m dl pthread)

# 4. ADD THE STRIP COMMAND (Now it can find the target)
add_custom_command(TARGET txsex_force POST_BUILD
        COMMAND arm-linux-gnueabihf-strip --strip-unneeded $<TARGET_FILE:txsex_force>
        COMMENT "Optimizing binary size for Akai Force storage..."
)

