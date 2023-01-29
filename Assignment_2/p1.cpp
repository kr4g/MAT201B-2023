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

using namespace al;
using namespace std;

Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }
RGB c() { return RGB(rnd::uniform(), rnd::uniform(), rnd::uniform()); }

// from class code review 1/24/2023
namespace Common {
    constexpr float G = 6.67430E-11; // gravitational constant
    constexpr float F_G(float m1, float m2, float r, float g) {
        return g * G * m1 * m2 / (r * r * (float)1e-4);  // gForce
    };
};

struct AnApp : App {
  Parameter maxForce{"maxForce", "", 1.0f, 0.0f, 0.1f};
  Parameter drag{"drag", "", 0.01f, 0.0f, 0.1f};
  Parameter forceDiffuseScalar{"force-diffuse-scalar", "", 0.1f, 0.0f, 0.9f};
  Parameter gravScale{"grav-scale", "", 1.0f, -100.0f, 100.0f};  // TODO: make cubic
  Parameter timeStep{"timeStep", "", 1.0f, 0.0f, 100.0f};
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
    // float dist = 0.0;  // distance between two particles
    // for each particle, calculate the distance and force of gravity 
    // between it and every other particle except itself.
    for (int i = 0; i < velocity.size(); ++i) {
      for (int j = 0; j < velocity.size(); ++j) {
        if (i != j) {
          float maxF = maxForce.get();
          Vec3f r = position.vertices()[j] - position.vertices()[i];
          Vec3f F = min(Common::F_G(1, 1, r.mag(), gravScale.get()), maxF) * r.normalize();
          gForce[i] += (float)dt * (F / 1.f) * 1.f;
          gForce[j] -= (float)dt * (F / 1.f) * 1.f;
        }
      }
    }

    // semi-implicit Euler integration
    // apply the force of gravity to the velocity
    for (int i = 0; i < velocity.size(); ++i) {
      velocity[i] += -velocity[i] * drag.get() + gForce[i] * dt * timeStep.get();
      gForce[i] = gForce[i] * forceDiffuseScalar.get();
    }
    for (int i = 0; i < position.vertices().size(); ++i) {
      position.vertices()[i] += velocity[i] * dt * timeStep.get();
    }
    nav().faceToward(Vec3f(0.f, 0.f, 0.f));
  }

  bool onKeyDown(Keyboard const& k) override {
    switch (k.key()) {
      case ' ':  // spacebar
        // randomize positions and velocities
        for (int i = 0; i < position.vertices().size(); ++i) {
          position.vertices()[i] = r();
          velocity[i] = r() * 0.01;
        }
        break;
      
      default:
        break;
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
    gui.add(maxForce);
    gui.add(drag);
    gui.add(forceDiffuseScalar);
    gui.add(gravScale);
    gui.add(timeStep);
  }
};

int main() {
  AnApp app;
  app.start();
  return 0;
}
