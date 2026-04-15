#include "ui/bl_event_dispatcher.h"

#include "platform/bl_window.h"
#include "rendering/bl_render_context.h"
#include "ui/bl_ui_root.h"

namespace bl {

void blEventDispatcher::Dispatch(
    const std::vector<Event>& events,
    blWindow& window,
    blRenderContext& render_context,
    blUiRoot& ui_root) const {
  for (const Event& event : events) {
    if (DispatchWindowEvent(event, window, render_context, ui_root)) {
      continue;
    }
    DispatchInputEvent(event, ui_root);
  }
}

bool blEventDispatcher::DispatchWindowEvent(
    const Event& event,
    blWindow& window,
    blRenderContext& render_context,
    blUiRoot& ui_root) const {
  if (event.type == EventType::WindowClose) {
    window.RequestClose();
    return true;
  }
  if (event.type == EventType::WindowResized) {
    render_context.Resize(event.width, event.height);
    ui_root.SetSize(static_cast<float>(event.width), static_cast<float>(event.height));
    return true;
  }
  return false;
}

void blEventDispatcher::DispatchInputEvent(const Event& event, blUiRoot& ui_root) const {
  ui_root.DispatchEvent(event);
}

}  // namespace bl
