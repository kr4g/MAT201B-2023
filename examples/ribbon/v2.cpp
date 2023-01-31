// MAT201B
// Karl Yerkes

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

struct Piece {
  Vec3f position;
  Vec3f forward, up, right;
  float width;
};

struct MyApp : App {
  ParameterColor color{"Color"};
  Parameter width{"Width", 0, 0.1, 1};
  std::vector<Piece> tail;
  Nav head;
  float t{0};
  float a{0};
  Vec3f target;

  void onCreate() override {
    head.moveF(0.51);
    head.spinF(0.39);
    target.set(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());
    target *= 2.7;
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

    tail.push_back({Vec3f(head.pos()),  //
                    Vec3f(head.uf()),   //
                    Vec3f(head.uu()),   //
                    Vec3f(head.ur()),   //
                    0.03});
  }

  void onDraw(Graphics& g) override {
    g.clear(0.1);
    g.color(color);
    g.rotate(a, Vec3f(0, 1, 0));
    g.depthTesting(true);
    g.lighting(true);
    Mesh m{Mesh::TRIANGLES};
    for (int i = 1; i < tail.size(); i++) {
      const auto& p = tail[i];
      const auto& q = tail[i - 1];

      m.vertex(p.position);
      m.vertex(q.position);
      m.vertex(q.position - q.right * q.width);

      m.vertex(p.position);
      m.vertex(q.position - q.right * q.width);
      m.vertex(p.position - p.right * p.width);

      m.vertex(p.position);
      m.vertex(q.position + q.right * q.width);
      m.vertex(q.position);

      m.vertex(p.position);
      m.vertex(p.position + p.right * p.width);
      m.vertex(q.position + q.right * q.width);
    }
    m.generateNormals();
    g.draw(m);
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
