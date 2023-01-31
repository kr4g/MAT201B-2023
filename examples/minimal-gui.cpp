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
  ParameterColor color{"Color"};
  ParameterInt mode{"Mode", "", 1, 1, 4};
  Mesh mesh;
  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(mode).add(color);
  }

  void onCreate() override {
    mesh.primitive(Mesh::TRIANGLES);
    addSphere(mesh, 0.3);
    addCone(mesh, 0.1);
    // mesh.transform()
  }
  void onAnimate(double dt) override {}
  void onDraw(Graphics& g) override {
    g.clear(color.get());
    g.lighting(true);
    g.depthTesting(true);

    g.draw(mesh);
  }
  bool onKeyDown(const Keyboard& k) override { return false; }
  void onSound(AudioIOData& io) override {}
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
