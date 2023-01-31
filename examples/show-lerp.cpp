// Karl Yerkes
// MAT201B
// 2022-02-03
//
#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

using namespace al;

struct MyApp : App {
  ParameterVec3 pvec{"p"};
  ParameterVec3 qvec{"q"};
  Parameter amount{"amount", 0, 0, 1};

  void onCreate() override {
    //
  }

  void onAnimate(double dt) override {}

  void onDraw(Graphics &g) override {
    g.clear(1);
    g.meshColor();

    Mesh mesh(Mesh::LINES);

    // p!
    mesh.vertex(0, 0, 0);
    mesh.color(1, 0, 0);
    mesh.vertex(pvec.get());
    mesh.color(1, 0, 0);

    // q!
    mesh.vertex(0, 0, 0);
    mesh.color(0, 0, 1);
    mesh.vertex(qvec.get());
    mesh.color(0, 0, 1);

    // lerp
    mesh.vertex(0, 0, 0);
    mesh.color(0, 1, 0);
    mesh.vertex(Vec3f(pvec).lerp(qvec, amount));
    mesh.color(0, 1, 0);

    g.draw(mesh);

    g.pointSize(10);
    {
      Mesh mesh(Mesh::POINTS);
      // p!
      mesh.vertex(pvec.get());
      mesh.color(1, 0, 0);
      // q!
      mesh.vertex(qvec.get());
      mesh.color(0, 0, 1);
      // lerp
      mesh.vertex(Vec3f(pvec).lerp(qvec, amount));
      mesh.color(0, 1, 0);
      g.draw(mesh);
    }
  }

  void onInit() override {
    // set up GUI
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto &gui = GUIdomain->newGUI();
    gui.add(pvec); // add parameter to GUI
    gui.add(qvec);
    gui.add(amount);
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
