# Aethera Engine

Aethera is a lightweight C++20 image-first 2D runtime. The core idea is to use an ordinary image as the starting material and progressively turn it into a programmable, deformable actor without requiring a heavyweight general-purpose game engine.

## MVP pipeline

```text
Image file
   ↓
RGBA pixels
   ↓
Shape mesh
   ↓
Hierarchical skeleton
   ↓
Automatic bone weights
   ↓
Weighted skinning
   ↓
Procedural animation
   ↓
SDL geometry renderer
```

The main runtime pieces are:

- `ImageLoader`: decodes an image into compact `ImageRgba8` pixels.
- `ShapeMeshGenerator`: creates a mesh using the image alpha as visibility information.
- `SkeletonPose`: maintains parent/child joint transforms.
- `ImagePipeline`: builds an `ImageActor` from an RGBA image.
- `MeshDeformer`: performs weighted bone deformation.
- `AnimationController`: provides simple procedural poses.
- `ImageActorRenderer`: uploads the RGBA image and renders the deformed mesh through SDL geometry.

## Build

Requirements:

- CMake 3.20+
- C++20 compiler
- SDL2 development package
- SDL2_image is optional. When available, `ImageLoader` can use formats supported by SDL_image. Without it, BMP remains available through SDL2.

```bash
cmake -S . -B build
cmake --build build
```

## Run the MVP

```bash
./build/aethera_demo path/to/image.png
```

The demo loads the supplied image, constructs an `ImageActor`, applies the idle procedural pose each frame, and renders the resulting skinned mesh.

## Project status

The lightweight runtime/MVP is implemented. Production-quality automatic computer vision is a separate layer: high-quality semantic segmentation, human/animal-specific skeleton models, a full visual editor, export tooling, and production-grade rigs still require additional implementation and training assets. These are not represented as completed features in this MVP.
