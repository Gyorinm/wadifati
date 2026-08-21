# Aethera Engine

Aethera is a lightweight C++20 image-first 2D runtime. An ordinary image is the starting material; the runtime turns its pixels into a programmable, deformable actor without a heavyweight general-purpose game engine.

## Runtime pipeline

```text
Image file
   ↓
RGBA pixels
   ↓
Alpha-aware shape mesh
   ↓
Hierarchical skeleton
   ↓
Automatic two-bone weights
   ↓
Weighted skinning
   ↓
Procedural animation
   ↓
SDL geometry renderer
```

Core components:

- `ImageLoader`: decodes an image into `ImageRgba8`.
- `ShapeMeshGenerator`: generates a mesh using alpha visibility.
- `SkeletonPose`: propagates parent/child joint transforms.
- `ImagePipeline`: creates an `ImageActor` from an RGBA image.
- `MeshDeformer`: applies weighted bone deformation.
- `AnimationController`: provides reusable idle/sway poses.
- `ImageActorRenderer`: renders a deformed actor through SDL geometry.
- `SceneFile`: saves and restores image actors, meshes and skeleton poses.
- `VisionBackend`: a swappable interface for deterministic heuristics now and model-backed vision later.

## Build

Requirements: CMake 3.20+, a C++20 compiler, and SDL2 development files. SDL2_image is optional; when available it enables image formats supported by SDL_image. Without it, BMP remains available through SDL2.

```bash
cmake -S . -B build
cmake --build build
```

## Run

Render and animate an image:

```bash
./build/aethera_demo path/to/image.png
```

Create a portable Aethera scene file from an image:

```bash
./build/aethera_import path/to/image.png character.aethera
```

The scene format is intentionally small and self-contained. It stores RGBA pixels, mesh vertices/indices, skeleton pose, and bone bindings.

## Status

The lightweight runtime is complete as an MVP. It can load an image, construct a mesh and skeleton, deform the mesh, animate it, render it, and persist the actor as a scene file.

The remaining research/product layers are intentionally separate: high-quality learned semantic segmentation, reliable human/animal-specific automatic rigs, a full visual editor, and production export/build tooling. They require model weights, training/evaluation data, and substantially larger UI/tooling work; they are not falsely presented here as finished.
