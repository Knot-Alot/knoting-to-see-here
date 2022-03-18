#pragma once
#include <knoting/subsystem.h>
#include <knoting/types.h>
#include <memory>

namespace knot {

class Engine;
class InputManager;

}  // namespace knot

namespace knot {

class CameraRotation : public Subsystem {
   public:
    CameraRotation(Engine& engine);

    void on_awake() override;
    void on_update(double m_delta_time) override;
    void on_late_update() override;
    void on_destroy() override;

   private:
    void camera_key_input();

   private:
    Engine& m_engine;
    std::shared_ptr<InputManager> m_inputManager;

    vec2d m_lastMousePosition = vec2d(0.0);
    vec2d m_mouseSensitivity = vec2d(1.0);
    vec2d m_pitchClamp = vec2(-60, 60);
    vec2d m_currentMousePos = vec2d(0.0);
    vec2d m_mouseDelta = vec2d(0.0);

    vec3 m_keyboardDirection = vec3(0);
    vec3 m_movementMultiplier = vec3(1);
    vec3 m_maxMovementMultiplier = vec3(5);
    vec3 m_minMovementMultiplier = vec3(1);
    vec3 m_moveSpeed = vec3(10);

    float m_pitch = 0;  // pitch = x
    float m_yaw = 0;    // yaw = y
    float m_roll = 0;   // roll = z

    vec3 m_right = vec3(1, 0, 0);
    vec3 m_up = vec3(0, 1, 0);
    vec3 m_forward = vec3(0, 0, 1);

    bool m_ePressed = true;
    bool m_lockState = true;
};

}  // namespace knot