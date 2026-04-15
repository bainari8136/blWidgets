#include "ui/bl_checkbox.h"

#include <iostream>

namespace bl {

blCheckbox::blCheckbox(std::string label, bool checked)
    : label_(std::move(label)), checked_(checked) {
  SetBounds(0.0F, 0.0F, preferred_width_, preferred_height_);
}

void blCheckbox::Paint(BLContext& ctx) {
  const float box_size = 20.0F;
  const float box_x = X();
  const float box_y = Y() + (Height() - box_size) * 0.5F;

  // Draw checkbox background
  BLRoundRect box_rect(box_x, box_y, box_size, box_size, 3.0);
  ctx.set_comp_op(BL_COMP_OP_SRC_OVER);
  ctx.set_fill_style(GetStyle().backgroundColor);
  ctx.fill_round_rect(box_rect);

  ctx.set_stroke_style(BLRgba32(0x1F, 0x2A, 0x38, 0xFF));
  ctx.set_stroke_width(1.5);
  ctx.stroke_round_rect(box_rect);

  // Draw checkmark if checked
  if (checked_) {
    ctx.set_stroke_style(BLRgba32(0x00, 0x00, 0x00, 0xFF));
    ctx.set_stroke_width(2.0);
    BLPath path;
    path.move_to(box_x + 4, box_y + box_size * 0.5F);
    path.line_to(box_x + box_size * 0.4F, box_y + box_size * 0.8F);
    path.line_to(box_x + box_size - 4, box_y + 4);
    ctx.stroke_path(path);
  }

  // Draw label
  BLFontFace face;
  if (face.create_from_file("C:/Windows/Fonts/segoeui.ttf") == BL_SUCCESS) {
    BLFont font;
    if (font.create_from_face(face, 18.0F) == BL_SUCCESS) {
      const double text_x = box_x + box_size + 8.0;
      const double text_y = Y() + (Height() * 0.75);
      ctx.set_fill_style(GetStyle().textColor);
      ctx.fill_utf8_text(BLPoint(text_x, text_y), font, label_.c_str());
    }
  }
}

bool blCheckbox::OnEvent(const Event& event) {
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
        checked_ = !checked_;
        std::cout << "blCheckbox toggled: " << label_ << " -> " << (checked_ ? "checked" : "unchecked") << std::endl;
        MarkDirty();
        consumed = true;
      }
      pressed_inside_ = false;
      break;
    default:
      break;
  }
  return consumed;
}

float blCheckbox::PreferredWidth() const {
  return preferred_width_;
}

float blCheckbox::PreferredHeight() const {
  return preferred_height_;
}

void blCheckbox::SetChecked(bool checked) {
  if (checked_ != checked) {
    checked_ = checked;
    MarkDirty();
  }
}

bool blCheckbox::IsChecked() const {
  return checked_;
}

void blCheckbox::SetLabel(const std::string& label) {
  if (label_ != label) {
    label_ = label;
    MarkDirty();
  }
}

}  // namespace bl