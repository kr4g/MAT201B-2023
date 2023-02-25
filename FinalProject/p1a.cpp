// Karl Yerkes
// 2022-01-20

/*
  PRESS '1' TO GENERATE A NEW VARIATION
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

struct AlloApp : App {
  // Parameter pointSize{"/pointSize", "", 1.0, 0.0, 2.0};

  Axes axes;

  const int N = 7;


  std::vector<State> state;  // push_back / pop_back

  // Mesh mesh;//(al::Mesh::LINES);

  std::vector<Vec3f> points;
  std::vector<RGB> colors;

  void onInit() override {
    // set up GUI
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto &gui = GUIdomain->newGUI();
  }

  void onCreate() override { 
    state.push_back(State{Vec3f(0, 0, 0), RGB(1, 1, 1)});
    evaluate(algae, N);
    nav().pos(0, 0, 50); 
  }

  void evaluate(const LSystem &lsys, int n) {
    points.clear();
    colors.clear();
    std::string result = generateString(lsys, n);
    // int count = 1;
    for (char c : result) {
      float d = rnd::uniformS();
      if (c == 'A') {
        // float s = 
        points.push_back(Vec3f(rnd::uniform(0.33, 1.0), rnd::uniform(0.1667, 0.33)*r(), rnd::uniform(0.1667, 0.33)*r()));
        colors.push_back(RGB(0, 1.f, 0.1f));
      } else if (c == 'B') {
        points.push_back(Vec3f(rnd::uniform(0.1667, 0.667), rnd::uniform(0.25, 1.667)*r(), rnd::uniform(0.25, 1.667)*r()));
        colors.push_back(RGB(1.f, 0, 0.25f));
      }
    }
  }

  bool onKeyDown(const Keyboard &k) override {
    if (k.key() == '1') {
      evaluate(algae, N);
      index = 0;
    }
    return true;
  }
  
  float angle = 0;
  float time = 0.f;
  float interval = 1.f;
  int index = 0;
  void onAnimate(double dt) override {
    // time += dt;
    // if (time > interval) {
    //   time = 0.f;
    //   index = (index < N) ? index + 1 : N;
    // }
      // angle += 0.1;
      // nav().faceToward(Vec3d(0, 0, 0));
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
    drawBranch(mesh, points, colors, currentPoint, currentColor);
    // drawLine(mesh, state.back().pos, Vec3f(1, 0, 0), state.back().color, RGB(0, rnd::uniform(), 0));

    g.draw(mesh);
  }
};

int main() {
  AlloApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}