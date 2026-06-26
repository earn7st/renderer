# 实现方案

## Phase 2: 多线程渲染

### 架构

```
render_parallel():
  Step 1 (串行)  — collect_draw_batches()  收集 DrawBatch，~1ms
  Step 2 (并行)  — 顶点处理 (chunked, 64 tri/job)，barrier 同步启动
  Step 3 (串行)  — Tile 分箱，AABB → Tile 索引，~5ms/11K tri
  Step 4 (并行)  — Tile 光栅化，每个 Tile 独占像素区域
```

### 核心数据结构

**`ProcessedTriangle`** (~332B)：顶点着色器输出，存于共享数组。用 `batch_idx` (4B) 查材质数据，避免 600B 的 `ShaderConstants` 值拷贝。

**`TileBin`**：每 Tile 持有 `triangle_indices` (vector of uint32)，指向 `processed[]`。边界用 **exclusive 上界**（`[min, max)`），与 AABB `ceil()` + `x < max` 一致，消除 Tile 间缝隙。

**`JobSystem`**：条件变量 + 批次计数器唤醒 worker；`parallel_for` 内所有线程（含 main）在 **start barrier** 集结后同时认领任务（`atomic<int>` 无锁）。

### 关键设计决策

| 决策 | 理由 |
|------|------|
| Tile 64×64 | 数据 ~64KB，适合 L2；375 tiles (1600×900) 充分并行 |
| 逐像素边函数（非增量） | 增量遍历浮点累积误差导致 1.5% 像素差异，不可接受 |
| 分箱串行 | 仅占 5% 帧时间，避免 per-frame mutex 分配/barrier 开销 |
| exclusive Tile 上界 | 消除 Tile 边界像素丢失 |
| start barrier | 消除主线程抢先，所有线程同时开始 |

### 线程安全

- 帧缓冲写入：每 Tile 独占像素区域 → 零同步
- `processed[]`：按 job 索引写入不同区域 → 零同步
- Tile 分箱：串行 → 零同步
- Job 认领：`atomic<int>::fetch_add` → 无锁

### 变更文件

| 文件 | 内容 |
|------|------|
| `src/renderer/tile_binner.h` | **新** — ProcessedTriangle, TileBin, TileBinner |
| `src/renderer/job_system.h` | **新** — 线程池 + start barrier |
| `src/renderer/rasterizer.h/cpp` | `_rasterize_fill()` — 逐像素边函数 + Tile 限定光栅化 |
| `src/renderer/renderer.h/cpp` | `render_parallel()`, `collect_draw_batches()`, `rasterize_tile_job()` |
| `src/renderer/engine.h/cpp` | `run_interactive_parallel()` |
| `src/main.cpp` | `--parallel`, `--interactive` CLI |

---

## Phase 3: Depth Pre-pass + Hi-Z（计划）

光栅化前先跑 depth-only pass，再跑颜色 pass（深度测试 `EQUAL`）。Hi-Z 金字塔逐级降采样深度缓冲，Tile 光栅化前查询以快速跳过被遮挡 Tile。

---

## Phase 4: Culling 修复（计划）

修复 `should_clip()` 逻辑错误（`&&` → `||`），添加 backface culling，减少无用三角形进入光栅化。

---

## Phase 5: AVX2 SIMD 片段着色（计划）

PBR 逐像素标量计算 → AVX2 8 像素/指令，与 Tile 64×64 对齐。

---

## Phase 6: Mipmapping + 2×2 Quad 剔除（计划）

预生成纹理 mipmap 链改善缓存命中。`_rasterize_fill` 内 2×2 块粗检测：全外 → 跳过，全内 → 免逐像素边测试。
