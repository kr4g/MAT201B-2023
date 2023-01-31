// Karl Yerkes
// MAT201B
// 2022-02-22
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

using namespace al;

struct MyApp : App {
  Parameter value{"foo", 0, 0, 1};

  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(value);
  }

  void onCreate() override {
    parameterServer().addListener("127.0.0.1", 9999);

    // example of sending an OSC message
    osc::Send(9999, "127.0.0.1").send("/foo", 1, 2.0f, 3.0);
  }

  void onMessage(osc::Message& m) override {
    m.print();
    if (m.addressPattern() == "/foo") {
      float f;
      m >> f;
      value.set(f);
    }

    if (m.addressPattern() == "/Array") {
      // TBD
      /*
            while (...) {
              float f;
              m >> f;
              // to something with f
            }
          } */
    }
  }

  void onDraw(Graphics& g) override { g.clear(0.2); }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
