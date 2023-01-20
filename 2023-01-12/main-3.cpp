#include "al/app/al_App.hpp"  // al::App

struct MyApp : al::App {
  float value = 0;
  bool should_click = false;

  // 40 Hz
  void onAnimate(double dt) override {
    // std::cout << 1 / dt << std::endl;
    value += 0.01;
    if (value >= 1.0) {
      value -= 1.0;
      should_click = true;
    }
  }
  void onDraw(al::Graphics& g) override {
    g.clear(value);
    //
  }

  // 100 Hz
  void onSound(al::AudioIOData& io) override {
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
};

int main() {
  MyApp app;
  app.start();  // blocks; there is a loop in there
  return 0;
}
