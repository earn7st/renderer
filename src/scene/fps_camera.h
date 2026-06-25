#ifndef __FPS_CAMERA_H__
#define __FPS_CAMERA_H__

#include "math/math_all.h"
#include "scene/camera.h"

// ============================================================
// Free-moving FPS-style camera.
//
// Generates a view matrix from position + yaw/pitch angles.
// Movement is driven by an external input state each frame.
// Mouse-look accumulates yaw/pitch deltas; WASD/QE translate
// relative to the current view direction.
//
// Usage:
//   FpsCamera cam(start_pos, start_yaw, start_pitch);
//   cam.set_perspective(fovy, aspect, near, far);
//   while (running) {
//       InputState input = poll_input();
//       cam.update(input, delta_time);
//       Uniform u = compute_uniforms(cam);
//       render(scene, u);
//   }
// ============================================================

struct InputState
{
    bool forward  = false;   // W
    bool backward = false;   // S
    bool left     = false;   // A
    bool right    = false;   // D
    bool up       = false;   // E
    bool down     = false;   // Q
    float mouse_dx = 0.0f;   // accumulated mouse delta X this frame
    float mouse_dy = 0.0f;   // accumulated mouse delta Y this frame
};

class FpsCamera
{
public:
    FpsCamera(const Vec3f& position = Vec3f(0.0f, 0.0f, -5.0f),
              float yaw = -90.0f,
              float pitch = 0.0f);

    // --- configuration ---
    void set_move_speed(float v)    { move_speed_ = v; }
    void set_mouse_sensitivity(float s) { mouse_sens_ = s; }
    void set_perspective(float fovy, float aspect, float near_plane, float far_plane);

    // --- per-frame update ---
    void update(const InputState& input, float delta_time);

    // --- query ---
    Vec3f position()    const { return pos_; }
    Vec3f forward()     const { return front_; }
    Vec3f up()          const { return up_; }
    float yaw()         const { return yaw_; }
    float pitch()       const { return pitch_; }

    // Getters matching the original Camera interface
    Vec3f get_pos()    const { return pos_; }
    Vec3f get_center() const { return pos_ + front_; }
    Vec3f get_up()     const { return up_; }
    float get_fovy()        const { return fovy_; }
    float get_aspect()      const { return aspect_; }
    float get_near_plane()  const { return near_plane_; }
    float get_far_plane()   const { return far_plane_; }

    // Initialise from a scene Camera (preserves fov/aspect/near/far)
    void init_from_camera(const Camera& cam);

private:
    void update_vectors();

    Vec3f pos_;
    Vec3f front_;
    Vec3f up_;
    Vec3f right_;
    Vec3f world_up_;

    float yaw_;      // degrees, 0 = +Z, -90 = +X (OpenGL convention)
    float pitch_;    // degrees, clamped to [-89, 89]

    float move_speed_ = 5.0f;
    float mouse_sens_ = 0.1f;

    float fovy_        = 60.0f;
    float aspect_      = 1.333f;
    float near_plane_  = 0.1f;
    float far_plane_   = 100.0f;
};

// --- Implementation ---

inline FpsCamera::FpsCamera(const Vec3f& position, float yaw, float pitch)
    : pos_(position), world_up_(Vec3f(0.0f, 1.0f, 0.0f)),
      yaw_(yaw), pitch_(pitch)
{
    update_vectors();
}

inline void FpsCamera::set_perspective(float fovy, float aspect, float near_plane, float far_plane)
{
    fovy_       = fovy;
    aspect_     = aspect;
    near_plane_ = near_plane;
    far_plane_  = far_plane;
}

inline void FpsCamera::init_from_camera(const Camera& cam)
{
    pos_   = cam.get_pos();
    // Derive yaw/pitch from the look-at direction
    Vec3f dir = normalize(cam.get_center() - cam.get_pos());
    pitch_ = std::asin(dir.y_) * 180.0f / (float)PI;
    yaw_   = std::atan2(-dir.z_, dir.x_) * 180.0f / (float)PI;
    update_vectors();
    fovy_       = cam.get_fovy();
    aspect_     = cam.get_aspect();
    near_plane_ = cam.get_near_plane();
    far_plane_  = cam.get_far_plane();
}

inline void FpsCamera::update_vectors()
{
    float yaw_rad   = yaw_   * (float)PI / 180.0f;
    float pitch_rad = pitch_ * (float)PI / 180.0f;

    front_.x_ = std::cos(yaw_rad) * std::cos(pitch_rad);
    front_.y_ = std::sin(pitch_rad);
    front_.z_ = -std::sin(yaw_rad) * std::cos(pitch_rad);
    front_    = normalize(front_);

    right_ = normalize(cross(front_, world_up_));
    up_    = normalize(cross(right_, front_));
}

inline void FpsCamera::update(const InputState& input, float dt)
{
    // Clamp dt to avoid huge jumps on first frame
    if (dt > 0.1f) dt = 0.016f;

    float velocity = move_speed_ * dt;

    if (input.forward)  pos_ = pos_ + front_ * velocity;
    if (input.backward) pos_ = pos_ - front_ * velocity;
    if (input.left)     pos_ = pos_ - right_ * velocity;
    if (input.right)    pos_ = pos_ + right_ * velocity;
    if (input.up)       pos_ = pos_ + world_up_ * velocity;
    if (input.down)     pos_ = pos_ - world_up_ * velocity;

    // Mouse look
    yaw_   += input.mouse_dx * mouse_sens_;
    pitch_ += input.mouse_dy * mouse_sens_;

    if (pitch_ >  89.0f) pitch_ =  89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;

    update_vectors();
}

#endif // __FPS_CAMERA_H__
