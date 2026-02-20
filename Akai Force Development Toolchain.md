Akai Force MIDI Development Blueprint
Target Hardware: Akai Force (Cortex-A17, armv7l, glibc 2.39)
Host Environment: Fedora 42 (Podman/Quadlet)
Client Environment: Arch Linux (JetBrains Gateway/Code-OSS)
1. The Container Recipe (Containerfile)
   Location: Fedora Host
   Base: Debian Bookworm (glibc 2.36 - Safe for Force 2.39)
   dockerfile
   FROM docker.io/library/debian:bookworm-slim

RUN dpkg --add-architecture armhf && \
apt-get update && apt-get install -y --no-install-recommends \
crossbuild-essential-armhf \
libasound2-dev:armhf \
libncurses5-dev:armhf \
libncursesw5-dev:armhf \
libudev-dev:armhf \
clangd bear cmake git pkg-config procps python3 \
libxtst6 libxrender1 libfontconfig1 libxi6 libgtk-3-0 \
openssh-server ca-certificates wget && \
rm -rf /var/lib/apt/lists/*

RUN mkdir -p /root/.ssh /var/run/sshd && \
chmod 700 /root /root/.ssh && \
ssh-keygen -A && \
sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
echo "PasswordAuthentication no" >> /etc/ssh/sshd_config

ENV CC=arm-linux-gnueabihf-gcc
ENV CXX=arm-linux-gnueabihf-g++
ENV STRIP=arm-linux-gnueabihf-strip
ENV PKG_CONFIG_PATH=/usr/lib/arm-linux-gnueabihf/pkgconfig

WORKDIR /workspace
EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
Use code with caution.

2. Podman Quadlet Configs
   Location: Fedora Host (~/.config/containers/systemd/)
   akai-compilation.pod
   ini
   [Pod]
   PodName=akai-compilation-pod
   PublishPort=2022:22
   Use code with caution.

akai-compilation.container
ini
[Unit]
Description=Akai Force Toolchain
After=akai-compilation-pod.service

[Container]
ContainerName=akai-toolchain
Image=akai-force-final-v1
Pod=akai-compilation-pod.service
Volume=%h/path/to/project:/workspace:Z
Volume=%h/.config/akai-ssh/authorized_keys:/root/.ssh/authorized_keys:Z
Use code with caution.

3. Client SSH Config (~/.ssh/config)
   Location: Arch Laptop
   text
   Host akai-build
   HostName 192.168.68.59
   User root
   Port 2022
   IdentityFile ~/.ssh/id_ed25519
   ForwardAgent yes
   UserKnownHostsFile ~/.ssh/known_hosts_akai
   StrictHostKeyChecking no
   Use code with caution.

4. CMake Toolchain (force.cmake)
   Location: Project Root (/workspace/TXSEX/)
   cmake
   set(CMAKE_SYSTEM_NAME Linux)
   set(CMAKE_SYSTEM_PROCESSOR arm)
   set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)
   set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++)

# Force success for cross-compilation identification
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Optimizations for Akai Force Hardware
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=vfpv4 -O3 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfloat-abi=hard -mfpu=vfpv4 -O3 -fPIC")

include_directories(/usr/include/arm-linux-gnueabihf)
link_directories(/usr/lib/arm-linux-gnueabihf)
Use code with caution.

5. Main Build Logic (CMakeLists.txt)
   Location: Project Root
   cmake
   cmake_minimum_required(VERSION 3.10)
   project(TXSEX CXX)

add_definitions(-D__LINUX_ALSA__)
file(GLOB SOURCES "*.cpp")
add_executable(txsex_force ${SOURCES})

target_link_libraries(txsex_force asound ncurses m dl pthread)

# Automatic Post-Build Strip
add_custom_command(TARGET txsex_force POST_BUILD
COMMAND arm-linux-gnueabihf-strip --strip-unneeded $<TARGET_FILE:txsex_force>
COMMENT "Stripping binary for Akai Force..."
)
Use code with caution.

6. Akai Force Persistence (boot.sh)
   Location: Mockba SD Card Root
   bash
   #!/bin/bash
# Mount RAM disk over read-only SSH folder
mount -t tmpfs -o size=1m tmpfs /root/.ssh
mkdir -p /root/.ssh
chmod 700 /root/.ssh

# Sync keys from persistent SD card
if [ -f /media/662522/authorized_keys ]; then
cp /media/662522/authorized_keys /root/.ssh/authorized_keys
chmod 600 /root/.ssh/authorized_keys
fi
Use code with caution.

Key Commands for the Workflow:
Build Container: podman build -t akai-force-final-v1 .
Refresh Quadlet: systemctl --user daemon-reload && systemctl --user restart akai-compilation.service
Verify Binary: file cmake-build-debug-system/txsex_force
Manual Deploy: scp -P 2022 akai-build:/workspace/TXSEX/cmake-build-debug-system/txsex_force root@192.168.68.xx:/tmp/