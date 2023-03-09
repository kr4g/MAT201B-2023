#include <iostream>
#include <vector>
#include <stack>
#include <cmath>
using namespace std;

// ------------------------------------------------------------
// FOR TESTING
// ------------------------------------------------------------


const double PI = 3.14159265358979323846;

// ------------------------------------------------------------
// L-system data
const string variables = "XF";
const string constants = "+-[]";
const string start = "X";
const string rule1 = "F+[[X]-X]-F[-FX]+X";
const string rule2 = "FF";
const int angle = 25;
// ------------------------------------------------------------

// Apply L-system rules
string applyRules(string input, int iterations) {
    if(iterations == 0) {
        return input;
    }
    string output = "";
    for(int i = 0; i < input.length(); i++) {
        char c = input[i];
        if(variables.find(c) != string::npos) {
            if(c == 'X') {
                output += rule1;
            } else if(c == 'F') {
                output += rule2;
            }
        } else if(constants.find(c) != string::npos) {
            output += c;
        }
    }
    return applyRules(output, iterations-1);
}


// ------------------------------------------------------------
// State Struct
struct state {
    double x, y;
    double angle;
};

// Function to convert L-system string to turtle graphics commands
vector<pair<double,double>> createPoints(string input) {
    vector<pair<double,double>> points;
    stack<state> state_stack;
    state s;
    s.x = 0;
    s.y = 0;
    s.angle = 0;
    state_stack.push(s);
    for(int i = 0; i < input.length(); i++) {
        char c = input[i];
        if(c == 'F') {
            double x1 = s.x + cos(s.angle*PI/180.0);
            double y1 = s.y + sin(s.angle*PI/180.0);
            points.push_back(make_pair(s.x,s.y));
            points.push_back(make_pair(x1,y1));
            s.x = x1;
            s.y = y1;
        } else if(c == '+') {
            s.angle += angle;
        } else if(c == '-') {
            s.angle -= angle;
        } else if(c == '[') {
            state_stack.push(s);
        } else if(c == ']') {
            s = state_stack.top();
            state_stack.pop();
        }
    }
    return points;
}


int main() {
    // Apply L-system rules
    string lSystemString = applyRules(start, 5);
    cout << lSystemString << endl;
    
    // Convert L-system string to turtle graphics commands
    vector<pair<double,double>> points = createPoints(lSystemString);
    
    // Print the vertices of the resulting graph
    for(int i = 0; i < points.size(); i++) {
        cout << "(" << points[i].first << ", " << points[i].second << ")" << endl;
    }
    
    return 0;
}
