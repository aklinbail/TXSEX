cmake_minimum_required(VERSION 3.10)
project(txSex C CXX)

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

# --- DEPLOYMENT PACKAGING ---
set(DIST_DIR "${CMAKE_BINARY_DIR}/txsex_addon")

add_custom_command(TARGET txsex_force POST_BUILD
        # 1. Create a clean distribution folder
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_DIR}

        # 2. Copy the fresh binary
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:txsex_force> ${DIST_DIR}/txsex_force

        # 3. Copy all template files (Scripts, README, XPM)
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/dist_template ${DIST_DIR}

        # 4. Set executable permissions for the shell scripts
        COMMAND chmod +x ${DIST_DIR}/*.sh
        COMMAND chmod +x ${DIST_DIR}/txsex_force

        # 5. Create the ZIP file
        COMMAND zip -rj ${CMAKE_BINARY_DIR}/TX81z_Addon_v0.1.zip ${DIST_DIR}

        COMMENT "Building Mockba Addon Zip Package..."
)
