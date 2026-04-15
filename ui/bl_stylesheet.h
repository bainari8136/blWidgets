#pragma once

#include <string>
#include <vector>

#include "ui/bl_style.h"

namespace bl {

class blWidget;

class blStyleSheet {
 public:
  bool LoadFromString(const std::string& css_text);
  blStyle ComputeStyle(const blWidget& widget, const blStyle& base_style) const;

 private:
  struct Selector {
    std::string type;
    std::string id;
    std::vector<std::string> classes;
  };

  struct Rule {
    Selector selector;
    bool has_background_color = false;
    BLRgba32 background_color;
    bool has_text_color = false;
    BLRgba32 text_color;
    bool has_padding = false;
    float padding = 0.0F;
  };

  static std::string Trim(const std::string& value);
  static bool ParseSelector(const std::string& text, Selector& selector);
  static bool ParseHexColor(const std::string& value, BLRgba32& color);
  static bool ParsePadding(const std::string& value, float& padding);
  static bool Matches(const Selector& selector, const blWidget& widget);

  std::vector<Rule> rules_;
};

}  // namespace bl
