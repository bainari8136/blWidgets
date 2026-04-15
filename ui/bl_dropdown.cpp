#include "ui/bl_dropdown.h"

#include <iostream>
#include <algorithm>

namespace bl {

blDropdown::blDropdown(std::vector<std::string> options, size_t selected_index, float preferred_width, float preferred_height)
    : options_(std::move(options)), selected_index_(selected_index), preferred_width_(preferred_width), preferred_height_(preferred_height) {
  if (!options_.empty()) {
    selected_index_ = std::min(selected_index_, options_.size() - 1);
  }
  SetBounds(0.0F, 0.0F, preferred_width_, preferred_height_);
}

void blDropdown::Paint(BLContext& ctx) {
  // Draw main button
  BLRoundRect main_rect(X(), Y(), Width(), preferred_height_, 4.0);
  ctx.set_comp_op(BL_COMP_OP_SRC_OVER);
  ctx.set_fill_style(BLRgba32(0xFF, 0xFF, 0xFF, 0xFF));
  ctx.fill_round_rect(main_rect);

  ctx.set_stroke_style(BLRgba32(0xE0, 0xE0, 0xE0, 0xFF));
  ctx.set_stroke_width(1.5);
  ctx.stroke_round_rect(main_rect);

  // Load font for both main text and dropdown options
  BLFontFace face;
  BLFont font;
  bool font_loaded = false;
  if (face.create_from_file("C:/Windows/Fonts/segoeui.ttf") == BL_SUCCESS) {
    if (font.create_from_face(face, 14.0F) == BL_SUCCESS) {
      font_loaded = true;
    }
  }

  // Draw selected text
  if (font_loaded) {
    const double text_x = X() + 8.0;
    const double text_y = Y() + (preferred_height_ * 0.65);

    std::string display_text = options_.empty() ? "Select..." : options_[selected_index_];
    ctx.set_fill_style(BLRgba32(0x21, 0x21, 0x21, 0xFF));
    ctx.fill_utf8_text(BLPoint(text_x, text_y), font, display_text.c_str());
  }

  // Draw arrow
  const float arrow_size = 6.0F;
  const float arrow_x = X() + Width() - 16.0F;
  const float arrow_y = Y() + preferred_height_ / 2.0F;

  ctx.set_fill_style(BLRgba32(0x42, 0x85, 0xF4, 0xFF));
  if (open_) {
    // Up arrow
    BLTriangle arrow(arrow_x, arrow_y + arrow_size/2, arrow_x - arrow_size/2, arrow_y - arrow_size/2, arrow_x + arrow_size/2, arrow_y - arrow_size/2);
    ctx.fill_triangle(arrow);
  } else {
    // Down arrow
    BLTriangle arrow(arrow_x, arrow_y - arrow_size/2, arrow_x - arrow_size/2, arrow_y + arrow_size/2, arrow_x + arrow_size/2, arrow_y + arrow_size/2);
    ctx.fill_triangle(arrow);
  }

  // Draw dropdown list if open
  if (open_ && !options_.empty()) {
    const float list_height = static_cast<float>(options_.size()) * item_height_;
    BLRoundRect list_rect(X(), Y() + preferred_height_, Width(), list_height, 4.0);
    ctx.set_fill_style(BLRgba32(0xFF, 0xFF, 0xFF, 0xFF));
    ctx.fill_round_rect(list_rect);

    ctx.set_stroke_style(BLRgba32(0xE0, 0xE0, 0xE0, 0xFF));
    ctx.set_stroke_width(1.0);
    ctx.stroke_round_rect(list_rect);

    // Draw options
    for (size_t i = 0; i < options_.size(); ++i) {
      const float item_y = Y() + preferred_height_ + static_cast<float>(i) * item_height_;
      const float item_x = X() + 8.0;

      // Highlight on hover (simplified - would need mouse position tracking)
      if (i == selected_index_) {
        BLRect highlight_rect(X() + 1.0F, item_y, Width() - 2.0F, item_height_);
        ctx.set_fill_style(BLRgba32(0xF0, 0xF8, 0xFF, 0xFF));
        ctx.fill_rect(highlight_rect);
      }

      ctx.set_fill_style(BLRgba32(0x21, 0x21, 0x21, 0xFF));
      ctx.fill_utf8_text(BLPoint(item_x, item_y + item_height_ * 0.7), font, options_[i].c_str());
    }
  }
}

bool blDropdown::OnEvent(const Event& event) {
  const bool inside_main = event.x >= X() && event.y >= Y() && event.x <= X() + Width() && event.y <= Y() + preferred_height_;
  bool consumed = false;

  if (open_) {
    // Check if clicking on an option
    for (size_t i = 0; i < options_.size(); ++i) {
      const float item_y = Y() + preferred_height_ + i * item_height_;
      const bool inside_item = event.x >= X() && event.y >= item_y && event.x <= X() + Width() && event.y <= item_y + item_height_;

      if (inside_item) {
        consumed = true;
        switch (event.type) {
          case EventType::MouseDown:
            pressed_inside_ = true;
            break;
          case EventType::MouseUp:
            if (pressed_inside_) {
              SetSelectedIndex(i);
              SetOpen(false);
              std::cout << "blDropdown selected: " << options_[i] << std::endl;
            }
            pressed_inside_ = false;
            break;
          default:
            break;
        }
        return consumed;
      }
    }
  }

  // Handle main button
  switch (event.type) {
    case EventType::MouseDown:
      if (inside_main) {
        pressed_inside_ = true;
        consumed = true;
      } else {
        SetOpen(false);
        // Do not consume if clicking outside to close
      }
      break;
    case EventType::MouseUp:
      if (pressed_inside_ && inside_main) {
        SetOpen(!open_);
        consumed = true;
      }
      pressed_inside_ = false;
      break;
    default:
      break;
  }
  return consumed;
}

void blDropdown::SetOptions(const std::vector<std::string>& options) {
  options_ = options;
  if (!options_.empty()) {
    selected_index_ = std::min(selected_index_, options_.size() - 1);
  } else {
    selected_index_ = 0;
  }
  MarkDirty();
}

void blDropdown::SetSelectedIndex(size_t index) {
  if (index < options_.size() && selected_index_ != index) {
    selected_index_ = index;
    MarkDirty();
  }
}

size_t blDropdown::GetSelectedIndex() const {
  return selected_index_;
}

const std::string& blDropdown::GetSelectedOption() const {
  static const std::string empty_string;
  return options_.empty() ? empty_string : options_[selected_index_];
}

void blDropdown::SetOpen(bool open) {
  if (open_ != open) {
    open_ = open;
    // Adjust height for dropdown list
    if (open_ && !options_.empty()) {
      SetSize(Width(), preferred_height_ + options_.size() * item_height_);
    } else {
      SetSize(Width(), preferred_height_);
    }
    MarkDirty();
  }
}

bool blDropdown::IsOpen() const {
  return open_;
}

float blDropdown::PreferredWidth() const {
  return preferred_width_;
}

float blDropdown::PreferredHeight() const {
  return preferred_height_;
}

}  // namespace bl