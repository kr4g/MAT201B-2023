// Karl Yerkes
// MAT201B
// 2022-01-04
// minimal app, ready for adapting..
//

// fractals!!!
//
// * https://marcocetica.com/posts/non_euclidean_geometry/
// * https://en.wikipedia.org/wiki/Mandelbrot_set
// * https://en.wikipedia.org/wiki/Lyapunov_fractal
// * https://en.wikipedia.org/wiki/Newton_fractal
//
// Probably need to use shaders. Can do it on the CPU first, then adapt the
// algorithm to a (fragment) shader. Question: Can we do a fractal on a sphere?
// What is a pixel if there is no rectangular grid? How might we give structure
// to a "flat" fractal? How can we make a fractal that is truly 3D?
//
// * https://en.wikipedia.org/wiki/Non-Euclidean_geometry
// * https://en.wikipedia.org/wiki/Mandelbulb
// * https://en.wikipedia.org/wiki/Julia_set
// * http://paulbourke.net/fractals/quatjulia/
// * https://www.iquilezles.org/www/articles/juliasets3d/juliasets3d.htm
//
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

using namespace al;

struct MyApp : App {
  ParameterColor color{"Color"};
  ParameterInt mode{"Mode", "", 1, 1, 4};

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(mode).add(color);
  }

  void onCreate() override {}
  void onAnimate(double dt) override {}
  void onDraw(Graphics& g) override { g.clear(color.get()); }
  bool onKeyDown(const Keyboard& k) override { return false; }
  void onSound(AudioIOData& io) override {}
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
