#pragma once

#include <memory>
#include <vector>

#include "platform/bl_window.h"
#include "rendering/bl_render_context.h"
#include "ui/bl_animation.h"
#include "ui/bl_event_dispatcher.h"
#include "ui/bl_ui_root.h"

namespace bl {

class blApp {
 public:
  bool Init();
  void Run();
  void Shutdown();

  void AddAnimation(std::shared_ptr<blAnimation> animation);

 private:
  void UpdateAnimations(float deltaTime);

 private:
  blWindow window_;
  blRenderContext render_context_;
  blUiRoot ui_root_;
  blEventDispatcher event_dispatcher_;
  std::vector<std::shared_ptr<blAnimation>> animations_;
};

}  // namespace bl
