# CLAUDE.md — C++ Software Renderer

A tile-based, multi-threaded forward-rendering software rasterizer written in C++17.
Renders 3D scenes with Blinn-Phong or PBR (Cook-Torrance/GGX) shading, outputs PPM
images, and displays interactively via SDL3.

## Build

```bash
# Configure + build
cmake -S . -B build/Debug   -DCMAKE_BUILD_TYPE=Debug   -G Ninja && ninja -C build/Debug   -j8
cmake -S . -B build/Release -DCMAKE_BUILD_TYPE=Release -G Ninja && ninja -C build/Release -j8

# Incremental
ninja -C build/Debug -j8
ninja -C build/Release -j8
```

Dependencies: SDL3 (Homebrew on macOS, vendored submodule on Windows), nlohmann/json
(vendored), stb_image (vendored). All paths are CWD-independent — resolved via the
`PROJECT_SOURCE_DIR` compile definition injected by CMake.

## Run

```bash
./build/Release/renderer <scene> <shader> [--interactive] [--parallel] [--headless] [-e out.ppm]

# Examples:
./build/Release/renderer WoodenStool   PBR --interactive --parallel
./build/Release/renderer ConcreteCatStatue PBR --headless --parallel -e results/out.ppm
```

Available scenes: `WoodenStool`, `ConcreteCatStatue`, `LubricantSpray`,
`MidCenturyLoungeChair`, `Lemon`, `CornellBox-Normal`, `Crate1`.

Shaders: `PBR`, `BlinnPhong`.

## Directory layout

```
src/
├── main.cpp                  Entry point, CLI parsing
├── utils.hpp                 Debug print helpers
├── math/
│   ├── math_all.h            Aggregate include
│   ├── vector.hpp            Vector2/3/4<T> templates, dot, cross, normalize
│   ├── matrix.hpp            4×4 Matrix, lookAt, perspective, rotate, translate
│   ├── geometry.hpp          inside_triangle, edge_function, barycentric, PBR GGX/Fresnel
│   ├── color.h               Predefined colour constants
│   ├── constants.h           PI
│   └── quaterions.hpp        Empty placeholder
├── renderer/
│   ├── engine.h/cpp          Top-level orchestrator (lifecycle, framebuffer, main loop)
│   ├── renderer.h/cpp        Render pipeline: single-thread + tile-parallel paths
│   ├── rasterizer.h/cpp      Triangle scan conversion (fill + wireframe), viewport transform
│   ├── shader.h/cpp          VS/FS typedefs; Blinn-Phong, PBR, flat, wireframe implementations
│   ├── shader_constants.h    MaterialData, ShaderConstants structs
│   ├── render_types.hpp      Vertex, Varying, Uniform, LightUniform, interpolate
│   ├── render_states.h       PolygonMode, CullMode, RenderState
│   ├── framebuffer.h/cpp     Colour + depth buffers, depth_test
│   ├── tile_binner.h         Header-only: ProcessedTriangle, TileBin, TileBinner (64×64 tiles)
│   ├── job_system.h          Header-only thread pool with start-barrier + atomic job claiming
│   ├── displayer.h/cpp       SDL3 window + RGBA texture upload
│   ├── input_handler.h/cpp   SDL3 event polling, keyboard + mouse FPS input
│   └── resource_manager.h/cpp Shader/material/mesh/texture registry
├── scene/
│   ├── scene.h/cpp           Scene: models, lights, camera
│   ├── scene_loader.h/cpp    JSON scene loader, compute_tangents(), TextureLoader
│   ├── model.h/cpp           Model: mesh ref, submeshes, transform
│   ├── mesh.h                Mesh: vertices, indices
│   ├── material.h/cpp        BlinnPhongMaterial, PBRMaterial; weak_ptr to textures + shader
│   ├── texture.h/cpp         Nearest-neighbour texture sampling
│   ├── light.h               DirectionalLight, PointLight, SpotLight
│   ├── camera.h/cpp          Perspective camera definition
│   ├── fps_camera.h          Header-only FPS camera (yaw/pitch, WASDQE)
│   └── transform.h           Header-only Transform (scale, translation, rotation)
└── thirdparty/
    ├── json.hpp              nlohmann/json v3.11.3
    └── stb_image.h           stb_image v2.30
```

## Data flow (tile-parallel path)

```
render_parallel():
  Step 1 (serial)  — collect_draw_batches()    ~1 ms
  Step 2 (parallel) — vertex processing          chunked 64 tris/job, start-barrier
  Step 3 (serial)  — TileBinner AABB binning    ~5 ms / 11K tris, 64×64 tiles
  Step 4 (parallel) — per-tile rasterisation     zero sync (exclusive pixel regions)
```

## Key structural conventions

- **`VertexOut` = `FragmentIn` = `Varying`** (typedefs in `shader.h`). The vertex shader
  outputs `Varying`; the rasterizer interpolates barycentrically; the fragment shader
  receives `Varying` (as `FragmentIn`).
- **Shaders are `std::function` wrappers.** The `Shader` class holds a VS + FS pair.
  `ShaderConstants` bundles `Uniform` + `LightUniform` + `MaterialData` for both stages.
