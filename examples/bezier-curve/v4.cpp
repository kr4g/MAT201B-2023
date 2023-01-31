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

/*
   Tension: 1 is high, 0 normal, -1 is low
   Bias: 0 is even,
         positive is towards first segment,
         negative towards the other
*/
double _hermite(double y0, double y1, double y2, double y3, double mu,
                double tension = 0, double bias = 0) {
  double m0, m1, mu2, mu3;
  double a0, a1, a2, a3;

  mu2 = mu * mu;
  mu3 = mu2 * mu;
  m0 = (y1 - y0) * (1 + bias) * (1 - tension) / 2;
  m0 += (y2 - y1) * (1 - bias) * (1 - tension) / 2;
  m1 = (y2 - y1) * (1 + bias) * (1 - tension) / 2;
  m1 += (y3 - y2) * (1 - bias) * (1 - tension) / 2;
  a0 = 2 * mu3 - 3 * mu2 + 1;
  a1 = mu3 - 2 * mu2 + mu;
  a2 = mu3 - mu2;
  a3 = -2 * mu3 + 3 * mu2;

  return (a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2);
}

void hermite(std::vector<Vec3f>& out, const Vec3f* in,  //
             int N, float tension = 0, float bias = 0) {
  for (float mu = 1.0f / N; mu <= 1; mu += 1.0f / N) {
    out.push_back(
        Vec3f(_hermite(in[0].x, in[1].x, in[2].x, in[3].x, mu, tension, bias),
              _hermite(in[0].y, in[1].y, in[2].y, in[3].y, mu, tension, bias),
              _hermite(in[0].z, in[1].z, in[2].z, in[3].z, mu, tension, bias)));
  }
}

Vec3f r() { return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()); }

const int M = 10;
const int N = 100;

struct MyApp : App {
  Parameter tension{"tension", 0, -1, 1};
  Parameter bias{"bias", 0, -1, 1};

  Mesh curve{Mesh::LINES};
  Mesh control{Mesh::POINTS};

  float a{0};
  float b{0};

  void reset() {
    curve.reset();
    control.reset();

    for (int i = 0; i < M; i++) {
      control.vertex(r());
      control.color(Color(1));
    }

    for (int i = 0; i < M - 3; i++) {
      hermite(curve.vertices(), &control.vertices()[i], N);
    }

    for (int i = 0; i < curve.vertices().size(); i++) {
      curve.color(HSV(0.5));
    }
  }

  void onCreate() override {
    nav().pos(0, 0, 5);
    reset();
  }

  void onAnimate(double dt) override {
    a += 0.11;
    b += 0.23;

    curve.reset();

    for (int i = 0; i < M - 3; i++) {
      hermite(curve.vertices(), &control.vertices()[i], N, tension, bias);
    }

    for (int i = 0; i < curve.vertices().size(); i++) {
      curve.color(HSV(0.5));
    }
  }

  void onDraw(Graphics& g) override {
    g.rotate(a, Vec3f(0, 1, 0));
    g.rotate(b, Vec3f(1, 1, 0));
    g.pointSize(5);
    g.meshColor();

    g.clear(0.2);
    g.draw(curve);
    g.draw(control);
  }

  void onMessage(osc::Message& m) override { m.print(); }
  bool onKeyDown(const Keyboard& k) override {
    if (k.key() == ' ') {
      reset();
    }
    return false;
  }

  void onSound(AudioIOData& io) override {}
  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(tension);
    gui.add(bias);
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 2);
  app.start();
}
