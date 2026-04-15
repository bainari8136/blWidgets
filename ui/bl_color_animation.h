#pragma once

#include "ui/bl_animation.h"

#include <blend2d/blend2d.h>

namespace bl {

class blColorAnimation : public blAnimation {
 public:
  blColorAnimation(BLRgba32 startColor, BLRgba32 endColor, float duration);

  void Update(float deltaTime) override;
  bool IsFinished() const override;

  BLRgba32 GetCurrentColor() const;

 private:
  BLRgba32 start_color_;
  BLRgba32 end_color_;
  float duration_;
  float elapsed_time_ = 0.0f;
  BLRgba32 current_color_;
};

}  // namespace bl