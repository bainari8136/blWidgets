#include "ui/bl_scroll_container.h"

#include <algorithm>

namespace bl {

blScrollContainer::blScrollContainer(float preferred_width, float preferred_height)
    : preferred_width_(preferred_width), preferred_height_(preferred_height) {
  SetBounds(0.0F, 0.0F, preferred_width_, preferred_height_);
}

void blScrollContainer::Paint(BLContext& ctx) {
  // Draw background
  BLRect rect(X(), Y(), Width(), Height());
  ctx.set_comp_op(BL_COMP_OP_SRC_OVER);
  ctx.set_fill_style(BLRgba32(0xF8, 0xF9, 0xFA, 0xFF));
  ctx.fill_rect(rect);

  ctx.set_stroke_style(BLRgba32(0xE0, 0xE0, 0xE0, 0xFF));
  ctx.set_stroke_width(1.0);
  ctx.stroke_rect(rect);

  // Save context and clip to content area
  ctx.save();
  BLRect content_rect(X() + 1.0F, Y() + 1.0F, Width() - scrollbar_width_ - 2.0F, Height() - 2.0F);
  ctx.clip_to_rect(content_rect);

  // Translate for scrolling
  ctx.translate(-scroll_x_, -scroll_y_);

  // Paint children with scroll offset
  for (const auto& child : Children()) {
    if (child) {
      child->Paint(ctx);
    }
  }

  ctx.restore();

  // Draw scrollbars if needed
  if (content_height_ > Height()) {
    // Vertical scrollbar
    const float scrollbar_height = Height() - 2.0F;
    const float thumb_height = std::max(20.0F, scrollbar_height * (Height() / content_height_));
    const float thumb_y = Y() + 1.0F + (scroll_y_ / content_height_) * (scrollbar_height - thumb_height);

    BLRect scrollbar_track(X() + Width() - scrollbar_width_, Y() + 1.0F, scrollbar_width_ - 1.0F, scrollbar_height);
    ctx.set_fill_style(BLRgba32(0xF0, 0xF0, 0xF0, 0xFF));
    ctx.fill_rect(scrollbar_track);

    BLRect scrollbar_thumb(X() + Width() - scrollbar_width_ + 2.0F, thumb_y, scrollbar_width_ - 4.0F, thumb_height);
    ctx.set_fill_style(BLRgba32(0xC0, 0xC0, 0xC0, 0xFF));
    ctx.fill_round_rect(scrollbar_thumb, 2.0);
  }

  if (content_width_ > Width() - scrollbar_width_) {
    // Horizontal scrollbar
    const float scrollbar_width = Width() - scrollbar_width_ - 2.0F;
    const float thumb_width = std::max(20.0F, scrollbar_width * ((Width() - scrollbar_width_) / content_width_));
    const float thumb_x = X() + 1.0F + (scroll_x_ / content_width_) * (scrollbar_width - thumb_width);

    BLRect scrollbar_track(X() + 1.0F, Y() + Height() - scrollbar_width_, scrollbar_width, scrollbar_width_ - 1.0F);
    ctx.set_fill_style(BLRgba32(0xF0, 0xF0, 0xF0, 0xFF));
    ctx.fill_rect(scrollbar_track);

    BLRect scrollbar_thumb(thumb_x, Y() + Height() - scrollbar_width_ + 2.0F, thumb_width, scrollbar_width_ - 4.0F);
    ctx.set_fill_style(BLRgba32(0xC0, 0xC0, 0xC0, 0xFF));
    ctx.fill_round_rect(scrollbar_thumb, 2.0);
  }
}

bool blScrollContainer::OnEvent(const Event& event) {
  bool consumed = false;
  // Handle scrollbar interaction first
  if (content_height_ > Height()) {
    const float scrollbar_x = X() + Width() - scrollbar_width_;
    const bool in_vertical_scrollbar = event.x >= scrollbar_x && event.x <= X() + Width() &&
                                     event.y >= Y() && event.y <= Y() + Height();

    if (in_vertical_scrollbar) {
      switch (event.type) {
        case EventType::MouseDown:
          dragging_scrollbar_ = true;
          // Calculate scroll position from click
          {
            const float scrollbar_height = Height() - 2.0F;
            const float thumb_height = std::max(20.0F, scrollbar_height * (Height() / content_height_));
            const float click_ratio = (event.y - Y() - 1.0F) / (scrollbar_height - thumb_height);
            scroll_y_ = click_ratio * (content_height_ - Height());
            scroll_y_ = std::max(0.0F, std::min(scroll_y_, content_height_ - Height()));
            MarkDirty();
          }
          return true;
        case EventType::MouseUp:
          if (dragging_scrollbar_) {
            dragging_scrollbar_ = false;
            return true;
          }
          break;
        case EventType::MouseMove:
          if (dragging_scrollbar_) {
            const float scrollbar_height = Height() - 2.0F;
            const float thumb_height = std::max(20.0F, scrollbar_height * (Height() / content_height_));
            const float click_ratio = (event.y - Y() - 1.0F) / (scrollbar_height - thumb_height);
            scroll_y_ = click_ratio * (content_height_ - Height());
            scroll_y_ = std::max(0.0F, std::min(scroll_y_, content_height_ - Height()));
            MarkDirty();
          }
          break;
        default:
          break;
      }
    } else if (dragging_scrollbar_) {
      // Continue dragging even if outside scrollbar
      if (event.type == EventType::MouseMove) {
        const float scrollbar_height = Height() - 2.0F;
        const float thumb_height = std::max(20.0F, scrollbar_height * (Height() / content_height_));
        const float click_ratio = (event.y - Y() - 1.0F) / (scrollbar_height - thumb_height);
        scroll_y_ = click_ratio * (content_height_ - Height());
        scroll_y_ = std::max(0.0F, std::min(scroll_y_, content_height_ - Height()));
        MarkDirty();
      } else if (event.type == EventType::MouseUp) {
        dragging_scrollbar_ = false;
        return true;
      }
    }
  }

  // Adjust event coordinates for scrolling and pass to children
  Event adjusted_event = event;
  adjusted_event.x += scroll_x_;
  adjusted_event.y += scroll_y_;

  // Pass event to children
  const auto& children_list = Children();
  for (auto it = children_list.rbegin(); it != children_list.rend(); ++it) {
    if (*it) {
      if ((*it)->OnEvent(adjusted_event)) {
        consumed = true;
        if (event.type == EventType::MouseDown || event.type == EventType::MouseUp) {
          return true;
        }
      }
    }
  }
  return consumed;
}

void blScrollContainer::SetScrollPosition(float x, float y) {
  scroll_x_ = std::max(0.0F, std::min(x, std::max(0.0F, content_width_ - (Width() - scrollbar_width_))));
  scroll_y_ = std::max(0.0F, std::min(y, std::max(0.0F, content_height_ - Height())));
  MarkDirty();
}

float blScrollContainer::GetScrollX() const {
  return scroll_x_;
}

float blScrollContainer::GetScrollY() const {
  return scroll_y_;
}

void blScrollContainer::SetContentSize(float width, float height) {
  content_width_ = width;
  content_height_ = height;
  // Adjust scroll position if content became smaller
  scroll_x_ = std::min(scroll_x_, std::max(0.0F, content_width_ - (Width() - scrollbar_width_)));
  scroll_y_ = std::min(scroll_y_, std::max(0.0F, content_height_ - Height()));
  MarkDirty();
}

float blScrollContainer::GetContentWidth() const {
  return content_width_;
}

float blScrollContainer::GetContentHeight() const {
  return content_height_;
}

float blScrollContainer::PreferredWidth() const {
  return preferred_width_;
}

float blScrollContainer::PreferredHeight() const {
  return preferred_height_;
}

}  // namespace bl