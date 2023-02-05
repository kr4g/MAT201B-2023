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

const int MAX_BOIDS = 9000;
const float MAX_PREY_LIFESPAN = 100.0f;
// const float MAX_PREDATORS = MAX_BOIDS * 0.1;
const int CUBE_SIZE = 3;
// const float FOOD_RESET_RATE = 3.f;
const float MIN_EDGE_PROXIMITY = 0.01f;

using namespace al;

double r() { return rnd::uniformS() * CUBE_SIZE; }

// A "boid" (play on bird) is one member of a flock.
class Boid {
 public:
  Nav bNav;  // Navigation object

  Vec3d goal;  // Where the boid is moving towards

  // Type of boid
  int type;  // 0 = prey, 1 = predator

  bool lifeStatus{true};

  // urge to persue food [0,1]
  // prey purse food particles, predators persue prey
  float hunger{1.0f};
  float fear{0.0f};

  float mutation{0.0f};
  float mutationRate{0.0f};

  float age{0.0f};
  float lifespan{MAX_PREY_LIFESPAN};

  void handleBounce() {
    // gradually steer boid away from cube edges
    // find distance to nearest each edge
    float xDist = std::min(std::abs(bNav.pos().x - CUBE_SIZE),
                           std::abs(bNav.pos().x + CUBE_SIZE));
    float yDist = std::min(std::abs(bNav.pos().y - CUBE_SIZE),
                           std::abs(bNav.pos().y + CUBE_SIZE));
    float zDist = std::min(std::abs(bNav.pos().z - CUBE_SIZE),
                           std::abs(bNav.pos().z + CUBE_SIZE));

    // as boid approaches edge, turn faster
    float closestDist = std::min(std::min(xDist, yDist), zDist);
    float turnRate = 0.1f / closestDist;

    if (xDist < MIN_EDGE_PROXIMITY) {
        bNav.faceToward(Vec3d(-bNav.pos().x, bNav.pos().y, bNav.pos().z),
                          Vec3d(0, 1, 0), turnRate);
    }
    if (yDist < MIN_EDGE_PROXIMITY) {
        bNav.faceToward(Vec3d(bNav.pos().x, -bNav.pos().y, bNav.pos().z),
                          Vec3d(0, 1, 0), turnRate);
    }
    if (zDist < MIN_EDGE_PROXIMITY) {
        bNav.faceToward(Vec3d(bNav.pos().x, bNav.pos().y, -bNav.pos().z),
                          Vec3d(0, 1, 0), turnRate);
    }
  }
  
  void detectBoids(std::vector<Boid>& boids) {};
  
  void findNearestFood(std::vector<Vec3f> &food) {
    float minDist = 1000.0f;
    Vec3f nearestFood;
    for (auto &f : food) {
      float dist = (f - bNav.pos()).mag();
      if (dist < CUBE_SIZE && dist < minDist) {
        minDist = dist;
        // if the target food is close but the boid is not hungry...
        if (goal == f && dist < 0.05 && hunger < 0.1) {
          continue;  // ...find another food
        }
        nearestFood = f + (r() * mutation);  // boids aren't always born with the best aim
      }
    }
    setGoal(nearestFood, 0.01);
  }

  void detectSurroundings(std::vector<Boid>& boids, std::vector<Vec3f>& food) {
    // stay in the tank
    handleBounce();
      
    // detect other boids
    // detectBoids(boids);

    // find nearest food and set goal to it
    findNearestFood(food);
  }

  void setGoal(Vec3d a, double amt) { 
    goal.set(a);
    bNav.smooth(0.1);
    bNav.faceToward(goal, Vec3d(0, 1, 0), amt);
  }

  // Update position based on velocity and delta time
  void update(double v, double dt) {
    // bNav.step(dt);
      bNav.moveF(v);
      bNav.step(dt);
      age += dt;
      mutation += mutationRate * dt;
      lifeStatus = age <= lifespan;
    }

  friend std::ostream& operator<<(std::ostream& os, const Boid& b) {
    os << "Boid: \n";
    return os;
  }
};

struct MyApp : App {
  
  Parameter timeStep{"Time Step", "", 1.33f, "", 0.01, 3.0};
  Parameter pPredators{"Predators", "", 0.0f, 0.01f, 0.03f};
  Parameter foodResetRate{"Food Reset Rate", "", 3.67f, 0.1667f, 10.f};
  // Parameter boidRespawnRate{"Boid Respawn Rate", "", 6.67f, 0.167f, 10.f};
  // Nav agent;
  // Nav target;
  std::vector<Boid> boids{MAX_BOIDS};
  double time{0};
  double foodRefresh{0};
  double boidRespawn{0};
  double timeScale{1.0};
  double angle{0};

  double initDist;

  Mesh predMesh;
  Mesh preyMesh;
  // Mesh boidMesh;
  Mesh foodMesh{Mesh::POINTS};

  std::vector<Vec3f> food{MAX_BOIDS * 0.67};
  // Nav point;

