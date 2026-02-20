🎹 Akai Force MIDI Development Infrastructure
1. The Image Recipe (Containerfile)
   Location: Fedora Host (~/akai-build/Containerfile)
   This builds the Debian Bookworm environment with all discovered MIDI (asound), UI (ncurses), and IDE (JetBrains) dependencies.
   dockerfile
# Lock to Debian Bookworm (Stable) for glibc 2.36 compatibility
FROM docker.io/library/debian:bookworm-slim

# 1. Enable armhf architecture and install ALL dependencies
RUN dpkg --add-architecture armhf && \
apt-get update && apt-get install -y --no-install-recommends \
# The Cross-Compiler & MIDI/UI Libs (armhf)
crossbuild-essential-armhf \
libasound2-dev:armhf \
libncurses5-dev:armhf \
libncursesw5-dev:armhf \
libudev-dev:armhf \
# Build & IDE Support (Procps/Python are required by JetBrains)
clangd bear cmake git pkg-config procps python3 \
# GUI Libraries (Required for the Headless IDE JRE to boot)
libxtst6 libxrender1 libfontconfig1 libxi6 libgtk-3-0 \
# Infrastructure
openssh-server ca-certificates wget zip && \
rm -rf /var/lib/apt/lists/*

# 2. SSH Configuration (Fixed for Key-only access from Arch)
RUN mkdir -p /root/.ssh /var/run/sshd && \
chmod 700 /root /root/.ssh && \
ssh-keygen -A && \
sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
echo "PasswordAuthentication no" >> /etc/ssh/sshd_config

# 3. Akai Force Environment Variables (Cortex-A17)
ENV CC=arm-linux-gnueabihf-gcc
ENV CXX=arm-linux-gnueabihf-g++
ENV STRIP=arm-linux-gnueabihf-strip

WORKDIR /workspace
EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]
Use code with caution.

2. Fedora Quadlets (Split Pod/Container)
   Location: Fedora Host (~/.config/containers/systemd/)
   akai-compilation.pod
   ini
   [Unit]
   Description=Akai Force Development Pod

[Pod]
PodName=akai-compilation-pod
# Publish the SSH port (2022) to the Fedora host
PublishPort=2022:22

[Install]
WantedBy=default.target
Use code with caution.

akai-compilation.container
ini
[Unit]
Description=Akai Force txSex Toolchain Container
After=akai-compilation-pod.service

[Container]
ContainerName=akai-toolchain
Image=akai-force-toolchain:v1
Pod=akai-compilation.pod

# Persistent Code Mount
Volume=%h/workspace/TXSEX:/workspace:Z

# Host SSH Keys for Arch Handshake
Volume=%h/.config/akai-ssh/authorized_keys:/root/.ssh/authorized_keys:Z

# Persistent JetBrains IDE Cache (Preserves CLion backend/settings)
Volume=akai-jetbrains-cache:/root/.cache/JetBrains

[Service]
Restart=always

[Install]
WantedBy=default.target
Use code with caution.

3. Arch Linux Client Config (~/.ssh/config)
   Location: Arch Laptop
   Note: Use IdentitiesOnly yes to prevent "Too many authentication failures" when you have multiple keys.
   text
   Host akai-build
   HostName 192.168.68.59
   User root
   Port 2022
   IdentityFile ~/.ssh/id_ed25519
   ForwardAgent yes
   IdentitiesOnly yes
   StrictHostKeyChecking no
   UserKnownHostsFile /dev/null

Host akai-force
HostName 192.168.68.xx
User root
IdentityFile ~/.ssh/id_ed25519
IdentitiesOnly yes
ForwardAgent yes
StrictHostKeyChecking no
UserKnownHostsFile /dev/null
Use code with caution.

4. Setup & Maintenance Instructions
   A. Build the Environment (Fedora Host)
   Run these commands to initialize the infrastructure:
   Build Image: podman build -t akai-force-toolchain:v1 ~/akai-build/
   Apply Quadlets: systemctl --user daemon-reload
   Start Services: systemctl --user start akai-compilation.service
   B. Connect from Arch (First Time)
   Start Agent: eval $(ssh-agent -s) && ssh-add ~/.ssh/id_ed25519
   Import Env (GUI Support): systemctl --user import-environment SSH_AUTH_SOCK
   Launch IDE: Open JetBrains Gateway, connect to akai-build, and point to /workspace.
   C. The "txSex" CMake Fixes
   Ensure your CMakeLists.txt links the libraries discovered in the original shell script:
   cmake
# Add to CMakeLists.txt
target_link_libraries(txsex_force asound ncurses m dl pthread)

# Add the 30ms TX81z safety gate to your onMIDI() function as discussed.
Use code with caution.

D. Persistence on the Force (Mockba Build)
Since the Force root is read-only, ensure your boot.sh on the SD card mounts a RAM-disk over /root/.ssh to keep your authorized keys working across reboots.