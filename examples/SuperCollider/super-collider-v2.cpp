// Karl Yerkes
// MAT201B
// 2022-02-22
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

using namespace al;

struct MyApp : App {
  Parameter value{"Value", 0, 0, 1};

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(value);
    parameterServer() << value;
  }

  void onDraw(Graphics& g) override { g.clear(0.2); }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}