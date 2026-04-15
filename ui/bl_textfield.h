#pragma once

#include <string>

#include "ui/bl_widget.h"

namespace bl {

class blTextfield : public blWidget {
 public:
  explicit blTextfield(std::string placeholder = "", float preferred_width = 200.0F, float preferred_height = 32.0F);

  void Paint(BLContext& ctx) override;
  bool OnEvent(const Event& event) override;
  float PreferredWidth() const override;
  float PreferredHeight() const override;

  void SetText(const std::string& text);
  const std::string& GetText() const;
  void SetPlaceholder(const std::string& placeholder);
  void SetFocused(bool focused);
  bool IsFocused() const;

 private:
  std::string text_;
  std::string placeholder_;
  bool focused_ = false;
  bool pressed_inside_ = false;
  size_t cursor_pos_ = 0;
  float preferred_width_ = 200.0F;
  float preferred_height_ = 32.0F;
};

}  // namespace bl