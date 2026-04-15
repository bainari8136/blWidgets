#pragma once

#include <memory>

#include "ui/bl_sizer.h"
#include "ui/bl_widget.h"

namespace bl {

class blPanel : public blWidget {
 public:
  blPanel();

  void Paint(BLContext& ctx) override;
  bool OnEvent(const Event& event) override;
  float PreferredWidth() const override;
  float PreferredHeight() const override;

  void SetBorderWidth(float width);
  void SetCornerRadius(float radius);
  void SetLayoutSizer(const std::shared_ptr<blSizer>& sizer);

 private:
  void PerformLayoutIfNeeded();

  float border_width_ = 1.0F;
  float corner_radius_ = 8.0F;
  float preferred_width_ = 200.0F;
  float preferred_height_ = 100.0F;
  std::shared_ptr<blSizer> layout_sizer_;
  bool needs_layout_ = false;
};

}  // namespace bl