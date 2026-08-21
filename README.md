# Aethera Engine

Aethera is a lightweight, image-first 2D simulation and game engine.

The core idea is simple: an image is the starting material. Aethera progressively turns visual content into structured objects with parts, motion, physics, and behavior without requiring a heavyweight general-purpose game engine.

## Current pipeline

```text
Image / Asset
      |
      v
Image Analysis
      |
      +---- background estimation
      +---- connected foreground regions
      +---- noise filtering
      +---- isolated transparent layers
      |
      v
Image Object
      |
      +---- Parts / Nodes
      +---- Parent / Child transforms
      +---- Appearance
      +---- Motion
      +---- Physics
      +---- Behavior
      |
      v
Renderer
```

The current image analyzer is deliberately dependency-light. It handles images with transparency and can also estimate a flat background from the four corners of an opaque image. It is a geometric/color segmentation layer, not a semantic vision model: it does not yet know that a region is a human arm, a fish fin, a window, or a tree branch.

Detected regions are converted to independent RGBA layers with transparent pixels outside the detected component. This means moving a detected region does not drag its original rectangular background with it.

Automatic semantic recognition, skeletal inference, IK, procedural animation, richer physics, and the Aethera scripting language are later layers in the roadmap.

## Build

Aethera uses C++20 and SDL2 for the platform window, input, and low-level 2D presentation. SDL2 is deliberately used as a thin dependency rather than as a game engine.

```bash
cmake -S . -B build
cmake --build build
```

The repository also contains a small analyzer test target:

```bash
cmake --build build --target aethera_image_test
./build/aethera_image_test
```
