// Karl Yerkes
// MAT201B
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/ui/al_Parameter.hpp"

using namespace al;

struct State {
  float a, b, m;
  float n[3];
};

// https://en.wikipedia.org/wiki/Superformula
float super(const float psi, const State& state) {
  const float t = state.m * psi / 4;
  return pow(pow(abs(cos(t) / state.a), state.n[1]) +
                 pow(abs(sin(t) / state.b), state.n[2]),
             -1 / state.n[0]);
}

// https://en.wikipedia.org/wiki/Superformula#Extension_to_higher_dimensions
Vec3f spherical_product(const float latitude, const float longitude,
                        const float r1, const float r2) {
  return Vec3f(r1 * cos(longitude) * r2 * cos(latitude),
               r1 * sin(longitude) * r2 * cos(latitude),  //
               r2 * sin(latitude));
}

struct MyApp : App {
  Parameter r1[6]{
      {"a", "r1", 1, 0, 2},    //
      {"b", "r1", 1, 0, 2},    //
      {"m", "r1", 1, 0, 16},   //
      {"n1", "r1", 1, 0, 30},  //
      {"n2", "r1", 1, 0, 20},  //
      {"n3", "r1", 1, 0, 20},
  };
  Parameter r2[6]{
      {"a", "r2", 1, 0, 2},    //
      {"b", "r2", 1, 0, 2},    //
      {"m", "r2", 1, 0, 16},   //
      {"n1", "r2", 1, 0, 30},  //
      {"n2", "r2", 1, 0, 20},  //
      {"n3", "r2", 1, 0, 20},
  };
  ParameterInt primitive{"Primitive", "", 2, 0, 10};
  ParameterInt N{"N", "", 100, 0, 200 / primitive};
  Parameter spin[2]{
      {"Spin1", 0.01, -0.5, 0.5},  //
      {"Spin2", 0.01, -0.5, 0.5},  //
  };
  float a{0};
  float b{0};

  PresetHandler presetHandler{"sequencerPresets"};

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(presetHandler);

    gui.add(N);
    presetHandler << N;
    gui.add(primitive);
    presetHandler << primitive;
    gui.add(spin[0]).add(spin[1]);
    presetHandler << spin[0] << spin[1];
    for (int i = 0; i < 6; i++) {
      gui.add(r1[i]);
      presetHandler << r1[i];
    }
    for (int i = 0; i < 6; i++) {
      gui.add(r2[i]);
      presetHandler << r2[i];
    }
  }

  void onCreate() override {}
  void onAnimate(double dt) override {
    a += spin[0];
    b += spin[1];
  }

  void onDraw(Graphics& g) override {
    g.clear(0.2);
    g.rotate(a, Vec3f(1, 1, 1));
    g.rotate(b, Vec3f(0, 1, 0));
    Mesh e{(Mesh::Primitive)primitive.get()};
    for (float latitude = -M_PI / 2; latitude < M_PI / 2; latitude += 1.0 / N) {
      for (float longitude = -M_PI; longitude < M_PI; longitude += 1.0 / N) {
        e.vertex(spherical_product(
            latitude, longitude,  //
            super(latitude, {r1[0], r1[1], r1[2], {r1[3], r1[4], r1[5]}}),
            super(longitude, {r2[0], r2[1], r2[2], {r2[3], r2[4], r2[5]}})));
      }
    }

    g.draw(e);
  }

  bool onKeyDown(const Keyboard& k) override { return false; }

  void onSound(AudioIOData& io) override {}
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
