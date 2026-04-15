#include "core/bl_app.h"

#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ui/bl_box_sizer.h"
#include "ui/bl_label.h"
#include "ui/bl_panel.h"
#include "ui/bl_style.h"
#include "ui/bl_textfield.h"
#include "ui/bl_dropdown.h"
#include "ui/bl_radio_button.h"
#include "ui/bl_scroll_container.h"
#include "ui/bl_progress_bar.h"

namespace bl {

bool blApp::Init() {
  if (!window_.Create(960, 600, "blWidgets Prototype")) {
    return false;
  }

  if (!render_context_.Initialize(
          window_.NativeHandle(),
          window_.FramebufferWidth(),
          window_.FramebufferHeight())) {
    return false;
  }

  // Create main vertical layout
  auto root_sizer = std::make_shared<blBoxSizer>(blBoxSizer::Orientation::Vertical);

  // Title label
  auto title_label = std::make_shared<blLabel>("blWidgets Demo - New Widgets");
  title_label->AddClass("title");

  // New widgets panel with internal layout
  auto widgets_panel = std::make_shared<blPanel>();
  widgets_panel->SetId("main-panel");
  widgets_panel->AddClass("panel");
  widgets_panel->SetBorderWidth(2.0f);

  // Create vertical sizer for panel contents
  auto panel_sizer = std::make_shared<blBoxSizer>(blBoxSizer::Orientation::Vertical);

  // Textfield
  auto textfield = std::make_shared<blTextfield>("Enter text here...");
  panel_sizer->Add(textfield, 0);

  // Radio buttons group
  auto radio_sizer = std::make_shared<blBoxSizer>(blBoxSizer::Orientation::Horizontal);
  auto radio1 = std::make_shared<blRadioButton>("Option A", true);
  auto radio2 = std::make_shared<blRadioButton>("Option B", false);
  auto radio3 = std::make_shared<blRadioButton>("Option C", false);
  radio_sizer->Add(radio1, 1);
  radio_sizer->Add(radio2, 1);
  radio_sizer->Add(radio3, 1);
  
  auto radio_panel = std::make_shared<blPanel>();
  radio_panel->SetLayoutSizer(radio_sizer);
  panel_sizer->Add(radio_panel, 0);

  // Dropdown and Progress bar row
  auto controls_sizer = std::make_shared<blBoxSizer>(blBoxSizer::Orientation::Horizontal);
  auto dropdown = std::make_shared<blDropdown>(std::vector<std::string>{"Choice 1", "Choice 2", "Choice 3", "Choice 4"}, 0);
  auto progress_bar = std::make_shared<blProgressBar>(0.0f, 100.0f, 65.0f, 200.0f, 20.0f);
  controls_sizer->Add(dropdown, 1);
  controls_sizer->Add(progress_bar, 1);
  
  auto controls_panel = std::make_shared<blPanel>();
  controls_panel->SetLayoutSizer(controls_sizer);
  panel_sizer->Add(controls_panel, 0);

  // Scroll container
  auto scroll_container = std::make_shared<blScrollContainer>();
  scroll_container->SetContentSize(280.0f, 300.0f);

  auto label1 = std::make_shared<blLabel>("Scrollable Content");
  auto label2 = std::make_shared<blLabel>("This is a scrollable area");
  auto label3 = std::make_shared<blLabel>("You can scroll to see more content");
  auto label4 = std::make_shared<blLabel>("Bottom of content");

  scroll_container->AddChild(label1);
  scroll_container->AddChild(label2);
  scroll_container->AddChild(label3);
  scroll_container->AddChild(label4);

  panel_sizer->Add(scroll_container, 1);

  widgets_panel->SetLayoutSizer(panel_sizer);
  widgets_panel->SetSize(500.0f, 350.0f);

  // Add everything to root
  root_sizer->Add(title_label, 0);
  root_sizer->Add(widgets_panel, 0);

  ui_root_.SetRootSizer(root_sizer);
  ui_root_.SetStyleSheet(R"css(
    .title {
      color: #2D3745;
      padding: 10px;
    }

    #main-panel {
      background-color: #F8F9FA;
    }

    label {
      color: #25381f;
    }
  )css");
  ui_root_.SetSize(static_cast<float>(window_.FramebufferWidth()), static_cast<float>(window_.FramebufferHeight()));
  return true;
}

void blApp::Run() {
  double previous_time = glfwGetTime();
  while (window_.IsRunning()) {
    double current_time = glfwGetTime();
    float delta_time = static_cast<float>(current_time - previous_time);
    previous_time = current_time;

    window_.PollEvents();
    event_dispatcher_.Dispatch(window_.ConsumeEvents(), window_, render_context_, ui_root_);

    // Update animations
    UpdateAnimations(delta_time);
    // Update UI
    ui_root_.Update(delta_time);
    if (!ui_root_.IsDirty()) {
      continue;
    }

    const std::vector<BLRect> dirty_regions = ui_root_.DirtyRegions();
    if (!render_context_.BeginFrame(BLRgba32(0x21, 0x28, 0x33, 0xFF), dirty_regions)) {
      window_.RequestClose();
      break;
    }

    ui_root_.Paint(render_context_.GetContext());
    render_context_.EndFrame();
    render_context_.Present();
    window_.SwapBuffers();
  }
}

void blApp::Shutdown() {
  render_context_.Shutdown();
  window_.Destroy();
}

void blApp::AddAnimation(std::shared_ptr<blAnimation> animation) {
  animations_.push_back(animation);
}

void blApp::UpdateAnimations(float deltaTime) {
  for (auto it = animations_.begin(); it != animations_.end();) {
    (*it)->Update(deltaTime);
    if ((*it)->IsFinished()) {
      it = animations_.erase(it);
    } else {
      ++it;
    }
  }
}

}  // namespace bl
