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

const int N = 7;  // number of iterations for main system

// --------------------------------------------------------------
// 0. GET THE L-SYSTEM TYPE FOR THE MAIN SYSTEM
// --------------------------------------------------------------
// The main l-system will work like an interconnected root network 
// where ertain vertices in the mesh can generate branches made of 
// different L-Systems.
LSystem mainSystemType = TYPE_DEF[LSystemType::BOURKE_ALGAE_2];  // returns an initialized `LSystem` struct for the given type

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

struct AlloApp : App {
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
    // 1. GENERATE THE MAIN L-SYSTEM STRING
    // This only generates the string which will be parsed later.
    mainSystemString = generateString(mainSystemType, N);
    cout << "main l-sys type:\n" << mainSystemString << endl;

    nav().pos(0, 0, 10); 
  }

  bool onKeyDown(const Keyboard &k) override {
    if (k.key() == ' ') {
        // SONIFY THE SELECTED L-SYSTEM
    }
    return true;
  }
  
  float angle = 0;
  float time = 0.f;
  float interval = 1.f;
  int index = 0;
  void onAnimate(double dt) override {
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

  void onDraw(Graphics &g) override {
    g.clear(0);
    g.meshColor();

    // g.rotate(angle, 0, 1, 0);
    axes.draw(g);

    // --------------------------------------------------------------
    // 2. RENDER THE MAIN L-SYSTEM STRING
    // --------------------------------------------------------------
    Mesh mainSystemMesh(Mesh::LINES);

    // a. visually
    renderLSystem(mainSystemType,    // the LSystem struct for the main system
                  mainSystemString,  // the string generated from the main system
                  STD_RULES_DRAW,    // the render rules for parsing the string
                  Vec3f(0, 0, 0),    // the starting position of the main system
                  mainSystemMesh);   // the mesh to render the main system to

    g.draw(mainSystemMesh);

    // b. sonically
    renderLSystem(mainSystemType,    // the LSystem struct for the main system
                  mainSystemString,  // the string generated from the main system
                  STD_RULES_FM,      // the sonification rules for parsing the string
                  Vec3f(0, 0, 0),    // the starting position of the main system
                  mainSystemMesh);   // the mesh to render the main system to

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
  }
};

int main() {
  AlloApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
