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
