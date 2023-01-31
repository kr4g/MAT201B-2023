// Karl Yerkes
// MAT201B
// 2022-01-04
// minimal app, ready for adapting..
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

using namespace al;

struct MyApp : App {
  ParameterColor color{"Color"};
  ParameterInt mode{"Mode", "", 1, 1, 4};
  Parameter r{"r", 0, 0, 4};
  Parameter N{"N", 0, 0, 1000};
  Parameter x0{"x0", 0, 0, 1};
  Mesh mesh;

  void recompute() {
    mesh.reset();
    mesh.vertex(0, x0);
    float x = x0;
    for (int i = 1; i < N; i++) {
      x = r * x * (1 - x);
      mesh.vertex((float)i / N, x);
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(color.get());
    g.color(0);
    // g.scale(0.01, 1, 1);
    g.draw(mesh);
  }

  void onCreate() override {
    mesh.primitive(Mesh::LINE_STRIP);
    nav().pos(0, 0, 5);
  }

  void onAnimate(double dt) override {}
  bool onKeyDown(const Keyboard& k) override { return false; }
  void onSound(AudioIOData& io) override {}
  bool onMouseDrag(Mouse const&) override {
    recompute();
    return true;
  }
  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(mode).add(color);
    gui.add(r).add(N).add(x0);
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
