#!/bin/bash

# Converted dockerfile to bash script

set -eux  # Enable debugging and exit on error

# Update and install dependencies
apt-get update && apt-get install -y --fix-missing \
    git \
    lsb-release \
    python3 \
    python3-pip \
    autoconf \
    bc \
    bison \
    dos2unix \
    gdb \
    gcc \
    lcov \
    make \
    flex \
    build-essential \
    ca-certificates \
    curl \
    device-tree-compiler \
    lcov \
    nano \
    valgrind \
    clang \
    bear

# Set working directory to /tmp
cd /tmp

# Install RISC-V Toolchain
ARCH=$(dpkg --print-architecture)
ARCH="${ARCH##*-}"

# Determine correct toolchain URL based on architecture
if [ "$ARCH" = "arm64" ]; then
    TOOLCHAIN_URL="https://github.com/langproc/langproc-2022-cw/releases/download/v1.0.0/riscv-gnu-toolchain-2022-09-21-ubuntu-22.04-arm64.tar.gz"
else
    TOOLCHAIN_URL="https://github.com/langproc/langproc-2022-cw/releases/download/v1.0.0/riscv-gnu-toolchain-2022-09-21-ubuntu-22.04-amd64.tar.gz"
fi

# Download and extract the toolchain
curl --output riscv-gnu-toolchain.tar.gz -L "$TOOLCHAIN_URL"
rm -rf /opt/riscv
tar -xzf riscv-gnu-toolchain.tar.gz --directory /opt
rm -rf riscv-gnu-toolchain.tar.gz

# Set environment variables
echo 'export PATH="/opt/riscv/bin:$PATH"' >> ~/.bashrc
echo 'export RISCV="/opt/riscv"' >> ~/.bashrc
source ~/.bashrc

# Verify installation
riscv64-unknown-elf-gcc --help || { echo "RISC-V GCC installation failed"; exit 1; }

# Install Spike RISC-V ISA Simulator
cd /tmp
git clone https://github.com/riscv-software-src/riscv-isa-sim.git
cd riscv-isa-sim
git checkout v1.1.0
mkdir build && cd build
../configure --prefix=$RISCV --with-isa=RV32IMFD --with-target=riscv32-unknown-elf
make -j$(nproc)
make install
cd /tmp && rm -rf riscv-isa-sim

# Verify Spike installation
spike --help || { echo "Spike installation failed"; exit 1; }

# Install riscv-pk (Proxy Kernel)
cd /tmp
git clone https://github.com/riscv-software-src/riscv-pk.git
cd riscv-pk
git checkout 573c858d9071a2216537f71de651a814f76ee76d
mkdir build && cd build
../configure --prefix=$RISCV --host=riscv64-unknown-elf --with-arch=rv32imfd --with-abi=ilp32d
make -j$(nproc)
make install
cd /tmp && rm -rf riscv-pk

echo "RISC-V toolchain and Spike simulator setup complete!"
exec /bin/bash  # Start a bash session
