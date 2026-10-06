# Repository Guidelines

## Project Structure & Module Organization

This repository is an unofficial Linux/macOS port of Aliens vs Predator Gold. The root `CMakeLists.txt` defines a single `avp` executable and explicitly lists source files, so add new compiled files there. Core engine and platform code lives directly in `src/`. Game-specific logic is under `src/avp/`, shared legacy headers are in `src/include/`, SDL 1.2 support is in `src/sdl12/`, and legacy Win95/import tooling code is under `src/win95/` and `src/avp/win95/`. There is no separate assets tree in this repository; runtime game data is provided externally.

## Build, Test, and Development Commands

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug` configures a debug build in `build/`.
- `cmake -S . -B build -DSDL_TYPE=SDL2` selects SDL2 explicitly; use `SDL` for SDL 1.2 or omit it for auto-detection.
- `cmake --build build` builds the `avp` executable.
- `AVP_DATA=/path/to/lowercase/game-data ./build/avp` runs the game using external data files.

Required dependencies are CMake, SDL 1.2 or SDL2, OpenAL, and OpenGL.

## Coding Style & Naming Conventions

Use the repository `.clang-format` for C/C++ changes: 4-space indentation, no tabs, 100-column limit, right-aligned pointers, and unsorted includes. Keep existing legacy naming where editing old systems, including uppercase globals/macros and subsystem prefixes such as `bh_`. Prefer focused modernization that matches nearby code; do not rewrite broad legacy patterns unless the change requires it. Use `.cpp`, `.h`, and `.hpp` consistently with adjacent files.

## Testing Guidelines

No automated test suite or CTest targets are currently configured. Validate changes with at least `cmake --build build`. For gameplay, input, rendering, audio, or data-loading changes, perform a local smoke test with `AVP_DATA` pointing at a lowercase Gold edition data directory. When adding future tests, keep them in a clearly named `tests/` directory and document the command here.

## Commit & Pull Request Guidelines

Recent commits use short imperative summaries such as `fixed missing header` and `replaced List instances with std::vector`. Keep subjects concise and describe the changed behavior or refactor. Pull requests should include a brief purpose statement, build result, manual smoke-test notes when relevant, linked issues, and screenshots or recordings for visible rendering/UI changes.

## Security & Configuration Tips

Do not commit proprietary game data, local user settings, or generated build output. Runtime settings live in `~/.avp`; build artifacts should stay under `build/` or another ignored directory.
