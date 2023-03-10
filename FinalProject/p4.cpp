// Ryan Millett
// MAT201B-2023

#include "l-system-backend/parser.hpp"  // pulls in all the necessary headers

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

#include "Gamma/DFT.h"         // STFT
#include "Gamma/Envelope.h"    //Env<2>
#include "Gamma/Oscillator.h"  // Sine<>
#include "al/app/al_DistributedApp.hpp"
#include "al/math/al_Random.hpp"
#include "al/scene/al_DistributedScene.hpp"

using namespace al;

#include <fstream>
#include <iostream>
#include <utility>
using namespace std;

using namespace gam;

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
  Mesh audioMesh;

  SimpleVoice() {
    registerParameters(frequency, modulation, index);

    envelope.levels(0, 1, 0);
    envelope.lengths(0.01, 0.95);

    audioMesh.primitive(Mesh::LINE_STRIP);
    for (int i = 0; i < stft.numBins(); i++) {
      audioMesh.vertex((float)i / stft.numBins(), 0);
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
          audioMesh.vertices()[i].y = log(1 + stft.bin(i).mag());
        }
      }
      io.out(0) += fm * envelope() * 0.1;
    }
    if (envelope.done()) free();  // frees this voice
  }

  virtual void onProcess(Graphics &g) override {
    g.color(amplitude);
    g.draw(audioMesh);
  }

  virtual void onTriggerOn() override {
    envelope.reset();
    frequency.set(rnd::uniform(127.0f));
    modulation.set(rnd::uniform(127.0f));
    index.set(rnd::uniform(127.0f));
  }
};

struct Axes {
  void draw(Graphics &g) {
    Mesh mesh(Mesh::LINES);
    // x axis
    mesh.vertex(0, 0, 0);
    // mesh.color(1, 1, 1);  // white
    mesh.color(1, 0, 0);
    mesh.vertex(1, 0, 0);
    mesh.color(1, 0, 0);

    // y axis
    mesh.vertex(0, 0, 0);
    // mesh.color(1, 1, 1);  // white
    mesh.color(0, 1, 0);
    mesh.vertex(0, 1, 0);
    mesh.color(0, 1, 0);

    // z axis
    mesh.vertex(0, 0, 0);
    // mesh.color(1, 1, 1);  // white
    mesh.color(0, 0, 1);
    mesh.vertex(0, 0, 1);
    mesh.color(0, 0, 1);

    g.draw(mesh);
  }
};

LSystemType MAIN_LSYS_TYPE{LSystemType::BOURKE_ALGAE_2};
const int N{9};  // number of iterations for main system

// --------------------------------------------------------------
// 0. GET THE L-SYSTEM TYPE FOR THE MAIN SYSTEM
// --------------------------------------------------------------
// This is the type of the main system.
// The main l-system will work like an interconnected root network 
// where ertain vertices in the mesh can generate branches made
// of different L-Systems.
LSystem mainSystem{TYPE_DEFS.at(MAIN_LSYS_TYPE)};


struct AlloApp : public DistributedApp {
//   Parameter timeStep{"Time Step", "", 0.33f, "", 0.01, 3.0};
//   Parameter epsilon{"Epsilon", "", 0.000000001, "", 0.0001, 0.1};
//   Parameter randomness{"Randomness", "", 0.000000001, "", 0.0, 1.0};
  // Parameter pointSize{"/pointSize", "", 1.0, 0.0, 2.0};
  Axes axes;

  std::string mainSystemString;

  void onInit() override {
    // set up GUI
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto &gui = GUIdomain->newGUI();
    // gui.add(timeStep);
    // gui.add(epsilon);
    // gui.add(randomness);
  }

  void onCreate() override {
    // --------------------------------------------------------------
    // 1. GENERATE THE MAIN L-SYSTEM STRING
    // --------------------------------------------------------------
    mainSystemString = generateString(mainSystem, N);  // This only generates the string which will be parsed later.
    cout << "main l-sys type: " << TYPE_NAMES.at(MAIN_LSYS_TYPE) << endl;
    cout << "iterations: " << N << endl;
    cout << "string:\n" << mainSystemString << endl;

    // Set up the synth
    scene.registerSynthClass<SimpleVoice>();
    registerDynamicScene(scene);
    scene.verbose(true);  // turns on messages about voices to the console


    nav().pos(0, 0, 10); 
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
  
  float angle = 0;
  float time = 0.f;
  float interval = 1.f;
  int index = 0;
  void onAnimate(double dt) override {
    scene.update(dt);
    // auto& v((mesh.vertices()));
    //   dt = dt * timeStep.get();
    //   time += dt;
    //   // inc = (eps > 0.1) ? inc * 0.9 : (eps < 0.00001) ? inc * 1.1 : increment.get();
    //   // eps += inc;
    //   if (time > interval) {
    //     time = 0.f;
    //     // index = (index < N) ? index + 1 : N;
    //   }
    //     angle += 0.1;
    nav().faceToward(Vec3d(0, 0, 0));
    }

  void onSound(AudioIOData &io) { scene.render(io); }


  void onDraw(Graphics &g) override {
    g.clear(0);
    g.meshColor();

    // g.rotate(angle, 0, 1, 0);
    axes.draw(g);

    // --------------------------------------------------------------
    // 2a. RENDER THE MAIN L-SYSTEM STRING (visually)
    // --------------------------------------------------------------
    // Mesh mainSystemMesh(Mesh::LINES);

    // // a. visually
    // renderLSystem(mainSystem,    // the LSystem struct for the main system
    //               mainSystemString,  // the string generated from the main system
    //               STD_RULES_DRAW,    // the draw rules for parsing the string
    //               Vec3f(0, 0, 0),    // the starting position of the main system
    //               mainSystemMesh);   // the mesh to render the main system to

    // g.draw(mainSystemMesh);

    // --------------------------------------------------------------
    // 3. GENERATE AND RENDER BRANCHES
    // --------------------------------------------------------------
//     int nLayers = 1;  // how many layers of branches to generate
//     mainSystemMesh.compress();  // compress the mesh to remove redundant vertices
//     auto roots = mainSystemMesh.vertices();
//     std::vector<LSystemType> branchTypes = {LSystemType::BOURKE_BUSH_2};  // TODO: add more types

//     // Overlay `nLayers` of branches from the main L-system's vertices
//     for (int i = 0; i < nLayers; ++i) {
//         // randomly select from a list of LSystem types
//         LSystemType lsysType = branchTypes[rnd::uniformi(branchTypes.size())];
//         LSystem lsys = TYPE_DEF[lsysType];  // get the LSystem struct for the selected type
//         lsys.setRenderActions(&STD_RULES_DRAW);

//         float probability = 0.667;  // hardcoding for now
//         // min/max number of iterations for the branch L-system
//         std::pair iterRange = {1, N * 3};  // hardcoding for now
//         generateAndRenderBranches(roots, lsys, probability, iterRange);
//     }
    scene.render(g);  // Render graphics
  }
  DistributedScene scene{TimeMasterMode::TIME_MASTER_CPU};
};

int main() {
  AlloApp app;
  app.configureAudio(48000, 512, 2, 0);
  Domain::master().spu(
      app.audioIO().framesPerSecond());  // tell Gamma the playback rate
  app.start();
}
