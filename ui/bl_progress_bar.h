#pragma once

#include <string>

#include "ui/bl_widget.h"

namespace bl {

class blProgressBar : public blWidget {
 public:
  explicit blProgressBar(float min_value = 0.0F, float max_value = 100.0F, float value = 0.0F, float preferred_width = 200.0F, float preferred_height = 20.0F);

  void Paint(BLContext& ctx) override;
  bool OnEvent(const Event& event) override;
  float PreferredWidth() const override;
  float PreferredHeight() const override;

  void SetValue(float value);
  float GetValue() const;
  void SetRange(float min_value, float max_value);
  void SetShowText(bool show_text);
  void SetText(const std::string& text);

 private:
  float min_value_ = 0.0F;
  float max_value_ = 100.0F;
  float value_ = 0.0F;
  bool show_text_ = true;
  std::string custom_text_;
  float preferred_width_ = 200.0F;
  float preferred_height_ = 20.0F;
};

}  // namespace bl