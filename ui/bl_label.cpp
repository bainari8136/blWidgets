#include "ui/bl_label.h"

#include <iostream>

namespace bl {

blLabel::blLabel(std::string text) : text_(std::move(text)) {
  SetBounds(0.0F, 0.0F, preferred_width_, preferred_height_);
}

void blLabel::Paint(BLContext& ctx) {
  BLFontFace face;
  if (face.create_from_file("C:/Windows/Fonts/segoeui.ttf") == BL_SUCCESS) {
    BLFont font;
    if (font.create_from_face(face, 18.0F) == BL_SUCCESS) {
      const double text_x = X() + GetStyle().padding;
      const double text_y = Y() + (Height() * 0.75);
      ctx.set_fill_style(GetStyle().textColor);
      ctx.fill_utf8_text(BLPoint(text_x, text_y), font, text_.c_str());
    }
  }
}

bool blLabel::OnEvent(const Event& event) {
  (void)event;  // Labels don't handle events
  return false;
}

float blLabel::PreferredWidth() const {
  return preferred_width_;
}

float blLabel::PreferredHeight() const {
  return preferred_height_;
}

void blLabel::SetText(const std::string& text) {
  if (text_ != text) {
    text_ = text;
    MarkDirty();
  }
}

const std::string& blLabel::GetText() const {
  return text_;
}

}  // namespace bl