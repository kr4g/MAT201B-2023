// Karl Yerkes | MAT201B
// 2022-02-22  | Audio-reactive visualizer
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Functions.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

#include <vector>

// 3rd order Bezier curve
// adapted from the code found here:
//   https://medium.com/geekculture/2d-and-3d-b%C3%A9zier-curves-in-c-499093ef45a9
//
void bezier(std::vector<Vec3f>& out, const std::vector<Vec3f>& in, int N) {
  for (float t = 1.0f / N; t <= 1; t += 1.0f / N) {
    out.push_back(
        Vec3f(pow3((1 - t)) * in[0].x + 3 * pow2(1 - t) * t * in[1].x +
                  3 * (1 - t) * pow2(t) * in[2].x + pow3(t) * in[3].x,
              pow3((1 - t)) * in[0].y + 3 * pow2(1 - t) * t * in[1].y +
                  3 * (1 - t) * pow2(t) * in[2].y + pow3(t) * in[3].y,
              pow3((1 - t)) * in[0].z + 3 * pow2(1 - t) * t * in[1].z +
                  3 * (1 - t) * pow2(t) * in[2].z + pow3(t) * in[3].z));
  }
}

Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }

struct MyApp : App {
  Mesh curve;
  Mesh control;

  void onCreate() override {
    control.vertex(r());
    control.vertex(r());
    control.vertex(r());
    control.vertex(r());

    curve.primitive(Mesh::LINES);
    bezier(curve.vertices(), control.vertices(), 50);

    nav().pos(0, 0, 5);
  }

  void onDraw(Graphics& g) override {
    g.clear(0.2);
    g.draw(curve);
  }

  void onMessage(osc::Message& m) override { m.print(); }
  bool onKeyDown(const Keyboard& k) override { return false; }
  void onAnimate(double dt) override {}
  void onSound(AudioIOData& io) override {}
  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 2);
  app.start();
}
