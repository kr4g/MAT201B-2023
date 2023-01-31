// Karl Yerkes
// MAT201B
// 2022-01-04
// minimal app, ready for adapting..
//

#include "al/app/al_App.hpp"

using namespace al;

struct MyApp : App {
  Mesh mesh;

  void onCreate() override {
    mesh.vertex(0, 0, 0); // std::vector.push_back(al::Vec3f(0, 0, 0))
    mesh.vertex(0, 1, 0);
    mesh.vertex(1, 0, 0);
    mesh.color(0, 0, 1);
    mesh.color(0, 1, 0);
    mesh.color(1, 0, 0);

    nav().pos(0, 0, 10);
  }

  void onAnimate(double dt) override {
    //
  }

  void onDraw(Graphics& g) override {
    g.clear(0.25);
    // g.color(1, 0, 0);
    g.meshColor();
    g.draw(mesh);
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
