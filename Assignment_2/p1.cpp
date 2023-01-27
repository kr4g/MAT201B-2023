// Ryan Millett
// From starter code by Karl Yerkes
// 2023-01-19
// MAT 201B
// Starter Code for Alssignment 2
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/graphics/al_Image.hpp"
#include "al/io/al_File.hpp"
#include "al/math/al_Random.hpp"

const double G = 6.67430E-11; // gravitational constant

using namespace al;
using namespace std;

Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }
RGB c() { return RGB(rnd::uniform(), rnd::uniform(), rnd::uniform()); }

struct AnApp : App {
  // Parameter maxForce{"max-force", "", 0.01, 0.0, 1.0};
  Parameter drag{"drag", "", 0.01, 0.0, 0.1};
  Parameter gravScale{"grav-scale", "", 1.0, 0.1, 10.0};
  Parameter timeStep{"timeStep", "", 1, 0, 10};
  Mesh position{Mesh::POINTS};
  std::vector<Vec3f> velocity;
  // std::vector<Vec3f> mass;
  std::vector<Vec3f> gForce;

  void onCreate() override {
    for (int i = 0; i < 1000; i++) {
      position.vertex(r());
      position.color(c());
      velocity.push_back(r() * 0.01);
      // mass.push_back(1.0f);
      gForce.push_back(0.0f);
    }
    nav().pos(0, 0, 5);
  }

  void onAnimate(double dt) override {
    // calculate the force of gravity...
    double r = 0.0;  // distance between two particles
    // for each particle, calculate the distance to every other particle
    for (int i = 0; i < velocity.size(); ++i) {
      for (int j = 0; j < velocity.size(); ++j) {
        if (i != j) {
          r = abs(position.vertices()[i] - position.vertices()[j]);
          gForce[i] = (gravScale.get() * G) * (1.0f / (r*r));
        }
      }
    }

    // semi-implicit Euler integration
    // apply the force of gravity to the velocity
    for (int i = 0; i < velocity.size(); ++i) {
      velocity[i] += -velocity[i] * gForce[i] * drag.get() * timeStep.get() * dt;
    }
    for (int i = 0; i < position.vertices().size(); ++i) {
      position.vertices()[i] += velocity[i] * timeStep.get() * dt;
    }
  }

  bool onKeyDown(Keyboard const& k) override {
    for (int i = 0; i < position.vertices().size(); ++i) {
      position.vertices()[i] = r();
      velocity[i] = r() * 0.01;
    }
    return true;
  }

  void onDraw(Graphics& g) override {
    g.clear(0.25);
    g.pointSize(20);
    g.meshColor();
    g.draw(position);
  }

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    // gui.add(maxForce);
    gui.add(drag);
    gui.add(gravScale);
    gui.add(timeStep);
  }
};

int main() {
  AnApp app;
  app.start();
  return 0;
}
