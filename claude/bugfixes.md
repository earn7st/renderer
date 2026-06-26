# Bug 修复记录

## 1. 绕序归一化破坏 Tile 边函数值

**症状**：渲染完全错乱，深度异常，部分三角形黑色。

**根因**：`_rasterize_fill()` 在 clamp 到 Tile 边界后从零重算边函数基值。`setup_triangle_edges()` 对 CW 三角形做了取反归一化，但重算用的常数项 C 来自原始顶点（未取反），与已取反的步进值不一致。误差 = 2×C（~10^6 量级），内/外判定反转。

**修复**：不从零重算，从 `setup_triangle_edges()` 已知正确的基值出发，用已归一化的步进值走到新原点：`f += step_x * dx + step_y * dy`。

---

## 2. 增量边函数遍历导致 ~1.5% 像素差异 + 暗色三角形

**症状**：129 个暗色像素簇（原始 13）。1.5% 通道与原版不同。

**根因**：增量边函数步进（`f += step_x`）在大三角形上累积浮点误差，边函数值在边缘像素偏离正确值 → 内/外判定翻转 → 深度测试变化 → 不同三角形可见。

**修复**：回退到逐像素 `edge_function()` + `inside_triangle()` + `compute_barycentric_coord_2D()`。增量遍历代码保留供未来参考，`TriangleEdgeSetup` 结构体保留在 `geometry.hpp`。

---

## 3. Tile 边界背景色细线

**症状**：并行渲染时 Tile 边界出现灰色细线。

**根因**：`TileBin::max_x` 是 inclusive（`min_x + 63`），但 `_rasterize_fill` 的 AABB `max_x` 是 exclusive（`ceil()` + `x < max`）。`min(exclusive, inclusive)` 比较后边界像素被两侧 Tile 均跳过。

**修复**：统一为 exclusive 上界 — `TileBin::max_x = min(screen_w, min_x + 64)`，`rasterize()` 传 `tile_max = screen_width`。

---

## 4. interactive --parallel dt 递增 / FPS < 1

**症状**：`--interactive --parallel` 启动后 FPS < 1 且 dt 逐帧增大。

**根因**：`render_parallel()` 每帧 3 次 `parallel_for`（barrier ×3）+ 分配 375 个 `std::mutex`。交互循环中 barrier 同步 + mutex 内核对象分配/销毁累积，造成进行性延迟。

**修复**：分箱从并行还原为串行（~5ms，占比 <5%），消除 1 次 barrier 和全部 mutex 分配。

---

## 5. 单线程 --interactive 无诊断输出

**症状**：在窗口标题加诊断后，Release 构建速度正常但 Debug 构建开始时 FPS 低。

**根因/修复**：`Display::present()` 每帧 23MB 帧缓冲拷贝（`const auto pixels = fb.get_color_data()` 值捕获而非引用），在 debug 构建下分配开销更明显。改为 `const auto& pixels`。Release 构建（`-DCMAKE_BUILD_TYPE=Release`）通过 `#ifdef NDEBUG` 剥离诊断代码。
