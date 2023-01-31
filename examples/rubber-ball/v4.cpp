// Karl Yerkes
// MAT201B
// 2022-01-04
// minimal app, ready for adapting..
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/graphics/al_Shapes.hpp"

using namespace al;

struct MyApp : App {
  Parameter scale_x{"x", 1, 0, 2};
  Parameter scale_y{"y", 1, 0, 2};
  Parameter scale_z{"z", 1, 0, 2};
  Mesh ball;

  void onCreate() override {
    addSphere(ball, 1, 100, 100);
    ball.decompress();
    ball.generateNormals();
    nav().pos(0, 0, 5);
  }
  double a = 0;
  double b = 0;
  void onAnimate(double dt) override {
    a += 0.29;
    b += 0.23;
  }

  void onDraw(Graphics& g) override {
    g.clear(0.2);
    g.depthTesting(true);
    g.lighting(true);
    g.rotate(a, Vec3f(0, 1, 0));
    g.rotate(b, Vec3f(1));
    g.scale(scale_x.get(), scale_y.get(), scale_z.get());
    g.draw(ball);
  }

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(scale_x);
    gui.add(scale_y);
    gui.add(scale_z);
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}