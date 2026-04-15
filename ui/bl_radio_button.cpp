#include "ui/bl_radio_button.h"

#include <iostream>

namespace bl {

blRadioButton::blRadioButton(std::string label, bool selected, float preferred_width, float preferred_height)
    : label_(std::move(label)), selected_(selected), preferred_width_(preferred_width), preferred_height_(preferred_height) {
  SetBounds(0.0F, 0.0F, preferred_width_, preferred_height_);
}

void blRadioButton::Paint(BLContext& ctx) {
  const float radius = 8.0F;
  const float center_x = X() + radius + 4.0F;
  const float center_y = Y() + Height() / 2.0F;

  // Draw outer circle
  ctx.set_comp_op(BL_COMP_OP_SRC_OVER);
  ctx.set_stroke_style(BLRgba32(0x42, 0x85, 0xF4, 0xFF));
  ctx.set_stroke_width(2.0);
  ctx.stroke_circle(center_x, center_y, radius);

  // Draw inner circle if selected
  if (selected_) {
    ctx.set_fill_style(BLRgba32(0x42, 0x85, 0xF4, 0xFF));
    ctx.fill_circle(center_x, center_y, radius * 0.5F);
  }

  // Draw label
  BLFontFace face;
  if (face.create_from_file("C:/Windows/Fonts/segoeui.ttf") == BL_SUCCESS) {
    BLFont font;
    if (font.create_from_face(face, 14.0F) == BL_SUCCESS) {
      const double text_x = X() + radius * 2.0 + 8.0;
      const double text_y = Y() + (Height() * 0.7);
      ctx.set_fill_style(BLRgba32(0x21, 0x21, 0x21, 0xFF));
      ctx.fill_utf8_text(BLPoint(text_x, text_y), font, label_.c_str());
    }
  }
}

bool blRadioButton::OnEvent(const Event& event) {
  const bool inside = HitTest(event.x, event.y);
  bool consumed = false;

  switch (event.type) {
    case EventType::MouseDown:
      if (inside) {
        pressed_inside_ = true;
        consumed = true;
      }
      break;
    case EventType::MouseUp:
      if (pressed_inside_ && inside) {
        SetSelected(true);
        std::cout << "blRadioButton selected: " << label_ << std::endl;
        consumed = true;
      }
      pressed_inside_ = false;
      break;
    default:
      break;
  }
  return consumed;
}

void blRadioButton::SetSelected(bool selected) {
  if (selected_ != selected) {
    selected_ = selected;
    MarkDirty();
  }
}

bool blRadioButton::IsSelected() const {
  return selected_;
}

void blRadioButton::SetLabel(const std::string& label) {
  if (label_ != label) {
    label_ = label;
    MarkDirty();
  }
}

const std::string& blRadioButton::GetLabel() const {
  return label_;
}

float blRadioButton::PreferredWidth() const {
  return preferred_width_;
}

float blRadioButton::PreferredHeight() const {
  return preferred_height_;
}

}  // namespace bl