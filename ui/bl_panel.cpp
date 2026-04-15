#include "ui/bl_panel.h"

namespace bl {

blPanel::blPanel() {
  SetBounds(0.0F, 0.0F, preferred_width_, preferred_height_);
}

void blPanel::PerformLayoutIfNeeded() {
  if (!needs_layout_ && !HasLayoutConstraintChanged()) {
    return;
  }
  if (layout_sizer_) {
    layout_sizer_->Layout(this);
  }
  ClearLayoutConstraintChanged();
  needs_layout_ = false;
}

void blPanel::Paint(BLContext& ctx) {
  PerformLayoutIfNeeded();
  
  BLRoundRect panel_rect(X(), Y(), Width(), Height(), corner_radius_);
  ctx.set_comp_op(BL_COMP_OP_SRC_OVER);
  ctx.set_fill_style(GetStyle().backgroundColor);
  ctx.fill_round_rect(panel_rect);

  if (border_width_ > 0.0F) {
    ctx.set_stroke_style(BLRgba32(0x1F, 0x2A, 0x38, 0xFF));
    ctx.set_stroke_width(border_width_);
    ctx.stroke_round_rect(panel_rect);
  }

  // Paint children
  blWidget::Paint(ctx);
}

bool blPanel::OnEvent(const Event& event) {
  // Panels pass events to children
  return blWidget::OnEvent(event);
}

float blPanel::PreferredWidth() const {
  return preferred_width_;
}

float blPanel::PreferredHeight() const {
  return preferred_height_;
}

void blPanel::SetBorderWidth(float width) {
  if (border_width_ != width) {
    border_width_ = width;
    MarkDirty();
  }
}

void blPanel::SetCornerRadius(float radius) {
  if (corner_radius_ != radius) {
    corner_radius_ = radius;
    MarkDirty();
  }
}

void blPanel::SetLayoutSizer(const std::shared_ptr<blSizer>& sizer) {
  layout_sizer_ = sizer;
  if (layout_sizer_) {
    SetChildren(layout_sizer_->GetChildren());
  }
  needs_layout_ = true;
  MarkDirty();
}

}  // namespace bl