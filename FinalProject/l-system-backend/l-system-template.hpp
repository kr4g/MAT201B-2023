#include <vector>
#include <map>
#include <string>

// using namespace al;
using namespace std;

// double r() { return rnd::uniformS(); }
// RGB c() { return RGB(rnd::uniform(), rnd::uniform(), rnd::uniform()); }

// The data structure for an L-system
struct LSystem {
    // For use by the L-System string parser
    std::vector<char> variables;          // variables correspond to actions that will be drawn/rendered (e.g. draw a line, make a sound, etc.)
    std::vector<char> constnats;          // constnats correspond to some change in state (e.g. change direction, start a new branch, etc.)

    // -------------------------------------------------
    // ----------------- L-System Data -----------------

    std::string axiom;                          // the initial string

    float angle;                                // global draw angle in degrees (could also be interpreted as FM modulator/carrior ratio, etc.)
    float length;                               // initial draw length          (could also be interpreted as sound event duration, etc.)
    float scaleFactor;                          // the rate at which consecutive render actions are scaled (e.g. 0.5 means each action is half the size of the previous one)

    std::map<char, std::string>  rules;         // substitution rules for each variable and constant
    // std::map<char, std::string>* renderActions; // draw/sound actions for each variable and constant (NOTE:  this is a pointer to a map, so that we can easily swap out the 
                                                //                                                           map for a different one. e.g. draw actions vs. sound actions)

    // -------------------------------------------------
    // -------------------------------------------------

    // Constructors
    LSystem(std::vector<char> v, std::vector<char> c, std::string a, float ang, float l, float sf, std::map<char, std::string> r) {
        variables = v;
        constnats = c;
        axiom = a;
        angle = ang;
        length = l;
        scaleFactor = sf;
        rules = r;
        // renderActions = ra;
    }

    // // Set renderActions pointer
    // void setRenderActions(std::map<char, std::string>* ra) { renderActions = ra; }
};

// Generate the L-System string given an L-System and the number of iterations
// 
// NOTE:  this only generates the string, it does not draw/render anything.
// Render actions (e.g. draw a line, make a sound, etc.) are handled by a separate
// string parser that interprets the L-System string as a sequence of actions.
std::string generateString(LSystem lsys, int n) {
    std::string result = "";
    std::string current = lsys.axiom;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < current.length(); j++) {
            char c = current[j];
            if (lsys.rules.find(c) != lsys.rules.end()) {
                result += lsys.rules[c];
            } else {
                result += c;
            }
        }
        current = result;
        result = "";
    }
    return current;
}
