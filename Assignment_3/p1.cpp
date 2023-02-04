// Ryan Millett
// from starter code by Karl Yerkes
// MAT201B
// 2022-02-03
//
#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"
#include "al/math/al_Vec.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/math/al_Functions.hpp"

const int MAX_BOIDS = 1000;
const int MAX_PREDATORS = MAX_BOIDS * 0.5;
const int CUBE_SIZE = 2;
const float FOOD_RESET_RATE = 3.f;

using namespace al;

// A "boid" (play on bird) is one member of a flock.
class Boid {
 public:
  Nav bNav;  // Navigation object

  Vec3d attention;  // Where the boid is moving towards

  // Type of boid
  int type;  // 0 = prey, 1 = predator

  // urge to persue food [0,1]
  // prey purse food particles, predators persue prey
  float hunger{1.0f};
  float fear{0.0f};

  float age{0.0f};

  void setAttention(Vec3d a, double amt) { 
    attention.set(a);
    bNav.smooth(0.1);
    bNav.faceToward(attention, Vec3d(0, 1, 0), amt);
  }

  // Update position based on velocity and delta time
  void update(double v, double dt) {
    // bNav.step(dt);
      bNav.moveF(v);
      bNav.step(dt);
      age += dt;
    }

  friend std::ostream& operator<<(std::ostream& os, const Boid& b) {
    os << "Boid: \n";
    return os;
  }
};

double r() { return rnd::uniformS() * CUBE_SIZE; }

struct MyApp : App {
  
  Parameter timeStep{"Time Step", "", 1.f, "", 0.01, 10.0};
  Parameter pPredators{"Predators", "", 0.0f, 0.0f, 1.0f};
  // Nav agent;
  // Nav target;
  std::vector<Boid> boids{MAX_BOIDS};
  double time{0};
  double timeStamp{0};
  double angle{0};

  Mesh predMesh;
  Mesh preyMesh;
  Mesh foodMesh{Mesh::POINTS};

  std::vector<Vec3f> food{MAX_BOIDS * 0.5};
  // Nav point;

  void onCreate() override {
    setUp();

    // place the camera so that we can see the axes
    nav().pos(0.5, 0.7, 5);
    nav().faceToward(Vec3d(0, 0, 0), Vec3d(0, 1, 0));

    // Don't do this:
    // nav().faceToward(0, 0, 0);
    // because it will be interpreted as this:
    // nav().faceToward(Vec3d(0), Vec3d(0), 0);
    // which has no effect because of the final 0!

    // create a prototype predator body
    predMesh.primitive(Mesh::TRIANGLE_FAN);
    predMesh.vertex(0, 0, -2);
    predMesh.color(1, 0, 0);
    predMesh.vertex(0, 1, 0);
    predMesh.color(1, 0, 0);
    predMesh.vertex(-1, 0, 0);
    predMesh.color(1, 0, 0);
    predMesh.vertex(1, 0, 0);
    predMesh.color(1, 0, 0);
    predMesh.vertex(0, 1, 0);
    predMesh.color(1, 0, 0);

    // create a prototype prey body
    preyMesh.primitive(Mesh::TRIANGLE_FAN);
    preyMesh.vertex(0, 0, -2);
    preyMesh.color(0, 1, 0);
    preyMesh.vertex(0, 1, 0);
    preyMesh.color(0, 1, 0);
    preyMesh.vertex(-1, 0, 0);
    preyMesh.color(0, 1, 0);
    preyMesh.vertex(1, 0, 0);
    preyMesh.color(0, 0, 1);
    preyMesh.vertex(0, 1, 0);
    preyMesh.color(0, 0, 1);
  }
  
  void handleBounce(Boid &b, float turnRate = 0.67f) {
      if (b.bNav.pos().x > CUBE_SIZE || b.bNav.pos().x < -CUBE_SIZE) {
          b.bNav.faceToward(Vec3d(-b.bNav.pos().x, b.bNav.pos().y, b.bNav.pos().z),
                            Vec3d(0, 1, 0), turnRate);
      }
      if (b.bNav.pos().y > CUBE_SIZE || b.bNav.pos().y < -CUBE_SIZE) {
          b.bNav.faceToward(Vec3d(b.bNav.pos().x, -b.bNav.pos().y, b.bNav.pos().z),
                            Vec3d(0, 1, 0), turnRate);
      }
      if (b.bNav.pos().z > CUBE_SIZE || b.bNav.pos().z < -CUBE_SIZE) {
          b.bNav.faceToward(Vec3d(b.bNav.pos().x, b.bNav.pos().y, -b.bNav.pos().z),
                            Vec3d(0, 1, 0), turnRate);
      }
  }
  
  void findNearestFood(Boid &b, std::vector<Vec3f> &food) {
    float minDist = 1000.0f;
    Vec3f nearestFood;
    for (auto &f : food) {
      float dist = (f - b.bNav.pos()).mag();
      if (dist < CUBE_SIZE && dist < minDist) {
        minDist = dist;
        // if the food is close and the boid is not hungry...
        if (dist < 0.05 && b.attention == f && b.hunger < 0.1) {
          // ...find another food
          continue;
        }
        nearestFood = f;
      }
    }
    b.setAttention(nearestFood, 0.01);
  }
  
