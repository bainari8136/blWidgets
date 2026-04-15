#pragma once

#include <string>

#include "ui/bl_widget.h"

namespace bl {

class blLabel : public blWidget {
 public:
  explicit blLabel(std::string text);

  void Paint(BLContext& ctx) override;
  bool OnEvent(const Event& event) override;
  float PreferredWidth() const override;
  float PreferredHeight() const override;
  const char* TypeName() const override { return "label"; }

  void SetText(const std::string& text);
  const std::string& GetText() const;

 private:
  std::string text_;
  float preferred_width_ = 200.0F;
  float preferred_height_ = 24.0F;
};

}  // namespace bl