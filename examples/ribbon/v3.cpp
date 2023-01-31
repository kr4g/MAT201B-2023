// MAT201B
// Karl Yerkes

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

struct MyApp : App {
  ParameterColor color{"Color"};
  Parameter width{"Width", 0.1, 0, 0.2};

  Nav head;
  Vec3f target;
  float t{0};
  float a{0};
  Mesh ribbon{Mesh::TRIANGLES};

  void onCreate() override {
    head.moveF(0.51);
    head.spinF(0.39);
    target.set(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());
    target *= 2.7;

    // initial 3 vertices
    ribbon.vertex(width, 0, 0);
    ribbon.vertex(0, 0, 0);
    ribbon.vertex(-width, 0, 0);
    ribbon.normal(width, 0, 0);
    ribbon.normal(0, 1, 0);
    ribbon.normal(-width, 0, 0);

    nav().pos(0, 0, 7);
  }

  void onAnimate(double dt) override {
    t += dt;
    head.step(dt);
    a += 0.1;

    if (t > 11) {
      t = 0;
      target.set(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());
      target *= 2.7;
    }

    head.faceToward(target, 0.01);

    if (ribbon.vertices().size() == 33333) {
      ribbon.reset();
    }

    ribbon.vertex(head.pos() + head.ur() * width);
    ribbon.vertex(head.pos());
    ribbon.vertex(head.pos() - head.ur() * width);
    ribbon.color(color);
    ribbon.color(color);
    ribbon.color(color);

    if (true) {
      // sort of curved surface shading
      ribbon.normal(Vec3f(head.uu()).lerp(head.ur(), 0.33));
      ribbon.normal(head.uu());
      ribbon.normal(Vec3f(head.uu()).lerp(head.ur(), -0.33));
    } else {
      // flat shading....
      ribbon.normal(head.uu());
      ribbon.normal(head.uu());
      ribbon.normal(head.uu());
    }

    const int n = ribbon.vertices().size();
    if (n > 3) {
      ribbon.index(n - 1);
      ribbon.index(n - 2);
      ribbon.index(n - 4);
      ribbon.index(n - 2);
      ribbon.index(n - 5);
      ribbon.index(n - 4);
      ribbon.index(n - 2);
      ribbon.index(n - 3);
      ribbon.index(n - 5);
      ribbon.index(n - 3);
      ribbon.index(n - 6);
      ribbon.index(n - 5);
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(0.3);
    g.meshColor();
    g.rotate(a, Vec3f(0, 1, 0));
    g.depthTesting(true);
    g.lighting(true);
    g.draw(ribbon);
  }

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(color);
    gui.add(width);
  }
  void onSound(AudioIOData& io) override {}
  bool onKeyDown(const Keyboard& k) override { return false; }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
