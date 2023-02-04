/*
Allocore Example: Flocking

Description:
This is an example implementation of a flocking algorithm. The original flocking
algorithm [1] consists of three main interactions between flockmates ("boids"):

    1) Collision avoidance (of nearby flockmates)
    2) Velocity matching (of nearby flockmates)
    3) Flock centering (of nearby flockmates)

Here, we implement 1) and 2) only. Another change from the reference source is
the use of Gaussian functions rather than inverse-squared functions for
calculating the "nearness" of flockmates. This is done primarily to avoid
infinities, but also to give smoother motions. Lastly, we give each boid a
random walk motion which helps both dissolve and redirect the flocks.

[1] Reynolds, C. W. (1987). Flocks, herds, and schools: A distributed behavioral
    model. Computer Graphics, 21(4):25–34.

Author:
Lance Putnam, Oct. 2014
*/

#include <cmath>

#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/math/al_Functions.hpp"
#include "al/math/al_Random.hpp"

#include "al/app/al_GUIDomain.hpp"
#include "al/graphics/al_Image.hpp"
#include "al/io/al_File.hpp"

const int MAX_BOIDS = 34;
const int nPred = 2;   // relative level of predator boids
const int nPrey = 32;  // relative level of prey boids

using namespace al;

// A "boid" (play on bird) is one member of a flock.
class Boid {
 public:
  // Each boid has a position and velocity.
  Vec2d pos, vel;

  // Type of boid
  int type;  // 0 = prey, 1 = predator

  // urge to persue food [0,1]
  // prey purse food particles, predators persue prey
  float hunger;

  float age = 0.0f;

  // Update position based on velocity and delta time
  void update(double dt) { pos += vel * dt; }
};

struct MyApp : public App {
  Parameter timeStep{"time-step", "", 1.0f, 0.01f, 100.0f};

  // static const int Nb = 32;  // Number of boids
  Boid boids[nPred + nPrey];
  Mesh heads, tails;
  Mesh box;

  // function to scale relative levels to a probability distribution
  std::vector<int> scale_to_prob_dist(float pPred, float pPrey) {
    float total = pPred + pPrey;
    float scale = MAX_BOIDS / total;
    int numPred = pPred * scale;
    int numPrey = pPrey * scale;
    std::vector<int> bCounts = {numPred, numPrey};
    return bCounts;
  }

  void onCreate() {
    box.primitive(Mesh::LINE_LOOP);
    box.vertex(-1, -1);
    box.vertex(1, -1);
    box.vertex(1, 1);
    box.vertex(-1, 1);
    nav().pullBack(4);

    resetBoids();
  }

  // Randomize boid positions/velocities uniformly inside unit disc
  void resetBoids() {
    for (auto& b : boids) {
      b.pos = rnd::ball<Vec2f>();
      b.vel = rnd::ball<Vec2f>();
      b.type = rnd::uniformi(0, 1);
      b.hunger = rnd::uniform(0, 1);
    }
  }

  void onAnimate(double dt_ms) {
    double dt = dt_ms * timeStep.get();

    // Compute boid-boid interactions
    for (int i = 0; i < MAX_BOIDS - 1; ++i) {
      for (int j = i + 1; j < MAX_BOIDS; ++j) {
        // printf("checking boids %d and %d\n", i,j);

        auto ds = boids[i].pos - boids[j].pos;
        auto dist = ds.mag();

        // Collision avoidance
        double pushRadius = 0.05;
        double pushStrength = 1;
        double push = exp(-al::pow2(dist / pushRadius)) * pushStrength;

        auto pushVector = ds.normalized() * push;
        boids[i].pos += pushVector;
        boids[j].pos -= pushVector;

        // Velocity matching
        double matchRadius = 0.125;
        double nearness = exp(-al::pow2(dist / matchRadius));
        Vec2d veli = boids[i].vel;
        Vec2d velj = boids[j].vel;

        // Take a weighted average of velocities according to nearness
        boids[i].vel = veli * (1 - 0.5 * nearness) + velj * (0.5 * nearness);
        boids[j].vel = velj * (1 - 0.5 * nearness) + veli * (0.5 * nearness);

        // TODO: Flock centering
      }
    }

    // Update boid independent behaviors
    for (auto& b : boids) {
      // Random "hunting" motion
      // Use cubed distribution to make small jumps more frequent
      auto hunt = rnd::ball<Vec2f>() * b.hunger;
      hunt *= hunt.magSqr();
      b.vel += hunt;

      // Bound boid into a box
      if (b.pos.x > 1 || b.pos.x < -1) {
        b.pos.x = b.pos.x > 0 ? 1 : -1;
        b.vel.x = -b.vel.x;
      }
      if (b.pos.y > 1 || b.pos.y < -1) {
        b.pos.y = b.pos.y > 0 ? 1 : -1;
        b.vel.y = -b.vel.y;
      }
    }

    // Generate meshes
    heads.reset();
    heads.primitive(Mesh::POINTS);

    tails.reset();
    tails.primitive(Mesh::LINES);

    for (size_t i = 0; i < MAX_BOIDS; ++i) {
      boids[i].update(dt);

      heads.vertex(boids[i].pos);
      heads.color(
        (boids[i].type == 0) ? HSV(0.6, 1, 1) : HSV(0, 1, 1)
      );

      tails.vertex(boids[i].pos);
      tails.vertex(boids[i].pos - boids[i].vel.normalized(0.07));

      tails.color(heads.colors()[i]);
      tails.color(RGB(0.5));
    }
  }

  void onDraw(Graphics& g) {
    g.clear(0);
    gl::depthTesting(true);
    gl::pointSize(8);
    // g.nicest();
    // g.stroke(8);
    g.meshColor();
    g.draw(heads);
    g.draw(tails);

    // g.stroke(1);
    g.color(1);
    g.draw(box);
  }

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(timeStep);
  }

  bool onKeyDown(const Keyboard& k) {
    switch (k.key()) {
      case 'r':
        resetBoids();
        break;
    }
    return true;
  }
};

int main() {
  MyApp().start();
  return 0;
}
