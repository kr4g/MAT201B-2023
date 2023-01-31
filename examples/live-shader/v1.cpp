//////////////////////////////////////////////////////////////////////////////
/// Karl Yerkes //////////////////////////////////////////////////////////////
/// MAT 201B /////////////////////////////////////////////////////////////////
/// 2022-03-03 ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/// Includes /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define _LIBCPP_NO_EXPERIMENTAL_DEPRECATION_WARNING_FILESYSTEM 1
#include <experimental/filesystem>
#include <fstream>
#include <vector>

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"

//////////////////////////////////////////////////////////////////////////////
/// Supporting Functions /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::string slurp(const std::string& fileName) {
  std::fstream file(fileName);
  std::string returnValue = "";
  while (file.good()) {
    std::string line;
    std::getline(file, line);
    returnValue += line + "\n";
  }
  return returnValue;
}

bool compile(al::ShaderProgram& p, const char* version, const char* path) {
  return p.compile(slurp(std::string(path) + "/" + version + "/vertex.glsl"),
                   slurp(std::string(path) + "/" + version + "/fragment.glsl"),
                   slurp(std::string(path) + "/" + version + "/geometry.glsl"));
}

namespace f = std::experimental::filesystem;
class FileWatch {
  std::unordered_map<const char*, f::file_time_type> last_known{
      {"fragment.glsl", f::file_time_type()},
      {"geometry.glsl", f::file_time_type()},
      {"vertex.glsl", f::file_time_type()}};

 public:
  bool changed_(const char* version) {
    bool has_changed{false};

    for (auto e : last_known) {
      auto filename(e.first);
      auto path = f::current_path() / version / filename;
      auto time = f::last_write_time(path);
      if (time > last_known[filename]) {
        last_known[filename] = time;
        has_changed = true;
      }
    }
    return has_changed;
  }

  f::file_time_type last;
  bool changed(const char* version, const char* path) {
    using Entry = f::directory_entry;
    auto directory = f::current_path() / path / version;
    // this checks _all_ files, not just the *.glsl files!
    for (auto const& entry : f::directory_iterator(directory)) {
      auto _new = entry.last_write_time();
      if (_new > last) {
        last = _new;
        return true;
      }
    }
    return false;
  }
};

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
  ShaderProgram stable, testing;
  Mesh mesh;
  //
  void onCreate() override {
    // if we cannot compile the shader code, exit.
    if (!compile(stable, "v1", "..")) {
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

    //
    // (file_watch.changed("v1", "..")) {
    std::cout << "///// Recompiling shaders... ////////////////////"
              << std::endl;
    if (compile(testing, "v1", "..")) {
      compile(stable, "v1", "..");
      std::cout << "...SUCCESS!" << std::endl;
    } else {
      std::cout << "...FAILURE!" << std::endl;
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