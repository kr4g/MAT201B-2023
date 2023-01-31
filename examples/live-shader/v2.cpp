//////////////////////////////////////////////////////////////////////////////
/// Karl Yerkes //////////////////////////////////////////////////////////////
/// MAT 201B /////////////////////////////////////////////////////////////////
/// 2022-03-03 ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "support.hpp"  // AlloLib includes + supporting classes/functions

//////////////////////////////////////////////////////////////////////////////
/// AlloLib App //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

using namespace al;
struct AlloApp : App {
  ParameterColor background{"Background", "", Color(0)};
  ParameterColor foreground{"Foreground", "", Color(1)};
  Parameter pointsize{"PointSize", "", 1.0, 0.0, 2.0};
  //
  void onInit() override {
    auto GUIdomain = GUIDomain::enableGUI(defaultWindowDomain());
    auto& gui = GUIdomain->newGUI();
    gui.add(background).add(foreground).add(pointsize);
  }

  FileWatch file_watch;
  ShaderProgram stable;
  Mesh mesh;
  //
  void onCreate() override {
    // if we cannot compile the shader code, exit.
    if (!file_watch.maybe_recompile(stable, "v2", "..")) {
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
    g.blending(true);
    g.blendTrans();
    g.depthTesting(true);
    g.draw(mesh);
  }

  double t = 0;
  void onAnimate(double dt) override {
    t += dt;
    mesh.colors().back().set(foreground.get(), 1.0);

    file_watch.maybe_recompile(stable, "v2", "..");
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