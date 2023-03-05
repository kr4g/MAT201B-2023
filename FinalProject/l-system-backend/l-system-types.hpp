#include "l-system-template.hpp"

// L-system types ("Presets")
enum class LSystemType {
    BOURKE_BUSH_2,
    BOURKE_ALGAE_2,
    // TREE,
    // KochCurve,
    // SierpinskiTriangle,
    // DragonCurve,
    // HilbertCurve
};

const std::map<LSystemType, std::string> TYPE_NAMES {
    {LSystemType::BOURKE_BUSH_2, "BOURKE_BUSH_2"},
    {LSystemType::BOURKE_ALGAE_2, "BOURKE_ALGAE_2"}
};

// Definitions for L-System types
const std::map<LSystemType, LSystem> TYPE_DEF {
    {LSystemType::BOURKE_BUSH_2, LSystem(
        {'F'},  // VARIABLES
        {'[', ']', '+', '-'},  // CONSTANTS
        "F",  // AXIOM
        22.5f,  // ANGLE
        1.0f,  // LENGTH
        0.95f,  // SCALE FACTOR
        {{'F', "FF+[+F-F-F]-[-F+F+F]"}})  // RULES
    },

    {LSystemType::BOURKE_ALGAE_2, LSystem(
        {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y'},  // VARIABLES
        {'[', ']', '+', '-'},  // CONSTANTS
        "aF",  // AXIOM
        12.0f,  // ANGLE
        1.0f,  // LENGTH
        0.75f,  // SCALE FACTOR
        {
            {'a', "FFFFFy[++++n][----t]fb"},
            {'b', "+FFFFFy[++++n][----t]fc"},
            {'c', "FFFFFy[++++n][----t]fd"},
            {'d', "-FFFFFy[++++n][----t]fe"},
            {'e', "FFFFFy[++++n][----t]fg"},
            {'g', "FFFFFy[+++fa]fh"},
            {'h', "FFFFFy[++++n][----t]fi"},
            {'i', "+FFFFFy[++++n][----t]fj"},
            {'j', "FFFFFy[++++n][----t]fk"},
            {'k', "-FFFFFy[++++n][----t]fl"},
            {'l', "FFFFFy[++++n][----t]fm"},
            {'m', "FFFFFy[---fa]fa"},
            {'n', "ofFFF"},
            {'o', "fFFFp"},
            {'p', "fFFF[-s]q"},
            {'q', "fFFF[-s]r"},
            {'r', "fFFF[-s]"},
            {'s', "fFfF"},
            {'t', "ufFFF"},
            {'u', "fFFFv"},
            {'v', "fFFF[+s]w"},
            {'w', "fFFF[+s]x"},
            {'x', "fFFF[+s]"},
            {'y', "Fy"}
        })
    }
};

