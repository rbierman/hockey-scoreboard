# ScoreMaster Monorepo

This repository contains the ScoreMaster system, a professional-grade hockey scoreboard solution.

## Project Structure

- **`scoremaster-controller/`**: High-performance C++26 scoreboard engine. Handles game logic, 2D rendering (Blend2D), and network communication.
- **`scoremaster-app/`**: Flutter mobile application for remote scoreboard management.

## Installation

Pre-built binaries for all platforms are available on the [GitHub Releases](https://github.com/rbierman/scoremaster/releases) page.

### 1. ScoreMaster Controller (Linux)

The controller is distributed as a Debian package for Raspberry Pi and other Linux systems.

- **Standard**: Includes a local SFML window for display.
- **Headless**: Optimized for devices without a display attached.

To install:
1. Download the appropriate `.deb` package.
2. Install using `dpkg`:
   ```bash
   sudo dpkg -i scoremaster-controller-*.deb
   sudo apt-get install -f # Install missing dependencies
   ```
3. The controller runs as a systemd service:
   ```bash
   sudo systemctl start scoremaster-controller
   ```

### 2. ScoreMaster App

#### Android
- Download and open the `.apk` file on your Android device to install.

#### Linux Desktop
1. Download `scoremaster-app-linux.tar.gz`.
2. Extract and run:
   ```bash
   tar -xzvf scoremaster-app-linux.tar.gz
   ./scoremaster_app
   ```

## Development & Releasing

For detailed development instructions, build guides, and release processes for each component, please refer to their respective README files:

- [ScoreMaster Controller (C++)](scoremaster-controller/README.md)
- [ScoreMaster App (Flutter)](scoremaster-app/README.md)
