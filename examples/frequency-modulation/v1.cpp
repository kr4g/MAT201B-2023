// Karl Yerkes
// MAT201B
// 2022-03-10
//

#include "Gamma/Oscillator.h" // gam::Sine<>
#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

using namespace al;

template <typename T>
T mtof(T m) {
  return 440 * pow(2, (m - 69) / 12);
}
template <typename T>
T dbtoa(T db) {
  return pow(10, db / 20);
}

struct MyApp : App {
  static const int P = 6;
  Parameter frequency{"Frequency", 60, 0, 127};
  Parameter modulation{"Modulation", 60, 0, 127};
  Parameter index{"Index", 60, 0, 127};
  Parameter gain{"Gain", -90, -90, 0};

  gam::Sine<> carrier;
  gam::Sine<> modulator;

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(gain).add(modulation).add(index).add(frequency);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      modulator.freq(mtof(modulation.get()));
      carrier.freq(mtof(frequency.get()) + mtof(index.get()) * modulator());
      float v = carrier() * dbtoa(gain.get());
      io.out(0) = io.out(1) = v;
    }
  }

  void onCreate() override {}
  void onAnimate(double dt) override {}
  void onDraw(Graphics& g) override { g.clear(0.1); }
  bool onKeyDown(const Keyboard& k) override { return false; }
};

int main() {
  MyApp app;
  app.configureAudio(44100, 512, 2, 2);
  app.start();
}
