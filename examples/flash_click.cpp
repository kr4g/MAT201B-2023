// Karl Yerkes
// MAT201B
// 2022-01-04
// like flash.cpp, but crashes
//

#include "al/app/al_App.hpp"

using namespace al;

struct MyApp : App {
  bool should_flash;  // shared memory
  bool should_click;

  void onCreate() override {
    should_flash = false;
    should_click = false;
  }

  void onAnimate(double dt) override {}

  // called at 60 Hz
  void onDraw(Graphics& g) override {
    g.clear(0);
    if (should_flash) {
      should_flash = false;
      g.clear(1);
    }
  }

  bool onKeyDown(const Keyboard& k) override {
    should_flash = true;
    should_click = true;
    return false;
  }

  // 512 / 48000 --> 0.010667ms (93.75 Hz)
  void onSound(AudioIOData& io) override {
    if (should_click) {
      should_click = false;
      io();
      io.out(0) = 1;
      io.out(1) = 1;
    }
    while (io()) {
      io.out(0) = 0;
      io.out(1) = 0;
    }
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
