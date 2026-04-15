#include "ui/bl_color_animation.h"

#include <algorithm>

namespace bl {

blColorAnimation::blColorAnimation(BLRgba32 startColor, BLRgba32 endColor, float duration)
    : start_color_(startColor), end_color_(endColor), duration_(duration), current_color_(startColor) {
}

void blColorAnimation::Update(float deltaTime) {
  elapsed_time_ += deltaTime;
  elapsed_time_ = std::min(elapsed_time_, duration_);

  float t = elapsed_time_ / duration_;
  t = std::min(t, 1.0f);  // Clamp to 1.0

  // Linear interpolation between colors
  uint32_t r = static_cast<uint32_t>(start_color_.r() + t * (end_color_.r() - start_color_.r()));
  uint32_t g = static_cast<uint32_t>(start_color_.g() + t * (end_color_.g() - start_color_.g()));
  uint32_t b = static_cast<uint32_t>(start_color_.b() + t * (end_color_.b() - start_color_.b()));
  uint32_t a = static_cast<uint32_t>(start_color_.a() + t * (end_color_.a() - start_color_.a()));

  current_color_ = BLRgba32(r, g, b, a);

  // Mark widget dirty if we have one
  if (auto widget = target_widget_.lock()) {
    widget->MarkDirty();
  }
}

bool blColorAnimation::IsFinished() const {
  return elapsed_time_ >= duration_;
}

BLRgba32 blColorAnimation::GetCurrentColor() const {
  return current_color_;
}

}  // namespace bl