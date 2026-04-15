#include "ui/bl_stylesheet.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>

#include "ui/bl_widget.h"

namespace bl {

std::string blStyleSheet::Trim(const std::string& value) {
  size_t start = 0;
  while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])) != 0) {
    ++start;
  }
  size_t end = value.size();
  while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) {
    --end;
  }
  return value.substr(start, end - start);
}

bool blStyleSheet::ParseSelector(const std::string& text, Selector& selector) {
  selector = Selector{};
  const std::string token = Trim(text);
  if (token.empty() || token.find(' ') != std::string::npos) {
    return false;
  }

  size_t i = 0;
  while (i < token.size()) {
    if (token[i] == '#') {
      const size_t start = i + 1;
      size_t end = start;
      while (end < token.size() && token[end] != '.' && token[end] != '#') {
        ++end;
      }
      selector.id = token.substr(start, end - start);
      i = end;
      continue;
    }
    if (token[i] == '.') {
      const size_t start = i + 1;
      size_t end = start;
      while (end < token.size() && token[end] != '.' && token[end] != '#') {
        ++end;
      }
      selector.classes.push_back(token.substr(start, end - start));
      i = end;
      continue;
    }

    const size_t start = i;
    size_t end = start;
    while (end < token.size() && token[end] != '.' && token[end] != '#') {
      ++end;
    }
    selector.type = token.substr(start, end - start);
    i = end;
  }

  return !(selector.type.empty() && selector.id.empty() && selector.classes.empty());
}

bool blStyleSheet::ParseHexColor(const std::string& value, BLRgba32& color) {
  const std::string token = Trim(value);
  if (token.empty() || token[0] != '#') {
    return false;
  }

  if (token.size() == 7) {
    const unsigned int hex = static_cast<unsigned int>(std::strtoul(token.c_str() + 1, nullptr, 16));
    const uint32_t r = (hex >> 16U) & 0xFFU;
    const uint32_t g = (hex >> 8U) & 0xFFU;
    const uint32_t b = hex & 0xFFU;
    color = BLRgba32(r, g, b, 0xFFU);
    return true;
  }

  if (token.size() == 9) {
    const unsigned int hex = static_cast<unsigned int>(std::strtoul(token.c_str() + 1, nullptr, 16));
    const uint32_t r = (hex >> 24U) & 0xFFU;
    const uint32_t g = (hex >> 16U) & 0xFFU;
    const uint32_t b = (hex >> 8U) & 0xFFU;
    const uint32_t a = hex & 0xFFU;
    color = BLRgba32(r, g, b, a);
    return true;
  }

  return false;
}

bool blStyleSheet::ParsePadding(const std::string& value, float& padding) {
  const std::string token = Trim(value);
  if (token.empty()) {
    return false;
  }
  const bool has_px = token.size() > 2 && token.substr(token.size() - 2) == "px";
  const std::string number = has_px ? token.substr(0, token.size() - 2) : token;
  char* end_ptr = nullptr;
  const float parsed = std::strtof(number.c_str(), &end_ptr);
  if (end_ptr == number.c_str()) {
    return false;
  }
  padding = parsed;
  return true;
}

bool blStyleSheet::Matches(const Selector& selector, const blWidget& widget) {
  if (!selector.type.empty() && selector.type != widget.TypeName()) {
    return false;
  }
  if (!selector.id.empty() && selector.id != widget.Id()) {
    return false;
  }
  for (const auto& class_name : selector.classes) {
    if (!widget.HasClass(class_name)) {
      return false;
    }
  }
  return true;
}

bool blStyleSheet::LoadFromString(const std::string& css_text) {
  rules_.clear();

  size_t pos = 0;
  bool parsed_any_rule = false;
  while (pos < css_text.size()) {
    const size_t open = css_text.find('{', pos);
    if (open == std::string::npos) {
      break;
    }
    const size_t close = css_text.find('}', open + 1);
    if (close == std::string::npos) {
      break;
    }

    Selector selector;
    if (!ParseSelector(css_text.substr(pos, open - pos), selector)) {
      pos = close + 1;
      continue;
    }

    Rule rule;
    rule.selector = selector;
    const std::string body = css_text.substr(open + 1, close - open - 1);
    size_t decl_pos = 0;
    while (decl_pos < body.size()) {
      const size_t semicolon = body.find(';', decl_pos);
      const std::string declaration =
          semicolon == std::string::npos ? body.substr(decl_pos) : body.substr(decl_pos, semicolon - decl_pos);
      const size_t colon = declaration.find(':');
      if (colon != std::string::npos) {
        std::string property = Trim(declaration.substr(0, colon));
        std::transform(property.begin(), property.end(), property.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        const std::string value = Trim(declaration.substr(colon + 1));

        if (property == "background-color") {
          BLRgba32 color;
          if (ParseHexColor(value, color)) {
            rule.has_background_color = true;
            rule.background_color = color;
          }
        } else if (property == "color" || property == "text-color") {
          BLRgba32 color;
          if (ParseHexColor(value, color)) {
            rule.has_text_color = true;
            rule.text_color = color;
          }
        } else if (property == "padding") {
          float padding = 0.0F;
          if (ParsePadding(value, padding)) {
            rule.has_padding = true;
            rule.padding = padding;
          }
        }
      }

      if (semicolon == std::string::npos) {
        break;
      }
      decl_pos = semicolon + 1;
    }

    rules_.push_back(rule);
    parsed_any_rule = true;
    pos = close + 1;
  }

  return parsed_any_rule;
}

blStyle blStyleSheet::ComputeStyle(const blWidget& widget, const blStyle& base_style) const {
  blStyle result = base_style;
  for (const Rule& rule : rules_) {
    if (!Matches(rule.selector, widget)) {
      continue;
    }
    if (rule.has_background_color) {
      result.backgroundColor = rule.background_color;
    }
    if (rule.has_text_color) {
      result.textColor = rule.text_color;
    }
    if (rule.has_padding) {
      result.padding = rule.padding;
    }
  }
  return result;
}

}  // namespace bl
