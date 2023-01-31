#pragma once

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
 public:
  f::file_time_type last;
  bool changed(const char* version, const char* path) {
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

  bool maybe_recompile(al::ShaderProgram& program,  //
                       const char* version,         //
                       const char* path) {
    if (!changed(version, path)) {
      return false;
    }

    std::cout <<                                             //
        "///// Recompiling shaders... ////////////////////"  //
              << std::endl;

    al::ShaderProgram testing;
    if (compile(testing, version, path)) {
      compile(program, version, path);
      std::cout << "...SUCCESS!" << std::endl;
    } else {
      std::cout << "...FAILURE!" << std::endl;
      return false;
    }
    return true;
  }
};