# Hockey Scoreboard Monorepo

This project is a hockey scoreboard system consisting of a high-performance C++ renderer and a Flutter-based mobile control application.

## Project Structure

- **`scoreboard-system/`**: The core C++ application. It handles real-time rendering of the scoreboard and hosts a WebSocket server for remote control.
  - **Technologies**: C++26, CMake, SFML (Windowing/Events), Blend2D (2D Rendering), ixwebsocket (WebSocket Server), nlohmann_json (JSON parsing), vcpkg (Dependency Management).
  - **Key Components**:
    - `ScoreboardRenderer`: Uses Blend2D to draw the scoreboard UI.
    - `ScoreboardController`: Manages the game state (score, time, penalties).
    - `WebSocketManager`: Handles incoming commands from the mobile app.
    - `NetworkManager`: Manages mDNS service discovery (via `mdns.h`).
- **`scoreboard-app/`**: A Flutter mobile application to control the scoreboard remotely.
  - **Technologies**: Flutter (Dart), `bonsoir` (mDNS discovery), `web_socket_channel` (Communication).
  - **Key Components**:
    - `WebSocketService`: Manages the connection and command sending to the scoreboard system.
    - `ScoreboardControlPage`: The main UI for controlling the game.
    - `DiscoveryDialog`: UI for finding scoreboard instances on the local network.

## Building and Running

### Scoreboard System (C++)

The system requires `vcpkg` for dependencies and `ninja` for building.

1.  **Configure**:
    ```bash
    cd scoreboard-system
    ./configure.sh
    ```
    *Note: `configure.sh` currently uses hardcoded paths for CMake and Ninja. You may need to run a standard CMake command if those paths differ on your system:*
    ```bash
    cmake -B cmake-build-debug -G Ninja -DCMAKE_TOOLCHAIN_FILE=[PATH_TO_VCPKG]/scripts/buildsystems/vcpkg.cmake
    ```

2.  **Build**:
    ```bash
    ninja -C cmake-build-debug
    ```

3.  **Run**:
    ```bash
    ./cmake-build-debug/hockey-scoreboard
    ```

### Scoreboard App (Flutter)

1.  **Install Dependencies**:
    ```bash
    cd scoreboard-app
    flutter pub get
    ```

2.  **Run**:
    ```bash
    flutter run
    ```

## Development Conventions

### Communication Protocol
The system and app communicate via JSON over WebSockets (default port 8080).
- **State Updates**: The system broadcasts the full `ScoreboardState` whenever it changes.
- **Commands**: The app sends command objects:
  ```json
  {
    "command": "addHomeScore",
    "delta": 1
  }
  ```
  Available commands include: `setHomeScore`, `setAwayScore`, `addHomeScore`, `addAwayScore`, `addHomeShots`, `addAwayShots`, `setHomeTeamName`, `setAwayTeamName`, `setHomePenalty`, `setAwayPenalty`, `addHomePenalty`, `addAwayPenalty`, `toggleClock`, `resetGame`, `nextPeriod`, `setTime`, `setClockMode`.

### Coding Style
- **C++**: Uses modern C++26 features. Prefers RAII and clean separation between rendering logic and state management.
- **Flutter**: Follows standard Flutter/Dart linting rules. Uses a service-based approach for network communication.
