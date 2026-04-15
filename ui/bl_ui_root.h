#pragma once

#include <memory>
#include <vector>

#include "ui/bl_event.h"
#include "ui/bl_sizer.h"
#include "ui/bl_stylesheet.h"
#include "ui/bl_widget.h"

namespace bl {

class blUiRoot {
 public:
  void SetRootSizer(const std::shared_ptr<blSizer>& sizer);
  void SetSize(float width, float height);
  void Paint(BLContext& ctx);
  void Update(float deltaTime);
  bool IsDirty() const;
  std::vector<BLRect> DirtyRegions() const;
  void DispatchEvent(const Event& event);
  void SetStyleSheet(const std::string& css_text);
  void InvalidateLayout() { needs_layout_ = true; }

 private:
  class RootWidget : public blWidget {
   public:
    void Paint(BLContext& ctx) override { blWidget::Paint(ctx); }
  };

  void PerformLayoutIfNeeded();
  void ApplyStylesRecursive(blWidget& widget);

  RootWidget root_widget_;
  std::shared_ptr<blSizer> root_sizer_;
  blStyleSheet style_sheet_;
  bool has_stylesheet_ = false;
  bool needs_layout_ = true;
};

}  // namespace bl
