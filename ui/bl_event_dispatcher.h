#pragma once

#include <vector>

#include "ui/bl_event.h"

namespace bl {

class blWindow;
class blRenderContext;
class blUiRoot;

class blEventDispatcher {
 public:
  void Dispatch(const std::vector<Event>& events, blWindow& window, blRenderContext& render_context, blUiRoot& ui_root) const;

 private:
  bool DispatchWindowEvent(const Event& event, blWindow& window, blRenderContext& render_context, blUiRoot& ui_root) const;
  void DispatchInputEvent(const Event& event, blUiRoot& ui_root) const;
};

}  // namespace bl
