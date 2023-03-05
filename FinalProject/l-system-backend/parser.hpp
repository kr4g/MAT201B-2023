#include "l-system-types.hpp"
#include "renderActions.hpp"

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;
using namespace std;

#include <utility>

  // VISUAL RENDERING
  void renderLSystem(const LSystem &lsys, const std::string &str, const al::Vec3f &startPoint, al::Mesh &mesh) {

    // float decayRate = lsys.scaleFactor;

    struct State : al::Pose {
        al::Color color;

        State(const al::Vec3f& position, const al::Color& color)
            : al::Pose(position), color(color)
        {}
    };

    std::vector<State> state;  // push_back / pop_back
    // std::vector<al::Pose> state;  // push_back / pop_back
    
    state.push_back(State{startPoint, al::Color(1, 1, 1)});
    // state.push_back(al::Pose(startPoint));
    // cout << "origin: " << state.back().pos() << endl;
    // al::Pose& p(state.back());

    // start the l-system at its origin
    mesh.vertex(state.back().pos());
    // mesh.color(state.back().color);
    mesh.color(al::Color(1, 1, 1));

    // stack.back(); // top
    // stack.pop_back(); // pop
    // stack.push_back(stack.back()); // push a copy of top

    // mesh.vertices(); // ...is the list of redundant vertices
    // mesh.compress(); // vertices + indices represetnation
    // mesh.vertices(); // ...is the list of unique vertices
    // mesh.indices();  // ...is the list of connectedness of vertices

    for (char c : str) {
        if (c == 'F') {  // Move forward by `LSystem.length` drawing a line
            // ...
        } else if (c == '+') {
            
        } else if (c == '-') {

        } else if (c == '[') {  // CHANGE CURRENT BRANCH
            // Push current state onto stack
            state.push_back(state.back());
        } else if (c == ']') {  // RESTORE PREVIOUS BRANCH
            // Pop previous state from stack
            state.pop_back();
        }
    }
    // g.draw(mesh);
  }

  // AUDIO RENDERING
  void renderLSystem(const LSystem &lsys, const std::string &str, float f) {


  }





//   void generateAndRenderBranches(std::vector<al::Vec3f> &vertices, const LSystem &lsys, float probability, std::pair<int, int> range) {
//     // mesh.compress(); // vertices + indices represetnation
//     // mesh.vertices(); // ...is the list of unique vertices
//     // mesh.indices();  // ...is the list of connectedness of vertices
//     // mesh.compress();
//     int n = range.second;  // TODO: random selection from range
//     for (auto vert: vertices) {
//         if (probability > rnd::uniform()) {  // randomly generate branches
//             al::Mesh m(al::Mesh::LINES);
//             renderLSystem(generateString(lsys, n), vert, 0.9, m);
//         }
//         // cout << vert << endl;
//         // mesh.vertex(vert);
//         // mesh.color(1, 1, 1);
//     }
//   }