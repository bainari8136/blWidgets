#pragma once

#include <string>

namespace bl {

enum class EventType {
  MouseMove,
  MouseDown,
  MouseUp,
  KeyDown,
  KeyUp,
  WindowResized,
  WindowClose
};

struct Event {
  EventType type = EventType::MouseMove;
  float x = 0.0F;
  float y = 0.0F;
  int button = 0;
  std::string key;
  int width = 0;
  int height = 0;
};

}  // namespace bl
