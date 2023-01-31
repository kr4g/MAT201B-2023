// Karl Yerkes
// MAT201B
// 2022-01-04
// minimal app, ready for adapting..
//

#include "al/app/al_App.hpp"

using namespace al;

struct MyApp : App {
  void onCreate() override {
    //
  }

  void onAnimate(double dt) override {
    //
  }

  void onDraw(Graphics& g) override {
    g.clear(1, 0, 0);
    //g.clear(0.25);
    //
  }

  bool onKeyDown(const Keyboard& k) override {
    //
    return false;
  }
  
  void onSound(AudioIOData& io) override {
    //
  }
};

int main() {
  MyApp app;
  app.start();
}
