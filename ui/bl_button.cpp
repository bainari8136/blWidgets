#include "ui/bl_button.h"

#include <iostream>
#include <utility>

namespace bl {

blButton::blButton(std::string label, float preferred_width, float preferred_height)

    : label_(std::move(label)), preferred_width_(preferred_width), preferred_height_(preferred_height) {
  current_background_color_ = GetStyle().backgroundColor;
  target_background_color_ = GetStyle().backgroundColor;
  SetBounds(0.0F, 0.0F, preferred_width_, preferred_height_);
}

void blButton::Paint(BLContext& ctx) {
  BLRoundRect button_rect(X(), Y(), Width(), Height(), 8.0);
  ctx.set_comp_op(BL_COMP_OP_SRC_OVER);
  ctx.set_fill_style(current_background_color_);
  ctx.fill_round_rect(button_rect);

  ctx.set_stroke_style(BLRgba32(0x1F, 0x2A, 0x38, 0xFF));
  ctx.set_stroke_width(1.5);
  ctx.stroke_round_rect(button_rect);

  BLFontFace face;
  if (face.create_from_file("C:/Windows/Fonts/segoeui.ttf") == BL_SUCCESS) {
    BLFont font;
    if (font.create_from_face(face, 18.0F) == BL_SUCCESS) {
      const double text_x = X() + GetStyle().padding;
      const double text_y = Y() + (Height() * 0.63);
      ctx.set_fill_style(GetStyle().textColor);
      ctx.fill_utf8_text(BLPoint(text_x, text_y), font, label_.c_str());
    }
  }
}

bool blButton::OnEvent(const Event& event) {
  const State previous_state = state_;
  const bool previous_pressed_inside = pressed_inside_;
  const bool inside = HitTest(event.x, event.y);
  bool consumed = false;

  switch (event.type) {
    case EventType::MouseMove:
      if (pressed_inside_) {
        state_ = inside ? State::Pressed : State::Normal;
      } else {
        state_ = inside ? State::Hovered : State::Normal;
      }
      break;
    case EventType::MouseDown:
      if (inside) {
        pressed_inside_ = true;
        state_ = State::Pressed;
        consumed = true;
      }
      break;
    case EventType::MouseUp:
      if (pressed_inside_ && inside) {
        std::cout << "blButton clicked: " << label_ << std::endl;
        consumed = true;
      }
      pressed_inside_ = false;
      state_ = inside ? State::Hovered : State::Normal;
      break;
    default:
      break;
  }

  // Update target color based on state
  BLRgba32 base_color = GetStyle().backgroundColor;
  if (state_ == State::Hovered) {
    target_background_color_ = BLRgba32(
        static_cast<uint32_t>((static_cast<int>(base_color.r()) + 20) > 255 ? 255 : (static_cast<int>(base_color.r()) + 20)),
        static_cast<uint32_t>((static_cast<int>(base_color.g()) + 20) > 255 ? 255 : (static_cast<int>(base_color.g()) + 20)),
        static_cast<uint32_t>((static_cast<int>(base_color.b()) + 20) > 255 ? 255 : (static_cast<int>(base_color.b()) + 20)),
        base_color.a());
  } else if (state_ == State::Pressed) {
    target_background_color_ = BLRgba32(
        static_cast<uint32_t>((static_cast<int>(base_color.r()) - 30) < 0 ? 0 : (static_cast<int>(base_color.r()) - 30)),
        static_cast<uint32_t>((static_cast<int>(base_color.g()) - 30) < 0 ? 0 : (static_cast<int>(base_color.g()) - 30)),
        static_cast<uint32_t>((static_cast<int>(base_color.b()) - 30) < 0 ? 0 : (static_cast<int>(base_color.b()) - 30)),
        base_color.a());
  } else {
    target_background_color_ = base_color;
  }

  // Reset animation if state changed
  if (state_ != previous_state || pressed_inside_ != previous_pressed_inside) {
    animation_progress_ = 0.0f;
    MarkDirty();
  }

  return consumed;
}

void blButton::Update(float deltaTime) {
  blWidget::Update(deltaTime);

  if (animation_progress_ < 1.0f) {
    animation_progress_ += deltaTime * animation_speed_;
    animation_progress_ = (animation_progress_ > 1.0f) ? 1.0f : animation_progress_;

    // Interpolate between current and target
    float t = animation_progress_;
    current_background_color_ = BLRgba32(
        static_cast<uint32_t>(current_background_color_.r() + t * (target_background_color_.r() - current_background_color_.r())),
        static_cast<uint32_t>(current_background_color_.g() + t * (target_background_color_.g() - current_background_color_.g())),
        static_cast<uint32_t>(current_background_color_.b() + t * (target_background_color_.b() - current_background_color_.b())),
        static_cast<uint32_t>(current_background_color_.a() + t * (target_background_color_.a() - current_background_color_.a()))
    );

    MarkDirty();
  }
}

void blButton::SetCurrentBackgroundColor(BLRgba32 color) {
  current_background_color_ = color;
  target_background_color_ = color;
  animation_progress_ = 1.0f;
}

float bl::blButton::PreferredWidth() const {
  return preferred_width_;
}

float bl::blButton::PreferredHeight() const {
  return preferred_height_;
}

void blButton::SetStyle(const blStyle& style) {
  blWidget::SetStyle(style);
  current_background_color_ = style.backgroundColor;
  target_background_color_ = style.backgroundColor;
  animation_progress_ = 1.0f;
}
 
}