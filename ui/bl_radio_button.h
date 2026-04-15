#pragma once

#include <string>

#include "ui/bl_widget.h"

namespace bl {

class blRadioButton : public blWidget {
 public:
  explicit blRadioButton(std::string label, bool selected = false, float preferred_width = 200.0F, float preferred_height = 24.0F);

  void Paint(BLContext& ctx) override;
  bool OnEvent(const Event& event) override;
  float PreferredWidth() const override;
  float PreferredHeight() const override;

  void SetSelected(bool selected);
  bool IsSelected() const;
  void SetLabel(const std::string& label);
  const std::string& GetLabel() const;

 private:
  std::string label_;
  bool selected_ = false;
  bool pressed_inside_ = false;
  float preferred_width_ = 200.0F;
  float preferred_height_ = 24.0F;
};

}  // namespace bl