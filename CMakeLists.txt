cmake_minimum_required(VERSION 3.10)
project(txSex C CXX)

# --- SETTINGS: Update these for new projects ---
set(BIN_NAME "txsex_force")
set(ZIP_NAME "txSex_Addon_v0.1.zip")
set(FORCE_HOST "akai-force")
set(FORCE_DEST "/media/662522/AddOns/txSex/")

# --- BUILD LOGIC ---
add_definitions(-D__LINUX_ALSA__)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=vfpv4 -O2")
include_directories(/usr/include/arm-linux-gnueabihf)
link_directories(/usr/lib/arm-linux-gnueabihf)

file(GLOB SOURCES "*.cpp")
add_executable(${BIN_NAME} ${SOURCES})
target_link_libraries(${BIN_NAME} asound ncurses m dl pthread)

# --- DEPLOYMENT CALL ---
set(DIST_DIR "${CMAKE_BINARY_DIR}/package_dist")

add_custom_command(TARGET ${BIN_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DIST_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/dist_template ${DIST_DIR}
        # Calls the generic script baked into the Containerfile
        COMMAND /usr/local/bin/deploy_force.sh $<TARGET_FILE:${BIN_NAME}> ${DIST_DIR} ${ZIP_NAME} ${FORCE_HOST} ${FORCE_DEST}
        COMMENT "Processing ${BIN_NAME} build, package, and deploy to ${FORCE_HOST}"
)