  void onCreate() override {
    setUp();

    // place the camera so that we can see the axes
    nav().pos(0.5, 0.7, CUBE_SIZE * 6);
    initDist = al::dist(nav().pos(), Vec3d(0, 0, 0));
    // nav().pos(0, 0, CUBE_SIZE * 2.5);
    nav().faceToward(Vec3d(0, 0, 0), Vec3d(0, 1, 0));

    // Don't do this:
    // nav().faceToward(0, 0, 0);
    // because it will be interpreted as this:
    // nav().faceToward(Vec3d(0), Vec3d(0), 0);
    // which has no effect because of the final 0!

    // create a prototype predator body
    predMesh.primitive(Mesh::TRIANGLE_FAN);
    predMesh.vertex(0, 0, -2);
    predMesh.color(rnd::uniform(0.5, 1.0), rnd::uniform(0.0, 0.5), 0);
    predMesh.vertex(0, 1, 0);
    predMesh.color(rnd::uniform(0.5, 1.0), rnd::uniform(0.0, 0.5), 0);
    predMesh.vertex(-1, 0, 0);
    predMesh.color(rnd::uniform(0.5, 1.0), rnd::uniform(0.0, 0.5), 0);
    predMesh.vertex(1, 0, 0);
    predMesh.color(rnd::uniform(0.5, 1.0), rnd::uniform(0.0, 0.5), 0);
    predMesh.vertex(0, 1, 0);
    predMesh.color(rnd::uniform(0.5, 1.0), rnd::uniform(0.0, 0.5), 0);

    // create a prototype prey body
    preyMesh.primitive(Mesh::TRIANGLE_FAN);
    preyMesh.vertex(0, 0, -2);
    preyMesh.color(0, rnd::uniform(0.5, 1.0), 0);
    preyMesh.vertex(0, 1, 0);
    preyMesh.color(0, rnd::uniform(0.5, 1.0), 0);
    preyMesh.vertex(-1, 0, 0);
    preyMesh.color(0, rnd::uniform(0.5, 1.0), 0);
    preyMesh.vertex(1, 0, 0);
    preyMesh.color(0, 0, rnd::uniform(0.5, 1.0));
    preyMesh.vertex(0, 1, 0);
    preyMesh.color(0, 0, rnd::uniform(0.5, 1.0));
  }
  
  // void handleBounce(Boid &b) {
  //   // gradually steer boid away from cube edges
  //   // find distance to nearest each edge
  //   float xDist = std::min(std::abs(b.bNav.pos().x - CUBE_SIZE),
  //                          std::abs(b.bNav.pos().x + CUBE_SIZE));
  //   float yDist = std::min(std::abs(b.bNav.pos().y - CUBE_SIZE),
  //                          std::abs(b.bNav.pos().y + CUBE_SIZE));
  //   float zDist = std::min(std::abs(b.bNav.pos().z - CUBE_SIZE),
  //                          std::abs(b.bNav.pos().z + CUBE_SIZE));
  // 
  //   // as boid approaches edge, turn faster
  //   float closestDist = std::min(std::min(xDist, yDist), zDist);
  //   float turnRate = 0.1f / closestDist;
  // 
  //   if (xDist < MIN_EDGE_PROXIMITY) {
  //       b.bNav.faceToward(Vec3d(-b.bNav.pos().x, b.bNav.pos().y, b.bNav.pos().z),
  //                         Vec3d(0, 1, 0), turnRate);
  //   }
  //   if (yDist < MIN_EDGE_PROXIMITY) {
  //       b.bNav.faceToward(Vec3d(b.bNav.pos().x, -b.bNav.pos().y, b.bNav.pos().z),
  //                         Vec3d(0, 1, 0), turnRate);
  //   }
  //   if (zDist < MIN_EDGE_PROXIMITY) {
  //       b.bNav.faceToward(Vec3d(b.bNav.pos().x, b.bNav.pos().y, -b.bNav.pos().z),
  //                         Vec3d(0, 1, 0), turnRate);
  //   }
  // }
  
  // void findNearestFood(Boid &b, std::vector<Vec3f> &food) {
  //   float minDist = 1000.0f;
  //   Vec3f nearestFood;
  //   for (auto &f : food) {
  //     float dist = (f - b.bNav.pos()).mag();
  //     if (dist < CUBE_SIZE && dist < minDist) {
  //       minDist = dist;
  //       // if the target food is close but the boid is not hungry...
  //       if (b.goal == f && dist < 0.05 && b.hunger < 0.1) {
  //         continue;  // ...find another food
  //       }
  //       nearestFood = f + (r() * b.mutation);  // boids aren't always born with the best aim
  //     }
  //   }
  //   b.setGoal(nearestFood, 0.01);
  // }
  
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

  void cleanupDeadBoids(std::vector<Boid>& boids) {
    // remove dead boids
    boids.erase(std::remove_if(boids.begin(), boids.end(),
                               [](const Boid& b) { return !b.lifeStatus; }),
                boids.end());
  }

