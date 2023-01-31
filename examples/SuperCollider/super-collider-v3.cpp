// Karl Yerkes
// MAT201B
// 2022-02-22
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

using namespace al;

struct MyApp : App {
  std::vector<Parameter> numbered{
      {"0", "numbered", 0, 0, 1},  //
      {"1", "numbered", 0, 0, 1},  //
      {"2", "numbered", 0, 0, 1},  //
      {"3", "numbered", 0, 0, 1},  //
      {"4", "numbered", 0, 0, 1},  //
  };

  std::vector<Parameter> named{
      {"note", "named", 0, 0, 1},       //
      {"break", "named", 0, 0, 1},      //
      {"resonance", "named", 0, 0, 1},  //
      {"pan", "named", 0, 0, 1},        //
      {"amplitude", "named", 0, 0, 1},  //
      {"width", "named", 0, 0, 1},      //
  };

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();

    for (int i = 0; i < numbered.size(); i++) {
      gui.add(numbered[i]);
      parameterServer() << numbered[i];
    }

    for (int i = 0; i < named.size(); i++) {
      gui.add(named[i]);
      parameterServer() << named[i];
    }
  }

  void onDraw(Graphics& g) override { g.clear(0.2); }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}