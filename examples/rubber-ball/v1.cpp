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
  Mesh ball;

  void onCreate() override {
    addSphere(ball);
    ball.decompress();
    ball.generateNormals();
    nav().pos(0, 0, 5);
  }

  void onDraw(Graphics& g) override {
    g.clear(0.2);
    g.depthTesting(true);
    g.lighting(true);
    g.draw(ball);
  }

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
