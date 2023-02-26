// Karl Yerkes
// 2022-01-20

#include "l-system.hpp"

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

#include <fstream>
#include <vector>
#include <iostream>
using namespace std;

const int N = 7;  // number of iterations

/*
Example 2: 
  Fractal (binary) tree

  variables : 0, 1
  constants: “[”, “]”
  axiom  : 0
  rules  : (1 → 11), (0 → 1[0]0)

  0: draw a line segment ending in a leaf
  1: draw a line segment
  [: push position and angle, turn left 45 degrees
  ]: pop position and angle, turn right 45 degrees


  The push and pop refer to a LIFO stack (more 
  technical grammar would have separate symbols 
  for "push position" and "turn left"). 
  
  When the turtle interpretation encounters a '[', 
  the current position and angle are saved, and are 
  then restored when the interpretation encounters 
  a ']'. 
  
  If multiple values have been "pushed," then a 
  "pop" restores the most recently saved values. 
*/
LSystem tree = {
    '0',             // axiom
    {
        {'0', "1[0]0"}, // rules
        {'1', "11"},
    },
    {'0', '1'},      // variables
    {'[', ']'}       // constants
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

void drawBranch(Mesh &mesh, const std::vector<Vec3f> &points, const std::vector<RGB> &colors, const Vec3f &startPoint, const RGB &startColor) {
    Vec3f currentPoint = startPoint;
    Color currentColor = startColor;

    for (int i = 0; i < points.size(); i++) {

        Vec3f nextPoint = currentPoint + points[i];
        Color nextColor = colors[i];
        drawLine(mesh, currentPoint, nextPoint, currentColor, nextColor);
        currentPoint = nextPoint;
        currentColor = nextColor;
    }
}

struct AlloApp : App {
  Parameter timeStep{"Time Step", "", 0.33f, "", 0.01, 3.0};
  Parameter epsilon{"Epsilon", "", 0.000000001, "", 0.0001, 0.1};
  Parameter randomness{"Randomness", "", 0.000000001, "", 0.0, 1.0};
  // Parameter pointSize{"/pointSize", "", 1.0, 0.0, 2.0};

  Axes axes;

  std::string result;

  void onInit() override {
    // set up GUI
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto &gui = GUIdomain->newGUI();
    gui.add(timeStep);
    gui.add(epsilon);
    gui.add(randomness);
  }

  void onCreate() override { 
    // state.push_back(State{Vec3f(0, 0, 0), RGB(1, 1, 1)});
    result = generateString(tree, N);
    cout << "l-sys tree:\n" << result << endl;
    // evaluate(tree, N);
    nav().pos(0, 0, 10); 
  }

  void evaluate(const std::string &str, Mesh &mesh) {
    struct State {
      Vec3f pos;  // position
      // Quatf rot;  // orientation
      RGB color;  // color
    };
    std::vector<State> state;  // push_back / pop_back
    
    Vec3f currentPoint(Vec3f(0, 0, 0));
    RGB currentColor(RGB(1, 1, 1));
    Vec3f nextPoint;
    Color nextColor;
    mesh.vertex(currentPoint);
    mesh.color(currentColor);

    state.push_back(State{currentPoint, currentColor});
    for (char c : str) {
      // currentPoint = state.back().pos;
      // currentColor = state.back().color;
      if (c == '0') {
        nextPoint = currentPoint + Vec3f(0.1*r(), 0.75*r(), 0.1*r())*0.667;
        nextColor = RGB(0, 1, 0);
        // cout << "0" << endl;
      } else if (c == '1') {
        nextPoint = currentPoint + Vec3f(0.25*r(), 0.1*r(), 0.5*r())*0.5;
        nextColor = RGB(0.5, 0.25, 0);
        // cout << "1" << endl;
      } else if (c == '[') {
        state.push_back(State{currentPoint, currentColor});
        // root = mesh.vertices().back();
      } else if (c == ']') {
        state.pop_back();
      }
      drawLine(mesh, currentPoint, nextPoint, currentColor, nextColor);
      currentPoint = nextPoint;
      currentColor = nextColor;
    }
  }

  bool onKeyDown(const Keyboard &k) override {
    if (k.key() == '1') {
      // result = generateString(lsys, n);
      // evaluate(tree, N);
      // index = 0;
    }
    return true;
  }
  
  float angle = 0;
  float time = 0.f;
  float interval = 1.f;
  int index = 0;
  void onAnimate(double dt) override {
      dt = dt * timeStep.get();
      time += dt;
      // inc = (eps > 0.1) ? inc * 0.9 : (eps < 0.00001) ? inc * 1.1 : increment.get();
      // eps += inc;
      if (time > interval) {
        time = 0.f;
        // index = (index < N) ? index + 1 : N;
      }
        angle += 0.1;
        nav().faceToward(Vec3d(0, 0, 0));
    }

  void onDraw(Graphics &g) override {
    g.clear(0);
    g.meshColor();

    g.rotate(angle, 0, 1, 0);
    axes.draw(g);

    Mesh mesh(Mesh::LINES);
    // evaluate(result, mesh);

    Vec3f currentPoint(Vec3f(0, 0, 0));
    RGB currentColor(RGB(1, 1, 1));

    mesh.vertex(currentPoint);
    mesh.color(currentColor);

    // drawLine(mesh, currentPoint, Vec3f(0, 1, 0), currentColor, RGB(1, 0, 0));
    // drawLine(mesh, Vec3f(0, 1, 0), Vec3f(0, 1, 1), RGB(1, 0, 0), RGB(0, 1, 0));
    // drawLine(mesh, Vec3f(0, 1, 1), Vec3f(0, 2, 1), RGB(0, 1, 0), RGB(0, 0, 1));
    // drawLine(mesh, Vec3f(0, 2, 1), Vec3f(0, 2, 2), RGB(0, 0, 1), RGB(1, 1, 1));

    evaluate(result, mesh);
    // cout << mesh.vertices().size() << endl;

    // cout << index << endl;
    // for (int i = 1; i < index; i++) {
    //     // flat scalar = i / index;
    // }
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