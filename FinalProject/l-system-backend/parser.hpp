#include "l-system-types.hpp"
#include "renderActions.hpp"

#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

using namespace al;
using namespace std;

#include <utility>

  // VISUAL RENDERING
  void renderLSystem(const std::string &str, const al::Vec3f &startPoint, float decay, al::Mesh &mesh) {

    // float decayRate = 0.9;  // *= 0.9 each iteration so that the draw actions become smaller and smaller

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
        if (c == '0') {  // draw a line segment ending in a leaf
            mesh.vertex(state.back().pos());
            state.back().pos() += state.back().uf(); // uf means "unit forward", the vector that points front
            mesh.vertex(state.back().pos());
        } else if (c == '1') {  // draw a line segment
            // cout << "branch: " << p.pos() << endl;
            mesh.vertex(state.back().pos());
            state.back().pos() += state.back().uf(); // uf means "unit forward", the vector that points front
            mesh.vertex(state.back().pos());
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

  void generateAndRenderBranches(std::vector<al::Vec3f> &vertices, const LSystem &lsys, float probability, std::pair range) {
    // mesh.compress(); // vertices + indices represetnation
    // mesh.vertices(); // ...is the list of unique vertices
    // mesh.indices();  // ...is the list of connectedness of vertices
    // mesh.compress();
    int n = range.second;  // TODO: random selection from range
    for (auto vert: vertices) {
        if (probability > rnd::uniform()) {  // randomly generate branches
            al::Mesh m(al::Mesh::LINES);
            renderLSystem(generateString(lsys, n), vert, 0.9, m);
        }
        // cout << vert << endl;
        // mesh.vertex(vert);
        // mesh.color(1, 1, 1);
    }
  }