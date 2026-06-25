#ifndef __TILE_BINNER_H__
#define __TILE_BINNER_H__

#include <vector>
#include <cstdint>

#include "math/geometry.hpp"
#include "renderer/render_types.hpp"
#include "renderer/shader.h"
#include "renderer/shader_constants.h"

// ============================================================
// Tile-based screen-space binning for parallel rasterization.
//
// The screen is divided into fixed-size tiles (TILE_SIZE × TILE_SIZE pixels).
// Each tile owns its pixel region exclusively — zero synchronization on
// framebuffer writes. Triangles are binned into tiles they overlap via AABB.
// ============================================================

constexpr int TILE_SIZE = 64;

// --- Post-vertex-shader triangle, ready for rasterization ---
struct ProcessedTriangle
{
    Vec2f  screen_pos[3];          // viewport-transformed, in pixels
    Vec4f  ndc_pos[3];             // after perspective division (for depth)
    Varying varyings[3];           // world_pos, normal, texcoord
    float  clip_w[3];              // w-buffer reciprocal for perspective correction
    const Shader*  shader = nullptr;
    uint32_t        batch_idx = 0;       // index into DrawBatch array for constants lookup
};

// --- One screen-space tile ---
struct TileBin
{
    int min_x, min_y;              // pixel bounds (inclusive)
    int max_x, max_y;
    std::vector<uint32_t> triangle_indices;
};

// --- Binning context for a frame (or model batch) ---
class TileBinner
{
public:
    TileBinner(uint32_t screen_w, uint32_t screen_h);

    // Reset for a new batch of triangles
    void begin_batch();

    // Store a processed triangle and bin it into overlapping tiles.
    // Returns the triangle index within this batch.
    uint32_t push_triangle(ProcessedTriangle&& tri);

    // Number of tiles across / down
    int tiles_x() const { return tiles_x_; }
    int tiles_y() const { return tiles_y_; }
    int tile_count() const { return tiles_x_ * tiles_y_; }

    const TileBin& tile(int tx, int ty) const { return tiles_[ty * tiles_x_ + tx]; }
    TileBin&       tile(int tx, int ty)       { return tiles_[ty * tiles_x_ + tx]; }

    const ProcessedTriangle& triangle(uint32_t idx) const { return triangles_[idx]; }

    void set_triangles(std::vector<ProcessedTriangle>&& tris) { triangles_ = std::move(tris); }

    const std::vector<ProcessedTriangle>& triangles() const { return triangles_; }

private:
    uint32_t screen_w_, screen_h_;
    int      tiles_x_, tiles_y_;
    std::vector<TileBin> tiles_;
    std::vector<ProcessedTriangle> triangles_;
};

// --- Implementation ---

inline TileBinner::TileBinner(uint32_t screen_w, uint32_t screen_h)
    : screen_w_(screen_w), screen_h_(screen_h)
{
    tiles_x_ = ((int)screen_w + TILE_SIZE - 1) / TILE_SIZE;
    tiles_y_ = ((int)screen_h + TILE_SIZE - 1) / TILE_SIZE;

    tiles_.reserve(tiles_x_ * tiles_y_);
    for (int ty = 0; ty < tiles_y_; ++ty)
    {
        for (int tx = 0; tx < tiles_x_; ++tx)
        {
            TileBin tb;
            tb.min_x = tx * TILE_SIZE;
            tb.min_y = ty * TILE_SIZE;
            // max_x / max_y are EXCLUSIVE (one past the last pixel),
            // matching the AABB semantics in _rasterize_fill (ceil + x < max).
            tb.max_x = std::min((int)screen_w, tb.min_x + TILE_SIZE);
            tb.max_y = std::min((int)screen_h, tb.min_y + TILE_SIZE);
            tiles_.push_back(std::move(tb));
        }
    }
}

inline void TileBinner::begin_batch()
{
    triangles_.clear();
    for (auto& t : tiles_) t.triangle_indices.clear();
}

inline uint32_t TileBinner::push_triangle(ProcessedTriangle&& tri)
{
    uint32_t idx = (uint32_t)triangles_.size();
    triangles_.push_back(std::move(tri));
    const auto& t = triangles_.back();

    // --- compute screen-space AABB ---
    float min_x = std::min({t.screen_pos[0].x_, t.screen_pos[1].x_, t.screen_pos[2].x_});
    float min_y = std::min({t.screen_pos[0].y_, t.screen_pos[1].y_, t.screen_pos[2].y_});
    float max_x = std::max({t.screen_pos[0].x_, t.screen_pos[1].x_, t.screen_pos[2].x_});
    float max_y = std::max({t.screen_pos[0].y_, t.screen_pos[1].y_, t.screen_pos[2].y_});

    // --- clamp to framebuffer ---
    int tx0 = std::max(0, (int)std::floor(min_x) / TILE_SIZE);
    int ty0 = std::max(0, (int)std::floor(min_y) / TILE_SIZE);
    int tx1 = std::min(tiles_x_ - 1, (int)std::ceil(max_x) / TILE_SIZE);
    int ty1 = std::min(tiles_y_ - 1, (int)std::ceil(max_y) / TILE_SIZE);

    // --- add triangle index to every overlapping tile ---
    for (int ty = ty0; ty <= ty1; ++ty)
        for (int tx = tx0; tx <= tx1; ++tx)
            tiles_[ty * tiles_x_ + tx].triangle_indices.push_back(idx);

    return idx;
}

#endif // __TILE_BINNER_H__
