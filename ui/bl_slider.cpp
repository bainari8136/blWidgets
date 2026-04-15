#include "ui/bl_slider.h"

#include <algorithm>
#include <iostream>

namespace bl {

blSlider::blSlider(std::string label, float min_value, float max_value, float value)
    : label_(std::move(label)), min_value_(min_value), max_value_(max_value), value_(std::clamp(value, min_value, max_value)) {
  SetBounds(0.0F, 0.0F, preferred_width_, preferred_height_);
}

void blSlider::Paint(BLContext& ctx) {
  const float track_height = 4.0F;
  const float track_y = Y() + Height() - 15.0F;
  const float thumb_radius = 8.0F;

  // Draw track
  BLRect track_rect(X() + thumb_radius, track_y, Width() - 2 * thumb_radius, track_height);
  ctx.set_comp_op(BL_COMP_OP_SRC_OVER);
  ctx.set_fill_style(BLRgba32(0xCC, 0xCC, 0xCC, 0xFF));
  ctx.fill_round_rect(BLRoundRect(track_rect, track_height * 0.5F));

  // Draw filled track
  float value_ratio = (value_ - min_value_) / (max_value_ - min_value_);
  BLRect filled_track_rect(X() + thumb_radius, track_y, (Width() - 2 * thumb_radius) * value_ratio, track_height);
  ctx.set_fill_style(GetStyle().backgroundColor);
  ctx.fill_round_rect(BLRoundRect(filled_track_rect, track_height * 0.5F));

  // Draw thumb
  float thumb_x = X() + thumb_radius + (Width() - 2 * thumb_radius) * value_ratio;
  float thumb_y = track_y + track_height * 0.5F;
  BLCircle thumb_circle(thumb_x, thumb_y, thumb_radius);
  ctx.set_fill_style(GetStyle().backgroundColor);
  ctx.fill_circle(thumb_circle);
  ctx.set_stroke_style(BLRgba32(0x1F, 0x2A, 0x38, 0xFF));
  ctx.set_stroke_width(1.5);
  ctx.stroke_circle(thumb_circle);

  // Draw label and value
  BLFontFace face;
  if (face.create_from_file("C:/Windows/Fonts/segoeui.ttf") == BL_SUCCESS) {
    BLFont font;
    if (font.create_from_face(face, 14.0F) == BL_SUCCESS) {
      std::string display_text = label_ + ": " + std::to_string(static_cast<int>(value_));
      const double text_x = X();
      const double text_y = Y() + 15.0;
      ctx.set_fill_style(GetStyle().textColor);
      ctx.fill_utf8_text(BLPoint(text_x, text_y), font, display_text.c_str());
    }
  }
}

bool blSlider::OnEvent(const Event& event) {
  bool consumed = false;
  switch (event.type) {
    case EventType::MouseDown: {
      if (HitTest(event.x, event.y)) {
        float value_ratio = (event.x - X() - 8.0F) / (Width() - 16.0F);
        value_ratio = std::clamp(value_ratio, 0.0F, 1.0F);
        float new_value = min_value_ + value_ratio * (max_value_ - min_value_);
        if (new_value != value_) {
          value_ = new_value;
          dragging_ = true;
          std::cout << "blSlider value changed: " << label_ << " -> " << value_ << std::endl;
          MarkDirty();
        } else {
          dragging_ = true;
        }
        consumed = true;
      }
      break;
    }
    case EventType::MouseMove:
      if (dragging_) {
        float value_ratio = (event.x - X() - 8.0F) / (Width() - 16.0F);
        value_ratio = std::clamp(value_ratio, 0.0F, 1.0F);
        float new_value = min_value_ + value_ratio * (max_value_ - min_value_);
        if (new_value != value_) {
          value_ = new_value;
          MarkDirty();
        }
      }
      break;
    case EventType::MouseUp:
      if (dragging_) {
        consumed = true;
      }
      dragging_ = false;
      break;
    default:
      break;
  }
  return consumed;
}

float blSlider::PreferredWidth() const {
  return preferred_width_;
}

float blSlider::PreferredHeight() const {
  return preferred_height_;
}

void blSlider::SetValue(float value) {
  float clamped_value = std::clamp(value, min_value_, max_value_);
  if (value_ != clamped_value) {
    value_ = clamped_value;
    MarkDirty();
  }
}

float blSlider::GetValue() const {
  return value_;
}

void blSlider::SetRange(float min_value, float max_value) {
  min_value_ = min_value;
  max_value_ = max_value;
  value_ = std::clamp(value_, min_value_, max_value_);
  MarkDirty();
}

void blSlider::SetLabel(const std::string& label) {
  if (label_ != label) {
    label_ = label;
    MarkDirty();
  }
}

}  // namespace bl