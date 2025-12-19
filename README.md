# Terminal Chat

A simple terminal-based chat application using C++ and Asio.

## Prerequisites

### Linux
Install Asio via your package manager:
```bash
# Ubuntu/Debian
sudo apt-get install libasio-dev

# Fedora/RHEL/CentOS
sudo dnf install asio-devel

# Arch Linux
sudo pacman -S asio
```

### Windows
Install Asio via vcpkg:
```bash
# Install vcpkg if you don't have it
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install Asio
.\vcpkg install asio
```

Or download Asio headers manually and place them in a `third_party/asio/include` directory.

## Building

### Linux/macOS
```bash
mkdir build && cd build
cmake ..
make
```

### Windows
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage

Run the server:
```bash
# Linux/macOS
./server/server

# Windows
.\server\Debug\server.exe
```

Run the client:
```bash
# Linux/macOS
./client/client

# Windows
.\client\Debug\client.exe
```
