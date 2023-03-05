// Karl Yerkes
// 2022-01-20

// #include "l-system.hpp"
#include "backend.hpp"

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
#include <vector>
#include <iostream>
using namespace std;

const int N = 7;  // number of iterations for main system

LSystem mainSystemType = TYPE_DEF[LSystemType::BOURKE_ALGAE_2];

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
    // The main l-system works like a root network
    mainSystemString = generateString(mainSystemType, N);
    cout << "main l-sys type:\n" << mainSystemString << endl;

    nav().pos(0, 0, 10); 
  }

  void drawLSystem(const std::string &str, const al::Vec3f &startPoint, float decay, al::Mesh &mesh) {

    // float decayRate = 0.9;  // *= 0.9 each iteration so that the draw actions become smaller and smaller

    struct State : al::Pose {
        al::Color color;

        State(const al::Vec3f& position, const al::Color& color)
            : al::Pose(position), color(color)
        {}
    };

    std::vector<State> state;  // push_back / pop_back
    // std::vector<al::Pose> state;  // push_back / pop_back
    
    state.push_back(State{startPoint, al::Color(1, 1, 1)});
    // state.push_back(al::Pose(startPoint));
    // cout << "origin: " << state.back().pos() << endl;
    // al::Pose& p(state.back());

    // start the l-system at its origin
    mesh.vertex(state.back().pos());
    // mesh.color(state.back().color);
    mesh.color(al::Color(1, 1, 1));

    // stack.back(); // top
    // stack.pop_back(); // pop
    // stack.push_back(stack.back()); // push a copy of top

    // mesh.vertices(); // ...is the list of redundant vertices
    // mesh.compress(); // vertices + indices represetnation
    // mesh.vertices(); // ...is the list of unique vertices
    // mesh.indices();  // ...is the list of connectedness of vertices

    for (char c : str) {
        if (c == '0') {  // draw a line segment ending in a leaf
            mesh.vertex(state.back().pos());
            state.back().pos() += state.back().uf(); // uf means "unit forward", the vector that points front
            mesh.vertex(state.back().pos());
        } else if (c == '1') {  // draw a line segment
            // cout << "branch: " << p.pos() << endl;
            mesh.vertex(state.back().pos());
            state.back().pos() += state.back().uf(); // uf means "unit forward", the vector that points front
            mesh.vertex(state.back().pos());
        } else if (c == '[') {  // CHANGE CURRENT BRANCH
            // Push current state onto stack
            state.push_back(state.back());
        } else if (c == ']') {  // RESTORE PREVIOUS BRANCH
            // Pop previous state from stack
            state.pop_back();
        }
    }
  }

  void generateBranches(std::vector<al::Vec3f> &vertices, const LSystem &lsys, float probability, int n) {
    // mesh.compress(); // vertices + indices represetnation
    // mesh.vertices(); // ...is the list of unique vertices
    // mesh.indices();  // ...is the list of connectedness of vertices
    // mesh.compress();
    for (auto vert: vertices) {
        if (probability > rnd::uniform()) {  // randomly generate branches
            Mesh m(Mesh::LINES);
            drawLSystem(generateString(lsys, n), vert, 0.9, m);
        }
        // cout << vert << endl;
        // mesh.vertex(vert);
        // mesh.color(1, 1, 1);
    }
  }

  bool onKeyDown(const Keyboard &k) override {
    if (k.key() == '1') {
      // mainSystemString = generateString(lsys, n);
      // drawLSystem(mainSystemType, N);
      // index = 0;
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

    Mesh mainSystemMesh(Mesh::LINES);

    drawLSystem(mainSystemString, Vec3f(0, 0, 0), mainSystemMesh);
    g.draw(mainSystemMesh);

    mainSystemMesh.compress();

    int nLayers = 1;
    auto roots = mainSystemMesh.vertices();
    std::vector<LSystemType> branchTypes = {LSystemType::TREE};  // TODO: add more types

    // Generate `nLayers` of branches
    for (int i = 0; i < nLayers; ++i) {
        // randomly select from a list of LSystem types
        auto lsystemType = branchTypes[rnd::uniformi(branchTypes.size())];
        float probability = 0.667;  // hardcoding for now
        int iterations = N * 3;     // hardcoding for now
        generateBranches(roots, TYPE_DEF[lsystemType], probability, iterations);
    }
  }
};

int main() {
  AlloApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
