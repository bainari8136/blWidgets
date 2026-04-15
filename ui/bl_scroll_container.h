#pragma once

#include "ui/bl_widget.h"

namespace bl {

class blScrollContainer : public blWidget {
 public:
  explicit blScrollContainer(float preferred_width = 300.0F, float preferred_height = 200.0F);

  void Paint(BLContext& ctx) override;
  bool OnEvent(const Event& event) override;
  float PreferredWidth() const override;
  float PreferredHeight() const override;

  void SetScrollPosition(float x, float y);
  float GetScrollX() const;
  float GetScrollY() const;
  void SetContentSize(float width, float height);
  float GetContentWidth() const;
  float GetContentHeight() const;

 private:
  float scroll_x_ = 0.0F;
  float scroll_y_ = 0.0F;
  float content_width_ = 0.0F;
  float content_height_ = 0.0F;
  bool dragging_scrollbar_ = false;
  float scrollbar_width_ = 16.0F;
  float preferred_width_ = 300.0F;
  float preferred_height_ = 200.0F;
};

}  // namespace bl