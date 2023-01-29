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
#include "al/graphics/al_Shapes.hpp"

using namespace al;
using namespace std;

#include<algorithm>
#include <iostream>
#include <vector>
#include <numeric> // for std::accumulate

const int MAX_PARTICLES = 100;
int numSuns = 1;
float pPlanets = 0.9, pMoons = 1.67, pAsteroids = 3.54;

// // const std::string imageFilename = "mpv-shot0005.jpg";
// const std::string imageFilename = "multi-color-texture-threads-5k-i4.jpg";
// const std::string filePath = File::currentPath() + "../images/" + imageFilename;

Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }
RGB c() { return RGB(rnd::uniform(), rnd::uniform(), rnd::uniform()); }

// from class code review 1/24/2023
namespace Common {
    constexpr float G = 6.67430E-11; // gravitational constant
    constexpr float F_G(float m1, float m2, float r, float g) {
        return ((abs(g) > 0) ? g : 1.0) * G * m1 * m2 / (r * r * (float)1e-4);  // gForce
    };
};

struct AnApp : App {
  Parameter sunMassScale{"sun-mass-scale", "", 1.018f, 1.0f, 1.33f};
  Parameter maxForce{"max-force", "", 1.0f, 0.0f, 0.1f};
  Parameter drag{"drag", "", 0.02f, 0.0f, 0.1f};
  Parameter gravScale{"grav-scale", "", 22.7400f, -1000.0f, 1000.0f};  // TODO: make cubic
  Parameter forceDiffuseScalar{"force-diffuse-scalar", "", 0.1f, 0.0f, 0.9f};
  ParameterBool isAsymmetrical{"asymmetric-force", "", 1.f, 0, 1.0f};
  Parameter timeStep{"time-step", "", 10.0f, 0.0001f, 1000.0f};
  ParameterBool rotate{"rotate", "", 1.f, 0, 1.0f};
  Mesh position{Mesh::POINTS};
  Mesh sphere;
  std::vector<Vec3f> velocity;
  std::vector<double> mass;
  std::vector<double> radius;
  std::vector<Vec3f> gForce;
  std::vector<double> dragScale;
  std::vector<double> age;

  double time = 0.0;

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

void generate_particles(int num_particles, std::string particle_type, double massVal, double rad, Vec3f vertex, Vec3f vel, Color color, float randScale = 0.f) {
    for (int i = 0; i < num_particles; ++i) {
        bool collision = position.vertices().size() > 0;
        Vec3f newVert = vertex;
        while (collision) {
            collision = false;
            newVert = vertex + r();
            for (int j = 0; j < position.vertices().size() - 1; ++j) {
                if ((position.vertices()[j] - newVert).mag() < radius[j] + rad) {
                    // cout << "j: " << j << endl;
                    collision = true;
                    break;
                }
            }
        }
        position.vertex(newVert);
        position.color(color + (c() * randScale * 0.5));
        velocity.push_back(vel + (rnd::uniformS() * randScale * 0.1));
        mass.push_back(massVal + (rnd::uniform() * randScale * 0.01));
        radius.push_back(rad + (rnd::uniform() * randScale * 0.1));
        gForce.push_back(Vec3f(0.0f, 0.0f, 0.0f));
        dragScale.push_back(1.0f);
        age.push_back(0.0);
        // cout << "Particle " << i << " of type " << particle_type << " created." << endl;
    }
}

  void removeParticle(int i) {
    position.vertices().erase(position.vertices().begin() + i);
    velocity.erase(velocity.begin() + i);
    mass.erase(mass.begin() + i);
    radius.erase(radius.begin() + i);
    gForce.erase(gForce.begin() + i);
    dragScale.erase(dragScale.begin() + i);
    age.erase(age.begin() + i);
  }

  void setParticles (int maxParticles, int numSuns, float pPlanets, float pMoons, float pAsteroids) {
    std::vector<std::string> labels { "Suns", "Planets", "Moons", "Asteroids" };
    std::vector<int> particles_per_category = scale_to_prob_dist(numSuns, pPlanets, pMoons, pAsteroids);
    cout << "---------" << endl;
    for (int i = 0; i < labels.size(); ++i) {
      cout << labels[i] << ": " << particles_per_category[i] << endl;
    }

    // Sun(s)
    generate_particles(particles_per_category[0],   // number of particles in this category
                       "Suns",                      // particle type
                       2e-12,                       // mass        
                       1.0f,                        // radius
                       Vec3f(0.0f, 0.0f, 0.0f),     // position
                       Vec3f(0.0f, 0.0f, 0.0f),     // velocity
                       Color(1.0, 1.0, 0.0, 1.0),   // color
                       0.0);                        // random multiplier

    // Planets
    generate_particles(particles_per_category[1],
                       "Planets",
                       1e-9,
                       0.3f,                        // radius
                       position.vertices()[0],
                       Vec3f(1e-6, -1e-6, 0),
                       Color(0.0, 0.3, 0.7, 1.0),
                       0.0887);

    // Moons
    // todo:  generate starting from random planet positions
    generate_particles(particles_per_category[2],
                       "Moons",
                       1e-6,
                       0.09f,                       // radius
                       position.vertices()[0],
                       Vec3f(1e-6, -1e-6, 0),
                       Color(0.0, 0.0, 0.8, 0.9),
                       0.33);

    // Asteroids
    generate_particles(particles_per_category[3],
                      "Asteroids",
                      1.f + rnd::uniform(),
                      0.01f + rnd::uniform() * 0.07f,                        // radius
                      r() * 1e-15,
                      r() * 1e-9,
                      Color(255.0/255.0, 248.0/255.0, 220.0/255.0, 0.167),
                      1.1667);

    cout << endl << position.vertices().size() << " particles generated.\n" << "---------" << endl;
  }