- **`ProcessedTriangle`** (~336 bytes) holds post-VS data (screen_pos, ndc_pos, varyings,
  clip_w, shader pointer, batch_idx). The `batch_idx` avoids copying `ShaderConstants`
  (~600 bytes) per triangle.
- **Tile bounds are EXCLUSIVE upper bounds** (`[min, max)`), matching `ceil()` + `x < max`.
- **`JobSystem::parallel_for`** uses a start-barrier: all threads (workers + main)
  rendezvous before claiming jobs atomically. This prevents the main thread from
  consuming half the work before workers wake up.
- **Perspective-correct interpolation** uses clip_w: `alpha_c = (alpha*w1*w2) / Z` where
  `Z = alpha*w1*w2 + beta*w0*w2 + gamma*w0*w1`.

## Recently completed (2026-07-02, dev branch)

| Change | Files | Notes |
|--------|-------|-------|
| Fix `should_clip` `&&` → `\|\|` | `renderer.cpp` | Trivial-reject was too conservative |
| World-space backface culling | `renderer.cpp` (2 sites) | `dot(face_normal, view_dir) <= 0` before VS/clip |
| Normal mapping (MikkTSpace) | `render_types.hpp`, `scene_loader.cpp`, `shader.cpp` | `compute_tangents()`, TBN in PBR FS |
| Normal mapping perf hotfix | `shader.cpp` | Removed 2 redundant `normalize()` (4→2 sqrt/px) |
| Path resolution (CWD-independent) | `CMakeLists.txt`, `engine.cpp`, `scene_loader.cpp`, `main.cpp` | `PROJECT_SOURCE_DIR` compile definition |
| Build dir restructure | `build/Debug/`, `build/Release/` | Separate Debug/Release targets |

## Known limitations & bugs

- **`should_clip` is trivial-reject only.** Triangles straddling frustum planes are not
  clipped (no Sutherland-Hodgman). Partially-off-screen triangles pass through unculled.
- **Point lights and spot lights are stub implementations** (`initialize_lights` has
  empty `else if` blocks for POINT and SPOT).
- **No frustum culling** for entire models — all models unconditionally enter the pipeline.
- **No MSAA / anti-aliasing.**
- **No transparency / alpha blending** (though BlinnPhongMaterial has `transparency` field).
- **Nearest-neighbour texture sampling only** — no bilinear, no mipmapping.
- **Incremental edge traversal was rolled back** (see [[bugfixes]] #2) due to floating-point
  accumulation error (~1.5% pixel diff). `TriangleEdgeSetup` struct preserved in
  `geometry.hpp` for future fixed-point implementation.
- **No unit tests.** Correctness is verified by pixel-diff comparison of rendered PPMs.

## Planned phases (from claude/implementation.md)

| Phase | Feature | Expected gain |
|-------|---------|---------------|
| 3 | Depth pre-pass + Hi-Z pyramid | 2-3× (high occlusion) |
| 4 | Proper frustum culling, backface culling ✓ done | 1.3-2× |
| 5 | AVX2/SIMD fragment shader | 2-4× |
| 6 | Mipmapping + 2×2 quad coarse culling | 1.2-1.5× + 1.3-1.5× |

## Performance notes

- **Debug vs Release:** ~6× difference. Debug builds suffer from non-inlined
  `std::function` calls, libc `sqrt` (PLT call vs `sqrtss` instruction), stack-spilled
  Vec3f temporaries, and no loop unrolling. Always benchmark in Release.
- **Hot path:** `_rasterize_fill()` inner loop — `inside_triangle` → `barycentric` →
  `interpolate(Varying)` → `depth_test` → `execute_fragment_shader` → `set_color`.
- **Normal mapping cost:** 2 `sqrt` + 1 extra texture sample per pixel (after hotfix).
  Full recovery to pre-normal-mapping FPS requires SIMD (Phase 5).
- **Tile size 64×64:** chosen so per-tile data (~64 KB) fits L2 cache.
- **Serial binning:** 5 ms / 11K triangles (<5% of frame); parallel binning was tried
  but caused progressive slowdown from per-frame mutex allocation (see [[bugfixes]] #4).
- See [[performance]] for benchmark data and scalability predictions.

## Documentation index

- `claude/bugfixes.md` — 5 historical bug investigations and resolutions
- `claude/implementation.md` — Phase 2 multithreading architecture + Phase 3–6 plans
- `claude/performance.md` — Benchmark data, time breakdown, scalability
- `claude/normal-mapping-and-culling-fixes.md` — 2026-07-02 changeset details

## Coding conventions (observed)

- C++17, no exceptions in hot paths
- STL containers (`std::vector`, `std::shared_ptr`, `std::weak_ptr`) throughout
- Header-only where possible (`tile_binner.h`, `job_system.h`, `fps_camera.h`, `transform.h`)
- `#pragma once` NOT used — classic `#ifndef` include guards
- Template math library (`Vector2<T>`, `Vector3<T>`, `Vector4<T>`)
- `Vec4f` for homogeneous coords, `Vec3f` for directions/colour, `Vec2f` for screen coords
- `RGBA` = `Vector4<float>` typedef
- `Matrix` stores `m_[4][4]` union with `m_array_[16]` for pass-by-pointer to SIMD
- Debug output: `#ifndef NDEBUG` blocks in `engine.cpp` interactive loops for title-bar
  diagnostics (rnd/prs ms, FPS)
