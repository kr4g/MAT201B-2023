// Karl Yerkes | MAT201B
// 2022-02-22  | Audio-reactive visualizer
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Functions.hpp"  // al::abs

using namespace al;

struct MyApp : App {
  Parameter value{"value", 0, 0, 1};

  void onDraw(Graphics& g) override {
    g.clear(value.get());

    // properties of things we can draw?
    //
    // position
    // scale
    // rotation
    // symmetry

    // complexity (polygon count)
    // primitive
    // color
    // material / surface (smooth, rough, translucent)
    // texture (as in wrapped image)
    // lighting
    // number (how many things are there?)
    // shape
    // curvature (straight angles or curved?)
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = io.in(0);
      // float f = (io.in(0) + io.in(1)) * 0.5;  // ensure (-1, 1)
      f = (f < 0) ? -f : f;  // absolute value
      f = 1 - pow4(f - 1);   // non-linear transformation
      // https://www.desmos.com/calculator/qumteupggk
      value.set(f);
      io.out(0) = io.out(1) = 0;

      // qualities of sound?
      //
      // pitch / frequency / f0 (fundamnetal frequency)
      // pan / spatialization (where is the sound? what is the space?)
      // amplitude / volume / loudness / power
      // envelope (amplitude changes over time)
      // timbre (spectral changes over time)
      // rhythm
      //
      // https://github.com/adamstark/Gist
    }
  }

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(value);
    parameterServer() << value;
  }

  void onMessage(osc::Message& m) override { m.print(); }
  bool onKeyDown(const Keyboard& k) override { return false; }
  void onCreate() override {}
  void onAnimate(double dt) override {}
};

int main() {
  MyApp app;
  AudioDevice dev;
  // dev.print();
  //
  dev.printAll();
  // app.configureAudio(48000, 512, 2, 2);
  //  app.configureAudio(device, 48000, 512, 2, 2);
  app.configureAudio();  // init with out only
  app.start();
}
