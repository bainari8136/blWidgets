#pragma once

#include <string>
#include <vector>

#include "ui/bl_widget.h"

namespace bl {

class blDropdown : public blWidget {
 public:
  explicit blDropdown(std::vector<std::string> options = {}, size_t selected_index = 0, float preferred_width = 200.0F, float preferred_height = 32.0F);

  void Paint(BLContext& ctx) override;
  bool OnEvent(const Event& event) override;
  float PreferredWidth() const override;
  float PreferredHeight() const override;

  void SetOptions(const std::vector<std::string>& options);
  void SetSelectedIndex(size_t index);
  size_t GetSelectedIndex() const;
  const std::string& GetSelectedOption() const;
  void SetOpen(bool open);
  bool IsOpen() const;

 private:
  std::vector<std::string> options_;
  size_t selected_index_ = 0;
  bool open_ = false;
  bool pressed_inside_ = false;
  float preferred_width_ = 200.0F;
  float preferred_height_ = 32.0F;
  float item_height_ = 24.0F;
};

}  // namespace bl