#pragma once

#include <string>

#include "ui/bl_widget.h"

namespace bl {

class blSlider : public blWidget {
 public:
  explicit blSlider(std::string label = "", float min_value = 0.0F, float max_value = 100.0F, float value = 50.0F);

  void Paint(BLContext& ctx) override;
  bool OnEvent(const Event& event) override;
  float PreferredWidth() const override;
  float PreferredHeight() const override;

  void SetValue(float value);
  float GetValue() const;
  void SetRange(float min_value, float max_value);
  void SetLabel(const std::string& label);

 private:
  std::string label_;
  float min_value_;
  float max_value_;
  float value_;
  bool dragging_ = false;
  float preferred_width_ = 200.0F;
  float preferred_height_ = 40.0F;
};

}  // namespace bl