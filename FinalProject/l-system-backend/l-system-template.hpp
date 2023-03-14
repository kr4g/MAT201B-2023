#include <vector>
#include <map>
#include <string>

// using namespace al;
using namespace std;

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
    }

    // // overloaded extraction operator
    // friend std::ostream& operator<<(std::ostream& os, const LSystem& lsys) {
    //     os << "Variables: ";
    //     for (int i = 0; i < lsys.variables.size(); i++) {
    //         os << lsys.variables[i] << " ";
    //     }
    //     os << "\nConstnats: ";
    //     for (int i = 0; i < lsys.constnats.size(); i++) {
    //         os << lsys.constnats[i] << " ";
    //     }
    //     os << "\nAxiom: " << lsys.axiom << "\nAngle: " << lsys.angle << "\nLength: " << lsys.length << "\nScale Factor: " << lsys.scaleFactor << "\nRules: " << "\nScale Factor: " << lsys.scaleFactor << "\n";
    // }
};