  void setUp() {
    // randomize food positions
    randomizeFoodList();
    
    // randomize boids
    for (auto& b : boids) {
      b.type = rnd::prob(pPredators.get());  // probability of being a predator
      b.mutation = rnd::prob(0.7) * rnd::uniform();  // random mutation
      b.mutationRate = rnd::uniform(b.mutation) * rnd::uniform();  // random mutation rate proportional to mutation
      b.lifespan = rnd::uniform(MAX_PREY_LIFESPAN - .5*MAX_PREY_LIFESPAN*b.mutation*b.mutationRate,
                                MAX_PREY_LIFESPAN + .5*MAX_PREY_LIFESPAN*b.mutation*b.mutationRate);  // random lifespan
      randomize(b.bNav);
    }
  }
  
  void randomize(Nav& boidNav) {
    // std::cout << "randomize" << std::endl;
    boidNav.pos(r(), r(), r());
    boidNav.quat().set(r(), r(), r(), r()).normalize();
  }

  void respawn() {
    for (int i = 0; i < MAX_BOIDS - boids.size(); ++i) {
      Boid b;
      b.type = rnd::prob(pPredators.get());  // probability of being a predator
      b.mutation = rnd::prob(0.7) * rnd::uniform();  // random mutation
      b.mutationRate = rnd::uniform(b.mutation) * rnd::uniform();  // random mutation rate proportional to mutation
      b.lifespan = rnd::uniform(MAX_PREY_LIFESPAN - .5*MAX_PREY_LIFESPAN*b.mutation*b.mutationRate,
                                MAX_PREY_LIFESPAN + .5*MAX_PREY_LIFESPAN*b.mutation*b.mutationRate);  // random lifespan
      randomize(b.bNav);
      boids.push_back(b);
    }
  }

  void onAnimate(double dt) override {
    // std::cout << "Total boids: " << boids.size() << ", t: " << time << std::endl;
    timeScale = al::dist(nav().pos(), Vec3d(0, 0, 0)) / initDist;
    dt *= timeStep.get() * timeScale;
    time += dt;
    foodRefresh += dt;
    boidRespawn += dt;
    angle += 0.06;
    if (foodRefresh > foodResetRate.get()) {
      foodRefresh = 0.0f;
      // std::cout << "time: " << time << std::endl;
      randomizeFoodList();
    }

    // double respawnScale = boids.size() / MAX_BOIDS;
    // if (boidRespawn*respawnScale > boidRespawnRate.get()) {
    //   respawn();
    // }
    // angle += 0.1;
    // for (auto& f : food) {
    //     f += r()*0.3;
    // }

    // The target (blue) aims for the origin (0,0,0) while the agent (red) aims
    // for the target. moveF(t) means "move forward" by amount t. faceToward
    // also accepts an amount t that controls the rate of steering.
    for (auto& b : boids) {
      // run boid self-orientation algorithm
      b.detectSurroundings(boids, food);

      // eat food
      if (al::dist(b.bNav.pos(), b.goal) < 0.05) {
        if (b.hunger < 0.1f) {  // if full...
          b.hunger = 0.1f;
          b.findNearestFood(food);  // ...set goal to next closest food
        } else {  // otherwise...
          b.hunger -= 0.01f;  // eat
          b.bNav.faceToward(b.goal + (r() * b.hunger),  // hungry boids eat more eratically
                            Vec3d(0, 1, 0),
                            b.hunger);
        }
      } else {
        b.hunger = (b.hunger < 1.0f) ? b.hunger + 0.001f : 1.0f;
      }
      b.update(0.67*b.hunger, dt);
    }
    // cleanupDeadBoids(boids);
    // Might we also animate the App's nav()? This would be like making a
    // virtual robotic camera. We might decide the position and direction of the
    // camera based on what is happening in the virtual world. We could make
    // cinematic maneuvers such as a "dolly shot" or mimic styles such as Dutch
    // angle or "handheld". Cuts and fades are also possible.
    nav().smooth(0.9);
    nav().faceToward(Vec3d(0, 0, 0));
  }

  bool onKeyDown(Keyboard const& k) override {
    switch (k.key()) {
      case ' ':
        // reset the simulation
        setUp();
        break;
    }
    return true;
  }

  void onDraw(Graphics& g) override {
    // graphics / drawing settings
    g.clear(0);
    g.meshColor();
    g.pointSize(10);

    g.rotate(angle, Vec3d(0, 1, 0));

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
        g.scale(
          // predators can be up to 1.5x larger than prey
          (b.type == 0) ? 0.03 * rnd::uniform(0.83, 1.0) : 0.09 * rnd::uniform(0.5, 1.0)
        );
        g.draw(
          // prey are blue/green, predators are red/orange
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
    gui.add(foodResetRate);
    // gui.add(boidRespawnRate);
    gui.add(timeStep);
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
