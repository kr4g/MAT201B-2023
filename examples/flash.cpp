// Karl Yerkes
// MAT201B
// 2022-01-04
// flash white when a key goes down
//

#include "al/app/al_App.hpp"

using namespace al;

struct MyApp : App {
  bool should_flash;

  void onCreate() override {
    should_flash = false;
  }

  void onAnimate(double dt) override {
  }

  void onDraw(Graphics& g) override {
    g.clear(0);
    if (should_flash) {
      should_flash = false;
      g.clear(1);
    }
  }

  bool onKeyDown(const Keyboard& k) override {
    should_flash = true;
    return false;
  }
};

int main() {
  MyApp app;
  app.start();
}