  void resetParticles(int maxParticles) {
    time = 0.0;
    position.vertices().clear();
    velocity.clear();
    mass.clear();
    radius.clear();
    gForce.clear();

    setParticles(maxParticles, numSuns, pPlanets, pMoons, pAsteroids);
  }

  void onCreate() override {
    // set up the particles
    setParticles(MAX_PARTICLES, numSuns, pPlanets, pMoons, pAsteroids);
    
    // addSphere(sphere, radius[0], 50, 50);
    // sphere.generateNormals();
    nav().pos(0, 0, 15);
  }

  bool isIntersecting(Vec3f point1, float radius1, Vec3f point2, float radius2) {
    float distance = sqrt(pow(point1.x - point2.x, 2) + pow(point1.y - point2.y, 2) + pow(point1.z - point2.z, 2));
    float sumRadii = radius1 + radius2;
    return distance < sumRadii;
  }

  void onAnimate(double dt) override {
    time += dt * timeStep.get();
    mass[0] *= sunMassScale.get();  // scale the mass of the sun
    // for each particle, calculate the distance and force of gravity 
    // between it and every other particle except itself.
    std::vector<int> deadParticles;
    for (int i = 0; i < velocity.size(); ++i) {
      if (i != 0 && position.vertices()[i] == position.vertices()[0]) {
      // && isIntersecting(position.vertices()[i],
      //                           radius[i],
      //                           position.vertices()[0],
      //                           radius[0])) { // if particle is at the sun...
        cout << "Particle " << i << " is at the sun!" << endl;
        // add to the sun's mass
        mass[0] += mass[i];
        // add particle to the list of particles to be removed
        deadParticles.push_back(i);
      }      
      // cout << "mass[" << i << "]: " << mass[i] << endl;
      for (int j = 0; j < velocity.size(); ++j) {
        // cout << "-> mass[" << j << "]: " << mass[j] << endl;
        if (i != j) {
          float maxF = maxForce.get();
          Vec3f r = position.vertices()[j] - position.vertices()[i];
          // calculate the force of gravity...
          Vec3f F = min(Common::F_G(mass[i], mass[j], r.mag(), gravScale.get()), maxF) * r.normalize();
          float asymF = (j > i && isAsymmetrical.get()) ? 0.5f : 1.f;
          gForce[i] += (float)dt * (F / 1.f) * 1.f * asymF;
          gForce[j] -= (float)dt * (F / 1.f) * 1.f * asymF;
        }
      }
      if (i != 0 ) {  // don't move the sun
        position.vertices()[i] += velocity[i] * dt * timeStep.get();
      } else {  // increase sun mass
        double c = sunMassScale.get();
        mass[0] *= (c >= 1.0) ? c : 1.0;
      }
      // semi-implicit Euler integration
      // apply the force of gravity to the velocity
      // cout << "i: " << i << endl;
      velocity[i] += -velocity[i] * (dragScale[i] * drag.get()) + gForce[i];
      gForce[i] = gForce[i] * forceDiffuseScalar.get();
      age[i] += dt * timeStep.get() / mass[i];  // age is proportional to mass (bigger mass = ages slower)
      float ageK = age[i] * 0.1f;
      float dragRate = (ageK < 1.0f) ? ageK : 1.0f;
      dragScale[i] *= dragRate;  // drag increases with age
    }
    
    for (int i = 0; i < deadParticles.size(); ++i) {
      // remove dead particle
      removeParticle(deadParticles[i]);
    }
    // nav().pos(0, 0 + time, 5);
    // cout << "time: " << time << ", nav.pos=" << nav().pos() << endl;
    // nav().pos(0, 0, 5);
    nav().faceToward(Vec3f(0.0f, 0.0f, 0.0f));  // face the sun
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
    gui.add(gravScale);
    gui.add(forceDiffuseScalar);
    gui.add(isAsymmetrical);
    gui.add(timeStep);
  }
};

int main() {
  AnApp app;
  app.start();
  return 0;
}
