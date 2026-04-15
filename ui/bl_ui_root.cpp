#include "ui/bl_ui_root.h"

namespace bl {

void blUiRoot::SetStyleSheet(const std::string& css_text) {
  has_stylesheet_ = style_sheet_.LoadFromString(css_text);
  if (!has_stylesheet_) {
    return;
  }
  ApplyStylesRecursive(root_widget_);
  needs_layout_ = true;
  root_widget_.MarkDirty();
}

void blUiRoot::SetRootSizer(const std::shared_ptr<blSizer>& sizer) {
  root_sizer_ = sizer;
  root_widget_.SetChildren(root_sizer_ ? root_sizer_->GetChildren() : std::vector<std::shared_ptr<blWidget>>{});
  if (has_stylesheet_) {
    ApplyStylesRecursive(root_widget_);
  }
  needs_layout_ = true;
  root_widget_.MarkDirty();
}

void blUiRoot::SetSize(float width, float height) {
  root_widget_.SetPosition(0.0F, 0.0F);
  root_widget_.SetSize(width, height);
  needs_layout_ = true;
}

void blUiRoot::Update(float deltaTime) {
  root_widget_.Update(deltaTime);
}

void blUiRoot::PerformLayoutIfNeeded() {
  if (!needs_layout_ && !root_widget_.HasLayoutConstraintChanged()) {
    return;
  }
  if (root_sizer_) {
    root_sizer_->Layout(&root_widget_);
  }
  root_widget_.ClearLayoutConstraintChanged();
  needs_layout_ = false;
}

void blUiRoot::ApplyStylesRecursive(blWidget& widget) {
  if (has_stylesheet_) {
    const blStyle merged_style = style_sheet_.ComputeStyle(widget, widget.GetStyle());
    widget.SetStyle(merged_style);
  }
  for (const auto& child : widget.Children()) {
    if (child) {
      ApplyStylesRecursive(*child);
    }
  }
}

void blUiRoot::Paint(BLContext& ctx) {
  PerformLayoutIfNeeded();
  if (!root_sizer_ || !root_widget_.IsDirty()) {
    return;
  }
  root_widget_.Paint(ctx);
  root_widget_.ClearDirtyRecursive();
}

bool blUiRoot::IsDirty() const {
  return root_widget_.IsDirty() || needs_layout_;
}

std::vector<BLRect> blUiRoot::DirtyRegions() const {
  if (!root_sizer_) {
    return {};
  }

  if (needs_layout_ || root_widget_.IsSelfDirty()) {
    return {BLRect(root_widget_.X(), root_widget_.Y(), root_widget_.Width(), root_widget_.Height())};
  }

  std::vector<BLRect> regions;
  const std::vector<std::shared_ptr<blWidget>>& widgets = root_widget_.Children();
  regions.reserve(widgets.size());
  for (const auto& widget : widgets) {
    if (widget && widget->IsDirty()) {
      regions.emplace_back(widget->X(), widget->Y(), widget->Width(), widget->Height());
    }
  }

  return regions;
}

void blUiRoot::DispatchEvent(const Event& event) {
  root_widget_.OnEvent(event);
}

}  // namespace bl
