//////////////////////////////////////////////////////////////////////////////
/// Karl Yerkes //////////////////////////////////////////////////////////////
/// MAT 201B /////////////////////////////////////////////////////////////////
/// 2022-03-03 ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "support.hpp"  // AlloLib includes + supporting classes/functions

//////////////////////////////////////////////////////////////////////////////
/// AlloLib App //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

const char* version = "v3";
const char* path = "..";

using namespace al;
struct AlloApp : App {
  ParameterColor background{"Background", "", Color(0)};
  ParameterColor foreground{"Foreground", "", Color(1)};
  Parameter pointsize{"PointSize", "", 1.0, 0.0, 2.0};
  Parameter focus[2]{{"1", "focus", -0.51, -1, 1},  //
                     {"2", "focus", 0.54, -1, 1}};
  Parameter zoomrate{"Zoomrate", "", 0.0, -0.1, 0.1};
  //
  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(background).add(foreground).add(pointsize);
    gui.add(focus[0]).add(focus[1]).add(zoomrate);
  }

  FileWatch file_watch;
  ShaderProgram stable;
  Mesh mesh;
  //
  void onCreate() override {
    // if we cannot compile the shader code, exit.
    if (!file_watch.maybe_recompile(stable, version, path)) {
      exit(1);
    }

    mesh.primitive(Mesh::POINTS);
    mesh.vertex(0, 0, 0);
    mesh.texCoord(1, 0);
    mesh.color(foreground);

    nav().pos(0, 0, 5);
  }

  void onDraw(Graphics& g) override {
    g.clear(background);
    g.shader(stable);
    g.shader().uniform("pointSize", pointsize);
    g.shader().uniform("iResolution", Vec2f(width(), height()));
    g.shader().uniform("Focus", Vec2f(focus[0], focus[1]));
    g.shader().uniform("Zoom", zoom);
    g.blending(true);
    g.blendTrans();
    g.depthTesting(true);
    g.draw(mesh);
  }

  double t = 0;
  float zoom;
  void onAnimate(double dt) override {
    t += dt;
    mesh.colors().back().set(foreground.get(), 1.0);

    zoom += zoomrate;

    file_watch.maybe_recompile(stable, version, path);
  }

  bool onKeyDown(const Keyboard& k) override {
    if (k.key() == ' ') {
      zoom = 0;
    }
  }
};

//////////////////////////////////////////////////////////////////////////////
/// Main /////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int main() {
  AlloApp app;
  app.configureAudio(48000, 512, 2, 0);
  app.start();
}