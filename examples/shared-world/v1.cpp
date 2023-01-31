// Karl Yerkes
// MAT201B
// 2022-01-04
// minimal app, ready for adapting..
//

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/math/al_Random.hpp"

using namespace al;

#include <regex>
#include <unordered_map>

std::vector<std::string> ip{
    "192.168.2.1",  // Karl
    "192.168.2.2",  // Sihwa
    "192.168.2.3",  // Adam
    "192.168.2.4",  // Ana
    "192.168.2.5",  // Sabina
    "192.168.2.6",  // Iason
    "192.168.2.7",  // Nefeli
};

struct MyApp : App {
  std::unordered_map<std::string, al::Pose> agent;
  Mesh cone{Mesh::TRIANGLES};
  std::string me;

  void onCreate() override {
    addCone(cone);
    cone.decompress();
    cone.generateNormals();

    char buffer[20];
    sprintf(buffer, "~%d~", rnd::uniform(10000));  // ~1234~ a key
    me += buffer;
  }

  void onMessage(osc::Message& m) override {
    // receive someones pose
    if (m.addressPattern() == "/pose") {
      // m.print();
      std::string who;
      m >> who;  // means "pick off the next argument"
      if (who != me) {
        Pose p;
        m >> p.pos().x;
        m >> p.pos().y;
        m >> p.pos().z;
        m >> p.quat().w;
        m >> p.quat().x;
        m >> p.quat().y;
        m >> p.quat().z;
        agent[who] = p;
        // array[int index] = something
        // dictionary[string index] = something
      }
    }
  }

  void onAnimate(double dt) override {
    // broadcast our pose
    for (auto i : ip) {
      osc::Send(9010, i.c_str())
          .send("/pose", me,                                  //
                nav().pos().x, nav().pos().y, nav().pos().z,  //
                nav().quat().w, nav().quat().x, nav().quat().y, nav().quat().z);

      // /address/pattern,type-string\0binary-data
      // /pose           ,sddddddd
      // \0~1234~••••••••••••••••••••••••••••••••••••••••••••••••••••••••
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(0.2);

    g.depthTesting(true);
    g.lighting(true);

    for (auto a : agent) {
      g.pushMatrix();
      g.translate(a.second.pos());
      g.rotate(a.second.quat());
      g.scale(0.1);
      g.draw(cone);
      g.popMatrix();
    }
  }

  bool onKeyDown(const Keyboard& k) override { return false; }

  void onSound(AudioIOData& io) override {}

  void onInit() override {
    // set up GUI
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    // gui.add(color);
  }
};

int main() {
  MyApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}
