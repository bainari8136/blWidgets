#include "ui/bl_progress_bar.h"

#include <iomanip>
#include <sstream>

namespace bl {

blProgressBar::blProgressBar(float min_value, float max_value, float value, float preferred_width, float preferred_height)
    : min_value_(min_value), max_value_(max_value), value_(value), preferred_width_(preferred_width), preferred_height_(preferred_height) {
  SetBounds(0.0F, 0.0F, preferred_width_, preferred_height_);
}

void blProgressBar::Paint(BLContext& ctx) {
  // Draw background
  BLRoundRect bg_rect(X(), Y(), Width(), Height(), Height() / 2.0F);
  ctx.set_comp_op(BL_COMP_OP_SRC_OVER);
  ctx.set_fill_style(BLRgba32(0xE0, 0xE0, 0xE0, 0xFF));
  ctx.fill_round_rect(bg_rect);

  // Draw progress fill
  const float progress_ratio = (value_ - min_value_) / (max_value_ - min_value_);
  const float clamped_ratio = std::max(0.0F, std::min(1.0F, progress_ratio));
  const float fill_width = clamped_ratio * (Width() - 4.0F);

  if (fill_width > 0.0F) {
    BLRoundRect fill_rect(X() + 2.0F, Y() + 2.0F, fill_width, Height() - 4.0F, (Height() - 4.0F) / 2.0F);
    ctx.set_fill_style(BLRgba32(0x42, 0x85, 0xF4, 0xFF));
    ctx.fill_round_rect(fill_rect);
  }

  // Draw border
  ctx.set_stroke_style(BLRgba32(0xB0, 0xB0, 0xB0, 0xFF));
  ctx.set_stroke_width(1.0);
  ctx.stroke_round_rect(bg_rect);

  // Draw text
  if (show_text_) {
    BLFontFace face;
    if (face.create_from_file("C:/Windows/Fonts/segoeui.ttf") == BL_SUCCESS) {
      BLFont font;
      if (font.create_from_face(face, 12.0F) == BL_SUCCESS) {
        std::string text;
        if (!custom_text_.empty()) {
          text = custom_text_;
        } else {
          std::stringstream ss;
          ss << std::fixed << std::setprecision(1) << (progress_ratio * 100.0F) << "%";
          text = ss.str();
        }

        // Center the text (approximate width calculation)
        const double approx_text_width = text.length() * 8.0; // Rough estimate
        const double text_x = X() + (Width() - approx_text_width) / 2.0;
        const double text_y = Y() + (Height() * 0.7);

        ctx.set_fill_style(BLRgba32(0x21, 0x21, 0x21, 0xFF));
        ctx.fill_utf8_text(BLPoint(text_x, text_y), font, text.c_str());
      }
    }
  }
}

bool blProgressBar::OnEvent(const Event& event) {
  // Progress bars typically don't handle input events
  (void)event;
  return false;
}

void blProgressBar::SetValue(float value) {
  value = std::max(min_value_, std::min(max_value_, value));
  if (value_ != value) {
    value_ = value;
    MarkDirty();
  }
}

float blProgressBar::GetValue() const {
  return value_;
}

void blProgressBar::SetRange(float min_value, float max_value) {
  if (min_value >= max_value) {
    return;  // Invalid range
  }

  min_value_ = min_value;
  max_value_ = max_value;
  value_ = std::max(min_value_, std::min(max_value_, value_));
  MarkDirty();
}

void blProgressBar::SetShowText(bool show_text) {
  if (show_text_ != show_text) {
    show_text_ = show_text;
    MarkDirty();
  }
}

void blProgressBar::SetText(const std::string& text) {
  if (custom_text_ != text) {
    custom_text_ = text;
    MarkDirty();
  }
}

float blProgressBar::PreferredWidth() const {
  return preferred_width_;
}

float blProgressBar::PreferredHeight() const {
  return preferred_height_;
}

}  // namespace bl