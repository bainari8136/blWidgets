#include "ui/bl_textfield.h"

#include <iostream>
#include <algorithm>

namespace bl {

blTextfield::blTextfield(std::string placeholder, float preferred_width, float preferred_height)
    : placeholder_(std::move(placeholder)), preferred_width_(preferred_width), preferred_height_(preferred_height) {
  SetBounds(0.0F, 0.0F, preferred_width_, preferred_height_);
}

void blTextfield::Paint(BLContext& ctx) {
  // Draw background
  BLRoundRect rect(X(), Y(), Width(), Height(), 4.0);
  ctx.set_comp_op(BL_COMP_OP_SRC_OVER);

  BLRgba32 bg_color = focused_ ? BLRgba32(0xFF, 0xFF, 0xFF, 0xFF) : BLRgba32(0xF8, 0xF9, 0xFA, 0xFF);
  ctx.set_fill_style(bg_color);
  ctx.fill_round_rect(rect);

  // Draw border
  BLRgba32 border_color = focused_ ? BLRgba32(0x42, 0x85, 0xF4, 0xFF) : BLRgba32(0xE0, 0xE0, 0xE0, 0xFF);
  ctx.set_stroke_style(border_color);
  ctx.set_stroke_width(1.5);
  ctx.stroke_round_rect(rect);

  // Draw text
  BLFontFace face;
  if (face.create_from_file("C:/Windows/Fonts/segoeui.ttf") == BL_SUCCESS) {
    BLFont font;
    if (font.create_from_face(face, 14.0F) == BL_SUCCESS) {
      const double text_x = X() + 8.0;
      const double text_y = Y() + (Height() * 0.65);

      if (text_.empty() && !focused_) {
        // Draw placeholder
        ctx.set_fill_style(BLRgba32(0x9E, 0x9E, 0x9E, 0xFF));
        ctx.fill_utf8_text(BLPoint(text_x, text_y), font, placeholder_.c_str());
      } else {
        // Draw text
        ctx.set_fill_style(BLRgba32(0x21, 0x21, 0x21, 0xFF));
        ctx.fill_utf8_text(BLPoint(text_x, text_y), font, text_.c_str());
      }

      // Draw cursor if focused
      if (focused_) {
        // Simple cursor positioning using approximate character width
        // TODO: Use proper text shaping for accurate cursor positioning
        const double approx_char_width = 8.0; // Approximate width for 14pt font
        double cursor_x = text_x + (cursor_pos_ * approx_char_width);
        double cursor_y = Y() + 4.0;
        ctx.set_stroke_style(BLRgba32(0x42, 0x85, 0xF4, 0xFF));
        ctx.set_stroke_width(1.0);
        ctx.stroke_line(cursor_x, cursor_y, cursor_x, cursor_y + Height() - 8.0);
      }
    }
  }
}

bool blTextfield::OnEvent(const Event& event) {
  const bool inside = HitTest(event.x, event.y);
  bool consumed = false;

  switch (event.type) {
    case EventType::MouseDown:
      if (inside) {
        pressed_inside_ = true;
        SetFocused(true);
        // Calculate cursor position based on click
        cursor_pos_ = text_.length();  // Simple implementation - set to end
        MarkDirty();
        consumed = true;
      } else {
        SetFocused(false);
      }
      break;
    case EventType::MouseUp:
      if (pressed_inside_) {
        consumed = true;
      }
      pressed_inside_ = false;
      break;
    case EventType::KeyDown:
      // Always consume key down if focused
      if (focused_) {
        consumed = true;
        if (event.key == "Backspace" && !text_.empty() && cursor_pos_ > 0) {
          text_.erase(cursor_pos_ - 1, 1);
          cursor_pos_--;
          MarkDirty();
        } else if (event.key == "Delete" && cursor_pos_ < text_.length()) {
          text_.erase(cursor_pos_, 1);
          MarkDirty();
        } else if (event.key == "Left" && cursor_pos_ > 0) {
          cursor_pos_--;
          MarkDirty();
        } else if (event.key == "Right" && cursor_pos_ < text_.length()) {
          cursor_pos_++;
          MarkDirty();
        } else if (event.key == "Home") {
          cursor_pos_ = 0;
          MarkDirty();
        } else if (event.key == "End") {
          cursor_pos_ = text_.length();
          MarkDirty();
        } else if (!event.key.empty() && event.key != "Backspace" && event.key != "Delete" && event.key != "Left" && event.key != "Right" && event.key != "Home" && event.key != "End") {
          // Insert character
          text_.insert(cursor_pos_, event.key);
          cursor_pos_ += event.key.length();
          MarkDirty();
        }
      }
      break;
    default:
      break;
  }
  return consumed;
}

void blTextfield::SetText(const std::string& text) {
  if (text_ != text) {
    text_ = text;
    cursor_pos_ = std::min(cursor_pos_, text_.length());
    MarkDirty();
  }
}

const std::string& blTextfield::GetText() const {
  return text_;
}

void blTextfield::SetPlaceholder(const std::string& placeholder) {
  if (placeholder_ != placeholder) {
    placeholder_ = placeholder;
    MarkDirty();
  }
}

void blTextfield::SetFocused(bool focused) {
  if (focused_ != focused) {
    focused_ = focused;
    if (focused_) {
      cursor_pos_ = text_.length();
    }
    MarkDirty();
  }
}

bool blTextfield::IsFocused() const {
  return focused_;
}

float blTextfield::PreferredWidth() const {
  return preferred_width_;
}

float blTextfield::PreferredHeight() const {
  return preferred_height_;
}

}  // namespace bl