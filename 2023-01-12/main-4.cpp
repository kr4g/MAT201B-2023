#include "al/app/al_App.hpp"  // al::App

using namespace al;
struct MyApp : App {
  float value = 0;
  float increment = 0.07;
  bool should_click = false;

  // 40 Hz
  void onAnimate(double dt) override {
    // std::cout << 1 / dt << std::endl;
    value += increment;
    if (value >= 1.0) {
      value -= 1.0;
      should_click = true;
    }
  }
  void onDraw(Graphics& g) override {
    g.clear(value);
    //
  }

  // 100 Hz
  void onSound(AudioIOData& io) override {
    // 16, 32, 128, 1024, 4096 .. generrate a number of samples

    if (should_click) {
      io();
      io.out(0) = io.out(1) = 1;
      should_click = false;
    }

    while (io()) {
      io.out(0) = 0;
      io.out(1) = 0;
    }
  }

  bool onKeyDown(Keyboard const& k) override {
    switch (k.key()) {
      case '1':
        increment += 0.001;
        break;
      case ',':
        increment += 0.001;
        break;
      case '.':
        increment -= 0.001;
        break;
    }
  }
};

int main() {
  MyApp app;
  app.start();  // blocks; there is a loop in there
  return 0;
}