  // void detectNeighbors(Boid &boid) {
  //   for (auto& bd : boids) {
  //     if (&bd == &boid) { continue; }
  //     auto ds = bd.bNav.pos() - boid.bNav.pos();
  //     auto dist = ds.mag();
  //     // Collision avoidance
  //     double pushRadius = 0.05;
  //     double pushStrength = 1.0;
  //     double push = exp(-al::pow2(dist / pushRadius)) * pushStrength;
  //     auto pushVector = ds.normalized() * push;
  //     boid.bNav.nudge(boid.bNav.pos() + pushVector, 0.6);
  //     bd.bNav.nudge(bd.bNav.pos() - pushVector, 0.6);
  //     // // Velocity matching
  //     // double matchRadius = 0.125;
  //     // double nearness = exp(-al::pow2(dist / matchRadius));
  //     // Vec2d veli = boid.bNav.uf();
  //     // Vec2d velj = b.bNav.uf();
  //     // // Take a weighted average of velocities according to nearness
  //     // boid.bNav.moveF(veli.mag() * (1 - 0.5 * nearness) + velj.mag() * (0.5 * nearness));
  //     // boid.bNav.moveF(velj.mag() * (1 - 0.5 * nearness) + veli.mag() * (0.5 * nearness));
  //   }
  // }

  void randomizeFoodList() {
    for (auto& f : food) {
      f.set(r(), r(), r());
    }
  }

  void randomizeFoodParticle(Vec3f& f) {
    f.set(r(), r(), r());
  }

  void setUp() {
    randomizeFoodList();
    for (auto& b : boids) {
      b.type = rnd::prob(pPredators.get());
      //  std::cout << b.type << std::endl;
    }
    for (auto& b : boids) {
      randomize(b.bNav);
    }
  }
  
  void randomize(Nav& boidNav) {
    // std::cout << "randomize" << std::endl;
    boidNav.pos(r(), r(), r());
    boidNav.quat().set(r(), r(), r(), r()).normalize();
  }

  void onAnimate(double dt) override {
    dt *= timeStep.get();
    time += dt;
    timeStamp += dt;
    if (timeStamp > FOOD_RESET_RATE) {
      timeStamp = 0.0f;
      // std::cout << "time: " << time << std::endl;
      randomizeFoodList();
    }
    // angle += 0.1;
    // for (auto& f : food) {
    //     f += r()*0.3;
    // }

    // The target (blue) aims for the origin (0,0,0) while the agent (red) aims
    // for the target. moveF(t) means "move forward" by amount t. faceToward
    // also accepts an amount t that controls the rate of steering.
    for (auto& b : boids) {
      // detect other boids
      // detectNeighbors(b);
      // stay in the tank
      handleBounce(b, 0.96f);

      // find nearest food and set attention to it
      findNearestFood(b, food);

      // eat food
      if (al::dist(b.bNav.pos(), b.attention) < 0.05) {
        b.hunger -= 0.01f;
        if (b.hunger < 0.0f) {
          b.hunger = 0.1f;
          // set attention to next closest food
          findNearestFood(b, food);
        }
        // b.setAttention((b.hunger > 0.167f) ?
        //                 b.attention + (r() * (1.f - b.hunger)) :
        //                 Vec3d(r(), r(), r()),
        //                 0.67);
      } else {
        b.hunger = (b.hunger < 1.0f) ? b.hunger + 0.001f : 1.0f;
        // b.bNav.faceToward(b.attention, Vec3d(0, 1, 0), 0.03);
      }
      // b.bNav.moveF(0.2);
      // b.bNav.step(dt);

      b.update(0.67*b.hunger, dt);
    }
    // Might we also animate the App's nav()? This would be like making a
    // virtual robotic camera. We might decide the position and direction of the
    // camera based on what is happening in the virtual world. We could make
    // cinematic maneuvers such as a "dolly shot" or mimic styles such as Dutch
    // angle or "handheld". Cuts and fades are also possible.
    nav().smooth(0.5);
    nav().faceToward(Vec3d(0, 0, 0));
  }

  void onDraw(Graphics& g) override {
    // graphics / drawing settings
    g.clear(0);
    g.meshColor();
    g.pointSize(10);

    // g.rotate(angle, Vec3d(0, 1, 0));

    {
      Mesh mesh(Mesh::LINES);
      // draw the axes
      mesh.vertex(-10, 0, 0);
      mesh.vertex(10, 0, 0);
      mesh.vertex(0, -10, 0);
      mesh.vertex(0, 10, 0);
      mesh.vertex(0, 0, -10);
      mesh.vertex(0, 0, 10);
      for (int i = 0; i < 6; i++) mesh.color(1,1,1);

      g.draw(mesh);
    }

    // draw a body for each agent
    for (auto& b : boids) {
      {
        Nav& a(b.bNav);
        // randomize(a);
        g.pushMatrix();  // push()
        g.translate(a.pos());
        g.rotate(a.quat());  // rotate using the quat
        g.scale(0.03);
        g.draw(
          (b.type == 0) ? preyMesh : predMesh
        );
        g.popMatrix();  // pop()
      }
    }

    // for (auto& f : food) {
    //   foodMesh.vertex(f);
    //   foodMesh.color(0.5, 0.25, 0);
    // }
    // g.draw(foodMesh);

    // {
    //   Nav& a(agent);
    //   g.pushMatrix();  // push()
    //   g.translate(a.pos());
    //   g.rotate(a.quat());  // rotate using the quat
    //   g.scale(0.03);
    //   g.draw(mesh);
    //   g.popMatrix();  // pop()
    // }
    // {
    //   Nav& a(target);
    //   g.pushMatrix();  // push()
    //   g.translate(a.pos());
    //   g.rotate(a.quat());  // rotate using the quat
    //   g.scale(0.03);
    //   g.draw(mesh);
    //   g.popMatrix();  // pop()
    //
  }

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    // gui.add(t);
    gui.add(pPredators);
    gui.add(timeStep);
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
