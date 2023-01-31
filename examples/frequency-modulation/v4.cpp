// Karl Yerkes
// MAT201B
// 2022-03-10
//

#include "Gamma/DFT.h"         // STFT
#include "Gamma/Envelope.h"    //Env<2>
#include "Gamma/Oscillator.h"  // Sine<>
#include "al/app/al_DistributedApp.hpp"
#include "al/math/al_Random.hpp"
#include "al/scene/al_DistributedScene.hpp"

using namespace gam;
using namespace al;

template <typename T>
T mtof(T m) {
  return 440 * pow(2, (m - 69) / 12);
}

template <typename T>
T dbtoa(T db) {
  return pow(10, db / 20);
}

struct SimpleVoice : PositionedVoice {
  Parameter frequency{"Frequency"};
  Parameter modulation{"Modulation"};
  Parameter index{"Index"};
  gam::Sine<> carrier;
  gam::Sine<> modulator;
  gam::Env<2> envelope;
  gam::STFT stft{1024, 1024 / 2, 0, gam::HANN, gam::COMPLEX};
  float amplitude{0};
  Mesh mesh;

  SimpleVoice() {
    registerParameters(frequency, modulation, index);

    envelope.levels(0, 1, 0);
    envelope.lengths(0.5, 0.5);

    mesh.primitive(Mesh::LINE_STRIP);
    for (int i = 0; i < stft.numBins(); i++) {
      mesh.vertex((float)i / stft.numBins(), 0);
    }
  }

  virtual void update(double dt) override {
    if (mIsReplica) return;
    // do things in the primary instance...
    amplitude = envelope.value();
  }

  virtual void onProcess(AudioIOData &io) override {
    while (io()) {
      modulator.freq(mtof(modulation.get()));
      carrier.freq(mtof(frequency.get()) + mtof(index.get()) * modulator());
      float fm = carrier();
      if (stft(fm)) {
        for (int i = 0; i < stft.numBins(); i++) {
          mesh.vertices()[i].y = log(1 + stft.bin(i).mag());
        }
      }
      io.out(0) += fm * envelope() * 0.1;
    }
    if (envelope.done()) free();  // frees this voice
  }

  virtual void onProcess(Graphics &g) override {
    g.color(amplitude);
    g.draw(mesh);
  }

  virtual void onTriggerOn() override {
    envelope.reset();
    frequency.set(rnd::uniform(127.0f));
    modulation.set(rnd::uniform(127.0f));
    index.set(rnd::uniform(127.0f));
  }
};

struct MyApp : public DistributedApp {
  void onCreate() {
    scene.registerSynthClass<SimpleVoice>();
    registerDynamicScene(scene);
    scene.verbose(true);  // turns on messages about voices to the console
  }

  void onAnimate(double dt) { scene.update(dt); }
  void onSound(AudioIOData &io) { scene.render(io); }
  void onDraw(Graphics &g) {
    g.clear();
    scene.render(g);  // Render graphics
  }

  bool onKeyDown(Keyboard const &k) {
    if (isPrimary() && k.key() == ' ') {  // Start a new voice on space bar
      auto *freeVoice = scene.getVoice<SimpleVoice>();
      Pose pose;
      pose.vec().x = al::rnd::uniformS(2);
      pose.vec().y = al::rnd::uniformS(2);
      pose.vec().z = -10.0 + al::rnd::uniform(6);
      freeVoice->setPose(pose);
      scene.triggerOn(freeVoice);
    }
    return true;
  }

  DistributedScene scene{TimeMasterMode::TIME_MASTER_CPU};
};

int main() {
  MyApp app;
  app.configureAudio(48000., 1024, 2, 0);
  Domain::master().spu(
      app.audioIO().framesPerSecond());  // tell Gamma the playback rate
  app.start();
}
