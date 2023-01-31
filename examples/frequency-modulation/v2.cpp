// Karl Yerkes
// MAT201B
// 2022-03-10
//

#include "Gamma/Oscillator.h"
#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

template <typename T>
T mtof(T m) {
  return 440 * pow(2, (m - 69) / 12);
}

template <typename T>
T dbtoa(T db) {
  return pow(10, db / 20);
}

struct FM {
  Parameter frequency{"Frequency", 60, 0, 127};
  Parameter modulation{"Modulation", 60, 0, 127};
  Parameter index{"Index", 60, 0, 127};
  gam::Sine<> carrier;
  gam::Sine<> modulator;

  float operator()() {
    modulator.freq(mtof(modulation.get()));
    carrier.freq(mtof(frequency.get()) + mtof(index.get()) * modulator());
    return carrier();
  }
};

struct MyApp : App {
  Parameter gain{"Gain", -90, -90, 0};
  ParameterColor background{"Bankground"};

  FM fm;
  gam::Accum<> timer;

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(gain).add(background);
    timer.period(1);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      if (timer()) {
        fm.frequency.set(rnd::uniform(127.0f));
        fm.modulation.set(rnd::uniform(127.0f));
        fm.index.set(rnd::uniform(127.0f));
        background.set(HSV(rnd::uniform(), rnd::uniform()));
      }
      io.out(0) = io.out(1) = fm() * dbtoa(gain.get());
    }
  }

  void onCreate() override {}
  void onAnimate(double dt) override {}
  void onDraw(Graphics& g) override {
    g.clear(background.get());
    //
  }
  bool onKeyDown(const Keyboard& k) override { return false; }
};

int main() {
  MyApp app;
  app.configureAudio(44100, 512, 2, 2);
  app.start();
}
