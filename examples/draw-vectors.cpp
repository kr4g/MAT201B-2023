// Karl Yerkes
// 2022-01-20

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

struct Axes {
  void draw(Graphics &g) {
    Mesh mesh(Mesh::LINES);
    // x axis
    mesh.vertex(0, 0, 0);
    mesh.color(1, 1, 1);  // white
    mesh.vertex(1, 0, 0);
    mesh.color(1, 0, 0);

    // y axis
    mesh.vertex(0, 0, 0);
    mesh.color(1, 1, 1);  // white
    mesh.vertex(0, 1, 0);
    mesh.color(0, 1, 0);

    // z axis
    mesh.vertex(0, 0, 0);
    mesh.color(1, 1, 1);  // white
    mesh.vertex(0, 0, 1);
    mesh.color(0, 0, 1);

    g.draw(mesh);
  }
};

struct AlloApp : App {
  // Parameter pointSize{"/pointSize", "", 1.0, 0.0, 2.0};

  Axes axes;

  void onInit() override {
    // set up GUI
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto &gui = GUIdomain->newGUI();
  }

  void onCreate() override { nav().pos(0, 0, 10); }


  bool onKeyDown(const Keyboard &k) override {
    if (k.key() == '1') {
    }
    return true;
  }
  float angle = 0;
  void onAnimate(double dt) override {

      angle += 0.1;
  }

  void onDraw(Graphics &g) override {
    g.clear(0.3);
    g.meshColor();

    g.rotate(angle, 0, 1, 0);
    axes.draw(g);

    Mesh mesh(Mesh::LINES);

    // Vector a
    Vec3f a(1,2,0);

    // mesh.vertex(0, 0, 0);
    // mesh.color(1, 1, 1);  // white
    // mesh.vertex(a);
    // mesh.color(1, 1, 0);

    // Vector b
    Vec3f b(1.5, -0.3, -0.3);
    mesh.vertex(0, 0, 0);
    mesh.color(1, 1, 1);  // white
    mesh.vertex(b);
    mesh.color(0, 1, 1);

    // Vector c
    Vec3f c = a + b;
    mesh.vertex(0, 0, 0);
    mesh.color(1, 1, 1);  // white
    mesh.vertex(c);
    mesh.color(1, 0, 1);

    mesh.vertex(b);
    mesh.color(1, 1, 1);  // white
    mesh.vertex(c);
    mesh.color(1, 0, 1);

    g.draw(mesh);
  }
};

int main() {
  AlloApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}