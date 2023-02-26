#include "al/app/al_App.hpp"
#include "al/app/al_GUIDomain.hpp"
#include "al/math/al_Random.hpp"

#include <vector>
#include <map>
#include <string>

using namespace al;
using namespace std;

// struct State {
//         Vec3f pos;  // position
//         // Quatf rot;  // orientation
//         RGB color;  // color
// };

double r() { return rnd::uniformS(); }
RGB c() { return RGB(rnd::uniform(), rnd::uniform(), rnd::uniform()); }
// struct for l-system axiom sets
// Example:
// Algae = {
//     'axiom': 'A',
//     'rules': {
//         'A': 'AB',
//         'B': 'A'
//     },
//     'variables': ('A', 'B'),
//     'constants': None
// }
struct LSystem {
    char axiom;
    std::map<char, std::string> rules;
    std::vector<char> variables;
    std::vector<char> constants;
};

std::string generateString(LSystem lsys, int n) {
    std::string result = "";
    // cast char to string
    std::string current = std::string(1, lsys.axiom);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < current.length(); j++) {
            if (lsys.rules.find(current[j]) != lsys.rules.end()) {
                result += lsys.rules[current[j]];
            } else {
                result += current[j];
            }
        }
        current = result;
        result = "";
    }
    return current;
}


// void evaluate(Mesh &mesh, const LSystem &lsys, const char &axiom) {
//     Vec3f current = state.back().pos;
//     // cout << current << endl;
//     if (axiom == 'A') {  // if 'A', connect from last branch to new branch
//         // cout << "A" << endl;
//         drawLine(mesh, current, current + Vec3f(0, 1, 0), RGB(0, 1, 0.1), RGB(0, 0.1, 1));
//     } else if (axiom == 'B') {  // if 'B', connect from the origin to new branch
//         // cout << "B" << endl;
//         // if 'B', connect from the origin to new branch
//         drawLine(mesh, Vec3f(0, 0, 0), Vec3f(r(), r(), r()), RGB(0.5, 0.1, 0.5), RGB(0.25, 0.5, 0.5));
//     }
// }

// map variables/constants to draw commands
// Example:
// 'A':  move forward 1 unit
//       color is random shade of green
// 'B':  move backward 0.667*1.0 units
//       color is random shade of brown


// void drawLSystem(Mesh &mesh, LSystem &lsys, int n) {
//     // std::string result = generateString(lsys, n);
//     // std::string result = "AB";
//     // std::cout << result << std::endl;

//     // start at the origin
//     mesh.vertex(0, 0, 0);
//     mesh.color(1, 0, 0);
    

//     // State s = {mesh.vertices().back(), Quatf(), RGB(0, 0.1, 1)};
//     // state.push_back(State());

//     for (int i = 0; i < n; i++) {
//         Vec3f current = mesh.vertices().back();
//         drawLine(mesh, current, Vec3f(r(), r(), r()), RGB(0.5, 0.1, 0.5), RGB(0.25, 0.5, 0.5));
//         // if (result[i] == 'A') {
//         //     state.push_back(State());
//         // }
//         // evaluate(mesh, lsys, result[i]);
//     }
// }


