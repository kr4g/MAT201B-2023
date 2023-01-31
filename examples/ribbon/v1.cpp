// MAT201B
// Karl Yerkes

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

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
  float t{0};

  void onCreate() override {}

  void onAnimate(double dt) override {
    t += dt;
    if (nav().vel().pos().mag() > 0.05) {
      tail.push_back({Vec3f(nav().pos()),  //
                      Vec3f(nav().uf()),   //
                      Vec3f(nav().uu()),   //
                      Vec3f(nav().ur()),   //
                      0.1});
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(0.1);
    g.color(color);
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
