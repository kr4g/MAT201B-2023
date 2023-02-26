// Karl Yerkes
// 2022-01-20


/*
  "ANIMATED" L-SYSTEM PLANTLIKE JELLYFISH/SQUID THING 
*/

#include "l-system.hpp"

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

#include <fstream>
#include <vector>
#include <iostream>
using namespace std;

struct State {
        Vec3f pos;  // position
        // Quatf rot;  // orientation
        RGB color;  // color
};

LSystem algae = {
    'A',             // axiom
    {
        {'A', "AB"}, // rules
        {'B', "A"},
    },
    {'A', 'B'},      // variables
    {}               // constants
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

void drawLine(Mesh &mesh, const Vec3f &start, const Vec3f &end, const RGB &startColor, const RGB &nextColor) {
    mesh.vertex(start);
    mesh.color(startColor);
    mesh.vertex(end);
    mesh.color(nextColor);
}

void drawBranch(Mesh &mesh, const std::vector<Vec3f> &pVec, const std::vector<RGB> &cVec, const Vec3f &startPoint, const RGB &startColor) {
    Vec3f currentPoint = startPoint;
    Color currentColor = startColor;

    for (int i = 0; i < pVec.size(); i++) {
        Vec3f nextPoint = currentPoint + pVec[i];
        Color nextColor = cVec[i];
        drawLine(mesh, currentPoint, nextPoint, currentColor, nextColor);
        currentPoint = nextPoint;
        currentColor = nextColor;
    }
}

// void generateBranches(int n) {
//   for (i = 0; i < n; i++) {
//     // generate a random branch
//     evaluate(algae, 7);
//   }
// }

struct AlloApp : App {
  Parameter timeStep{"Time Step", "", 0.33f, "", 0.01, 3.0};
  Parameter epsilon{"Epsilon", "", 0.000000001, "", 0.0001, 0.1};
  Parameter randomness{"Randomness", "", 0.000000001, "", 0.0, 1.0};
  // Parameter increment{"Increment", "", 0.f, "", -0.001, 0.001};
  // Parameter pointSize{"/pointSize", "", 1.0, 0.0, 2.0};

  Axes axes;

  const int N = 7;

  float angle = 0;
  float time = 0.f;
  // float interval = 0.667f;
  float interval = 0.06f;
  // double eps = 0.001;
  // double inc = increment.get();
  // double inc = 0.00001;

  std::vector<State> state;  // push_back / pop_back

  // Mesh mesh;//(al::Mesh::LINES);


  const int numBranches = 10;
  // initialize to 10 branches
  std::vector<std::vector<Vec3f>> branchPoints;
  std::vector<std::vector<RGB>> branchColors;

  void onInit() override {
    // set up GUI
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto &gui = GUIdomain->newGUI();
    gui.add(timeStep);
    gui.add(epsilon);
    gui.add(randomness);
  }

  void onCreate() override { 
    state.push_back(State{Vec3f(0, 0, 0), RGB(1, 1, 1)});
    for (int i = 0; i < numBranches; i++) {
      branchPoints.push_back(std::vector<Vec3f>());
      branchColors.push_back(std::vector<RGB>());
    }
    evaluate(algae, N);
    nav().pos(0, 0, 60); 
  }

  void evaluate(const LSystem &lsys, int n) {
    for (int i = 0; i < numBranches; i++) {
      branchPoints[i].clear();
      branchColors[i].clear();
    }
    for (int i = 0; i < numBranches; i++) {
      std::string result = generateString(lsys, n);
      float amt = 0.01f * pow(1.0 + i * epsilon.get(), i);
      // float amt = 0.01f * pow(1.79, i);
      double r_scale = randomness.get();
      int sign = 1;
      for (char c : result) {
        float d = rnd::uniformS();
        if (c == 'A') {
          // float s = 
          branchPoints[i].push_back(Vec3f(sign*0.01 + rnd::uniform(0.33, 0.75)*amt*r_scale, 0.1 + rnd::uniform(0.1667, 0.33)*r()*r_scale, 0.1 + rnd::uniform(0.1667, 0.33)*r()*r_scale));
          branchColors[i].push_back(RGB(0, rnd::uniform(0.667, 1.0), rnd::uniform(0.001, 0.1667)));
        } else if (c == 'B') {
          branchPoints[i].push_back(Vec3f(sign*0.001 + rnd::uniform(0.1667, 0.667)*amt*r_scale, 0.1 + rnd::uniform(0.25, 1.667)*r()*r_scale, 0.1 + rnd::uniform(0.25, 1.667)*r()*r_scale));
          branchColors[i].push_back(RGB(rnd::uniform(0.667, 1.0), 0, rnd::uniform(0.125, 0.33)));
        }
        amt += 0.001f;
        sign *= -1;
      }
    }
  }

  bool onKeyDown(const Keyboard &k) override {
    if (k.key() == '1') {
      evaluate(algae, N);
      // index = 0;
    }
    return true;
  }
  
  // int index = 0;
  void onAnimate(double dt) override {
    dt = dt * timeStep.get();
    time += dt;
    // inc = (eps > 0.1) ? inc * 0.9 : (eps < 0.00001) ? inc * 1.1 : increment.get();
    // eps += inc;
    if (time > interval) {
      time = 0.f;
      evaluate(algae, N);
      // index = (index < N) ? index + 1 : N;
    }
      angle += 0.1;
      nav().faceToward(Vec3d(0, 0, 0));
  }

  void onDraw(Graphics &g) override {
    g.clear(0);
    g.meshColor();

    // g.rotate(angle, 0, 1, 0);
    // axes.draw(g);

    Mesh mesh(Mesh::LINES);

    // Vec3f currentPoint(state.back().pos);
    // RGB currentColor(state.back().color);
    Vec3f currentPoint(Vec3f(0, 0, 0));
    RGB currentColor(RGB(1, 1, 1));

    // mesh.vertex(currentPoint);
    // mesh.color(currentColor);

    // cout << index << endl;
    // for (int i = 1; i < index; i++) {
    //     // flat scalar = i / index;
    // }
    // generateBranches(5);
    for (int i = 0; i < numBranches; i++) {
      drawBranch(mesh, branchPoints[i], branchColors[i], currentPoint, currentColor);
    }
    // drawBranch(mesh, points, colors, currentPoint, currentColor);
    // drawLine(mesh, state.back().pos, Vec3f(1, 0, 0), state.back().color, RGB(0, rnd::uniform(), 0));

    g.draw(mesh);
  }
};

int main() {
  AlloApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}