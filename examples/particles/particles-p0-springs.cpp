// Karl Yerkes
// 2022-01-20

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

Vec3f randomVec3f(float scale) {
  return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
}
string slurp(string fileName);  // forward declaration

struct AlloApp : App {
  Parameter pointSize{"/pointSize", "", 1.0, 0.0, 2.0};
  Parameter timeStep{"/timeStep", "", 0.1, 0.01, 0.6};
  Parameter dragFactor{"/dragFactor", "", 0.5, 0.0, 1.0};

  ShaderProgram pointShader;

  //  simulation state
  Mesh mesh;  // position *is inside the mesh* mesh.vertices() are the positions
  vector<Vec3f> velocity;
  vector<Vec3f> acceleration;
  vector<float> mass;

  void onInit() override {
    // set up GUI
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto &gui = GUIdomain->newGUI();
    gui.add(pointSize);   // add parameter to GUI
    gui.add(timeStep);    // add parameter to GUI
    gui.add(dragFactor);  // add parameter to GUI
  }

  void onCreate() override {
    navControl().useMouse(false);

    // compile shaders
    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    // set initial conditions of the simulation
    //

    // c++11 "lambda" function
    auto randomColor = []() { return HSV(rnd::uniform(), 1.0f, 1.0f); };

    mesh.primitive(Mesh::POINTS);
    // does 1000 work on your system? how many can you make before you get a low
    // frame rate? do you need to use <1000?
    for (int _ = 0; _ < 1000; _++) {
      mesh.vertex(randomVec3f(5));
      mesh.color(randomColor());

      // float m = rnd::uniform(3.0, 0.5);
      float m = 3 + rnd::normal() / 2;
      if (m < 0.5) m = 0.5;
      mass.push_back(m);

      // using a simplified volume/size relationship
      mesh.texCoord(pow(m, 1.0f / 3), 0);  // s, t

      // separate state arrays
      velocity.push_back(randomVec3f(0.1));
      acceleration.push_back(randomVec3f(1));
    }

    nav().pos(0, 0, 10);
  }

  bool freeze = false;
  void onAnimate(double dt) override {
    if (freeze) return;

    // ignore the real dt and set the time step;
    dt = timeStep;

    // Calculate forces

    // XXX you put code here that calculates gravitational forces
    // These are pair-wise. Each unique pairing of two particles
    // These are equal but opposite: A exerts a force on B while B exerts that
    // same amount of force on A (but in the opposite direction!) Use a nested
    // for loop to visit each pair once The time complexity is O(n*n)
    //

    float k = 50;
    for (int i = 0; i < velocity.size(); i++) {
      Vec3f Xe = mesh.vertices()[i];
      // spring likes to be 3 long and in the same dir as the point
      Vec3f Xr = Xe / Xe.mag() * 3;
      Vec3f F = -k * (Xe - Xr);
      acceleration[i] += F / mass[i];
    }

    // drag
    for (int i = 0; i < velocity.size(); i++) {
      acceleration[i] -= velocity[i] * dragFactor;
    }

    // Integration
    //
    vector<Vec3f> &position(mesh.vertices());
    for (int i = 0; i < velocity.size(); i++) {
      // "semi-implicit" Euler integration
      velocity[i] += acceleration[i] / mass[i] * dt;
      position[i] += velocity[i] * dt;
    }

    // clear all accelerations (IMPORTANT!!)
    for (auto &a : acceleration) a.zero();
  }

  bool onKeyDown(const Keyboard &k) override {
    if (k.key() == ' ') {
      freeze = !freeze;
    }

    if (k.key() == '1') {
      // introduce some "random" forces
      for (int i = 0; i < velocity.size(); i++) {
        // F = ma
        acceleration[i] = randomVec3f(1) / mass[i];
      }
    }

    return true;
  }

  void onDraw(Graphics &g) override {
    g.clear(0.3);
    g.shader(pointShader);
    g.shader().uniform("pointSize", pointSize / 100);
    g.blending(true);
    g.blendTrans();
    g.depthTesting(true);
    g.draw(mesh);
  }
};

int main() {
  AlloApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}

string slurp(string fileName) {
  fstream file(fileName);
  string returnValue = "";
  while (file.good()) {
    string line;
    getline(file, line);
    returnValue += line + "\n";
  }
  return returnValue;
}