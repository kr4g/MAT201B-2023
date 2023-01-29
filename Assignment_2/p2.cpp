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

#include<algorithm>
#include <iostream>
#include <vector>
#include <numeric> // for std::accumulate

const int MAX_PARTICLES = 1000;

// // const std::string imageFilename = "mpv-shot0005.jpg";
// const std::string imageFilename = "multi-color-texture-threads-5k-i4.jpg";
// const std::string filePath = File::currentPath() + "../images/" + imageFilename;

Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }
RGB c() { return RGB(rnd::uniform(), rnd::uniform(), rnd::uniform()); }

// from class code review 1/24/2023
namespace Common {
    constexpr float G = 6.67430E-11; // gravitational constant
    constexpr float F_G(float m1, float m2, float r, float g) {
        return ((g) ? g : 1.0) * G * m1 * m2 / (r * r * (float)1e-4);  // gForce
    };
};

struct AnApp : App {
  Parameter sunMassScale{"sun-mass-scale", "", 1.0f, 1.0f, 10.0f};
  Parameter maxForce{"max-force", "", 1.0f, 0.0f, 0.1f};
  Parameter drag{"drag", "", 0.001f, 0.0f, 0.05f};
  Parameter forceDiffuseScalar{"force-diffuse-scalar", "", 0.1f, 0.0f, 0.9f};
  Parameter gravScale{"grav-scale", "", 1.0f, -50.0f, 50.0f};  // TODO: make cubic
  Parameter timeStep{"time-step", "", 1.0f, 0.0f, 100.0f};
  Mesh position{Mesh::POINTS};
  std::vector<Vec3f> velocity;
  std::vector<double> mass;
  std::vector<Vec3f> gForce;
  std::vector<double> age;

  // function to scale relative levels to a probability distribution
  std::vector<int> scale_to_prob_dist(int numSuns, float pPlanets, float pMoons, float pAsteroids) {
    float total = numSuns + pPlanets + pMoons + pAsteroids;
    float scale = MAX_PARTICLES / total;
    int numPlanets = pPlanets * scale;
    int numMoons = pMoons * scale;
    int numAsteroids = pAsteroids * scale;
    int remaining = MAX_PARTICLES - numSuns - numPlanets - numMoons - numAsteroids;
    numAsteroids += remaining;
    std::vector<int> particleCounts = {numSuns, numPlanets, numMoons, numAsteroids};
    return particleCounts;
  }

  void generate_particles(int num_particles, std::string particle_type, double massVal, Vec3f vertex, Vec3f vel, Color color, float randScale = 0.f) {
    cout << endl << "Generating " << num_particles << " " << particle_type << "..." << endl;
    for (int i = 0; i < num_particles; ++i) {
      position.vertex(vertex + (r() * randScale));
      position.color(color);
      velocity.push_back(vel * (rnd::uniformS() * randScale));
      mass.push_back(massVal + (rnd::uniform() * randScale));
      gForce.push_back(Vec3f(0.0f, 0.0f, 0.0f));
      age.push_back(0.0);
      cout << "Particle " << i + position.vertices().size() - 1 << " vertex: " << vertex << endl;
    }
}

  void setParticles (int maxParticles, int numSuns, float pPlanets, float pMoons, float pAsteroids) {
    std::vector<std::string> labels { "Suns", "Planets", "Moons", "Asteroids" };
    std::vector<int> particles_per_category = scale_to_prob_dist(numSuns, pPlanets, pMoons, pAsteroids);
    for (int i = 0; i < labels.size(); ++i) {
      cout << labels[i] << ": " << particles_per_category[i] << endl;
    }

    // Sun(s)
    generate_particles(particles_per_category[0], "Suns", 2e-12, Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f), Color(1.0, 1.0, 0.0, 1.0), 0.0);

    // Planets
    generate_particles(particles_per_category[1], "Planets", 1e-10, position.vertices()[0], Vec3f(-1e-6, 1e-6, 0), Color(0.0, 0.3, 0.7, 1.0), 1.3);

    // Moons
    // todo:  generate starting from random planet positions
    generate_particles(particles_per_category[2], "Moons", 1e-6, position.vertices()[0], Vec3f(-1e-6, 1e-6, 0), Color(0.0, 0.0, 0.8, 0.67), 1.67);

    // Asteroids
    generate_particles(particles_per_category[3], "Asteroids", 5 + rnd::uniformS(), r(), r() * 1e-10, Color(255.0/255.0, 248.0/255.0, 220.0/255.0, 0.3), 3.75);

    cout << endl << position.vertices().size() << " particles generated." << endl;
  }


  void resetParticles(int maxParticles) {
    position.vertices().clear();
    velocity.clear();
    mass.clear();
    gForce.clear();

    setParticles(maxParticles, 1, 0.1f, 0.1f*1.67f, (0.1f+0.1f*1.67f)*9.3f);
  }

  void onCreate() override {
    // set up the particles
    setParticles(MAX_PARTICLES, 1, 0.1f, 0.1f*1.67f, (0.1f+0.1f*1.67f)*9.3f);
    nav().pos(0, 0, 25);
  }

  void onAnimate(double dt) override {
    // cout << "dt: " << dt * timeStep.get() << endl;
    // cout << "vel: " << velocity.size() << endl;
    mass[0] *= sunMassScale.get();  // scale the mass of the sun
    // calculate the force of gravity...
    // for each particle, calculate the distance and force of gravity 
    // between it and every other particle except itself.
    for (int i = 0; i < velocity.size(); ++i) {
      // cout << "mass[" << i << "]: " << mass[i] << endl;
      for (int j = 0; j < velocity.size(); ++j) {
        // cout << "-> mass[" << j << "]: " << mass[j] << endl;
        if (i != j) {
          float maxF = maxForce.get();
          Vec3f r = position.vertices()[j] - position.vertices()[i];
          Vec3f F = min(Common::F_G(mass[i], mass[j], r.mag(), gravScale.get()), maxF) * r.normalize();
          gForce[i] += (float)dt * (F / 1.f) * 1.f;
          gForce[j] -= (float)dt * (F / 1.f) * 1.f;
        }
      }
      if (i != 0 ) {
        position.vertices()[i] += velocity[i] * dt * timeStep.get();
      }
      // semi-implicit Euler integration
      // apply the force of gravity to the velocity
      // cout << "i: " << i << endl;
      velocity[i] += -velocity[i] * drag.get() + gForce[i];
      gForce[i] = gForce[i] * forceDiffuseScalar.get();
    }
    nav().faceToward(Vec3f(0.f, 0.f, 0.f));
  }

  bool onKeyDown(Keyboard const& k) override {
    switch (k.key()) {
      case ' ':
        // reset particles
        resetParticles(MAX_PARTICLES);
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
    gui.add(sunMassScale);
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
