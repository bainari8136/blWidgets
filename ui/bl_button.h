#pragma once

#include <string>

#include <blend2d/blend2d.h>

#include "ui/bl_widget.h"

namespace bl {

class blButton : public blWidget {
 public:
  blButton(std::string label, float preferred_width = 220.0F, float preferred_height = 72.0F);

  void Paint(BLContext& ctx) override;
  bool OnEvent(const Event& event) override;
  float PreferredWidth() const override;
  float PreferredHeight() const override;
  void SetStyle(const blStyle& style) override;
  const char* TypeName() const override { return "button"; }

  void SetCurrentBackgroundColor(BLRgba32 color);

  void Update(float deltaTime);

 private:
  enum class State {
    Normal,
    Hovered,
    Pressed
  };

  State state_ = State::Normal;
  bool pressed_inside_ = false;
  std::string label_;
  float preferred_width_ = 220.0F;
  float preferred_height_ = 72.0F;
  BLRgba32 current_background_color_;
  BLRgba32 target_background_color_;
  float animation_progress_ = 1.0f;  // 1.0 means at target
  float animation_speed_ = 5.0f;     // units per second
};

}  // namespace bl
