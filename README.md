# Aethera Engine

Aethera is a lightweight, image-first 2D simulation and game engine.

The core idea is simple: an image is the starting material. Aethera progressively turns visual content into structured objects with parts, motion, physics, and behavior without requiring a heavyweight general-purpose game engine.

## Design principles

- Image-first content workflow
- Small native runtime
- 2D rendering only in the initial engine
- Procedural and skeletal motion rather than frame-by-frame animation by default
- Local physics and constraints instead of a large general-purpose physics stack
- Explicit separation between appearance, structure, motion, and behavior
- A scripting layer will eventually allow users to describe behavior without writing engine code

## Initial architecture

```text
Image / Asset
      |
      v
  Image Layer
      |
      v
Structured Object
      |
      +---- Parts / Joints
      +---- Appearance
      +---- Motion
      +---- Physics
      +---- Behavior
      |
      v
   Renderer
```

The repository currently contains the foundation only. Automatic image understanding, segmentation, skeletal inference, and the Aethera scripting language are planned layers rather than assumptions in the first implementation.

## Build

Aethera uses C++20 and SDL2 for the platform window, input, and low-level 2D presentation. SDL2 is deliberately used as a thin dependency rather than as a game engine.

```bash
cmake -S . -B build
cmake --build build
./build/aethera_demo
```

On systems where SDL2 is installed through a package manager, CMake will locate it through `find_package(SDL2 CONFIG REQUIRED)` or the platform's SDL2 CMake package.
