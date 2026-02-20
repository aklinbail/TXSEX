cmake_minimum_required(VERSION 3.10)
project(txSex C CXX)

# 1. TOOLCHAIN & ARCHITECTURE
# Force cross-compile paths for Akai Force (armhf)
# Keep the definitions and flags

# 2. COMPILER DEFINITIONS & OPTIMIZATIONS
# CRITICAL: This define enables the actual ALSA MIDI driver in the code
add_definitions(-D__LINUX_ALSA__)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=vfpv4 -O2 -funsigned-char")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=vfpv4 -O2 -funsigned-char")

# System-level include paths for the armhf toolchain
include_directories(/usr/include/arm-linux-gnueabihf)
link_directories(/usr/lib/arm-linux-gnueabihf)

# 3. SOURCE FILES & TARGET
# Finds all .cpp files in the project root
file(GLOB SOURCES "*.cpp")

# Define the executable target
add_executable(txsex_force ${SOURCES})

# Link all required libraries found in the original compile_pi.sh
target_link_libraries(txsex_force asound ncurses m dl pthread)

# 4. CONSOLIDATED POST-BUILD (STRIP, PACKAGE, ZIP, DEPLOY)
set(DIST_DIR "${CMAKE_BINARY_DIR}/txsex_addon")
set(TEMPLATE_DIR "${CMAKE_SOURCE_DIR}/dist_template")
set(FORCE_IP "192.168.68.xx")
set(FORCE_DEST "/media/662522/AddOns/txSex/")

add_custom_command(TARGET txsex_force POST_BUILD
        # A. Optimize the binary first
        COMMAND arm-linux-gnueabihf-strip --strip-unneeded $<TARGET_FILE:txsex_force>

        # B. Prepare the distribution folder
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:txsex_force> ${DIST_DIR}/txsex_force

        # C. Copy template files (README, XPM, .sh scripts) from dist_template
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${TEMPLATE_DIR} ${DIST_DIR}

        # D. Set permissions (The shell wrapper fixes the *.sh glob error)
        COMMAND chmod +x ${DIST_DIR}/txsex_force
        COMMAND /bin/sh -c "chmod +x ${DIST_DIR}/*.sh 2>/dev/null || true"

        # E. Create the ZIP for Mockba Addon Manager
        COMMAND zip -rj ${CMAKE_BINARY_DIR}/TX81z_Addon_v0.1.zip ${DIST_DIR}

        # F. AUTO-DEPLOY TO AKAI FORCE (Uses the 'akai-force' SSH alias)
        # This stops existing process, pushes new binary, and sets executable bit
        COMMAND ssh root@${FORCE_IP} "mkdir -p ${FORCE_DEST} && pkill txsex_force || true"
        COMMAND scp $<TARGET_FILE:txsex_force> root@${FORCE_IP}:${FORCE_DEST}
        COMMAND ssh root@${FORCE_IP} "chmod +x ${FORCE_DEST}txsex_force"

        COMMENT "Stripping, Packaging, Zipping, and Deploying txSex to Akai Force..."
)
