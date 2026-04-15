#include <iostream>

#include "core/bl_app.h"

int main() {
  bl::blApp app;
  if (!app.Init()) {
    std::cerr << "Failed to initialize blWidgets prototype." << std::endl;
    return 1;
  }

  app.Run();
  app.Shutdown();
  return 0;
}
