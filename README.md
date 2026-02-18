# ScoreMaster Monorepo

This repository contains the ScoreMaster system, a professional-grade hockey scoreboard solution.

## Project Structure

- **`scoremaster-controller/`**: High-performance C++26 scoreboard engine. Handles game logic, 2D rendering (Blend2D), and network communication.
- **`scoremaster-app/`**: Flutter mobile application for remote scoreboard management.

## Releasing

Releases for the controller are handled via GitHub Actions. To trigger a new release (which builds Debian packages for standard and headless targets):

```bash
gh workflow run release.yml
```

This will automatically increment the patch version and create a new GitHub Release with the compiled `.deb` artifacts.
