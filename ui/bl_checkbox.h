#pragma once

#include <string>

#include "ui/bl_widget.h"

namespace bl {

class blCheckbox : public blWidget {
 public:
  explicit blCheckbox(std::string label, bool checked = false);

  void Paint(BLContext& ctx) override;
  bool OnEvent(const Event& event) override;
  float PreferredWidth() const override;
  float PreferredHeight() const override;

  void SetChecked(bool checked);
  bool IsChecked() const;
  void SetLabel(const std::string& label);

 private:
  std::string label_;
  bool checked_ = false;
  bool pressed_inside_ = false;
  float preferred_width_ = 200.0F;
  float preferred_height_ = 24.0F;
};

}  // namespace bl