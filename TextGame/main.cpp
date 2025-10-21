#include <bits/stdc++.h>
#include <thread>
#include <chrono>
using namespace std;

/*
================================================================================
ESCAPE — Text Adventure 

COMMENTARY SUMMARY
------------------------
DATA STRUCTURE:
- The story is modeled as a small directed graph.
- Each "Scene" node contains:
    title:         scene heading
    body:          narrative text lines
    options:       vector<Option> — each choice points to another scene ID
    isEnding:      marks final scenes
    endingTag:     labels ending type ("UNLOCK", "CAUGHT", "ESCAPE")
- Scenes are stored in a vector<Scene>, where index = node ID.

PLAYER STATE & LOGIC:
- The State struct tracks flags:
    hasKey:     found in the Cell; required to unlock the Gate (good ending)
    injured:    set if you reach into the trapped chest with bare hands
    foundHook:  found in Armoury; allows safe chest access if you lack a key
- Choices are stored as Options with a number and next scene ID.
- Dynamic checks override scene transitions (e.g., safe/unsafe chest opening).
- Input is number-driven and validated
================================================================================
*/

// ============ helpers ============
void pause_ms(int ms) { this_thread::sleep_for(chrono::milliseconds(ms)); }

void print_line(const string& s, int delay_per_char = 0) {
    for (char c : s) {
        cout << c << flush;
        if (delay_per_char > 0) pause_ms(delay_per_char);
    }
    cout << "\n";
}

void rule(char ch='-', int n=60) { cout << string(n, ch) << "\n"; }

// ============ Narrative Data Structure ============
struct Option { string text; int nextId; };

struct Scene {
    string title;
    vector<string> body;      // multi-line text
    vector<Option> options;   // choices to navigate
    bool isEnding = false;    // marks nodes
    string endingTag;         // UNLOCK, CAUGHT, ESCAPE
};

// ============ Story World State ============
struct State { bool hasKey = false; bool injured = false; bool foundHook = false; };

// Render scene text with small pauses
void renderBody(const vector<string>& lines) {
    for (const auto& ln : lines) { print_line(ln); pause_ms(400); }
}

// Get numeric input safely
int getChoice(int n) {
    while (true) {
        cout << "> " << flush;
        string in; if (!getline(cin, in)) return -1;
        string digits; for (char c : in) if (isdigit(c)) digits.push_back(c);
        if (digits.empty()) { print_line("Please enter a number matching an option."); continue; }
        int k = stoi(digits);
        if (k >= 1 && k <= n) return k;
        print_line("That isn't a valid option number—try again.");
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // ============ Story Graph ============
    // Scene index map:
    // 0=Intro, 1=Corridor, 2=Armory, 3=Cell, 4=Courtyard, 5=Gate,
    // 6=Roof, 7=Unlock(GOOD), 8=Caught(BAD), 9=Escape(ALT)
    vector<Scene> scenes(10);

    scenes[0] = {
        "The Wake-Up",
        {
            "You wake to darkness. A cold stone floor.",
            "A door to the cell is open. Beyond it, a lantern flickers in a corridor."
        },
        {
            {"Step into the corridor", 1},
            {"Search the cell first", 3}
        },
        false, ""
    };

    scenes[1] = {
        "The Corridor",
        {
            "The corridor is dark.",
            "Left leads towards a low noise; right leads towards air and moonlight."
        },
        {
            {"Go left toward the noise", 2},
            {"Go right toward the moonlight", 4},
            {"Return to the cell", 3}
        },
        false, ""
    };

    scenes[2] = {
        "The Armoury",
        {
            "You see a chest; racks of weapons line the walls.",
            "Something shines just out of reach."
        },
        {
            {"Reach into the chest", 3}, // dynamic override handles outcomes
            {"Search the racks", 2},     // may find hook
            {"Head back to the corridor", 1}
        },
        false, ""
    };

    scenes[3] = {
        "The Cell",
        {
            "You sweep the floor with your hands.",
            "You find a small iron key. You take it."
        },
        {
            {"Return to the corridor", 1}
        },
        false, ""
    };

    scenes[4] = {
        "The Courtyard",
        {
            "Moonlight lights an open courtyard. A gate can be seen to the east;",
            "a ladder climbs to the roof."
        },
        {
            {"Approach the gate", 5},
            {"Climb the ladder to the roof", 6},
            {"Back to the corridor", 1}
        },
        false, ""
    };

    scenes[5] = {
        "The Gate",
        {
            "The gate is secured with a heavy lock.",
            "You hear a security patrol in the distance."
        },
        {
            {"Try the lock with your key", 7},
            {"Force the gate open", 8},
            {"Retreat to the courtyard", 4}
        },
        false, ""
    };

    scenes[6] = {
        "The Roof",
        {
            "From here, you see the outer wall and a narrow ledge.",
            "You could reach the wall if you jump."
        },
        {
            {"Leap to the outer wall", 9},
            {"Climb back down", 4}
        },
        false, ""
    };

    scenes[7] = {
        "Forest Road",
        {
            "The lock clicks. The gate swings open.",
            "You disappear into the trees."
        },
        {}, true, "UNLOCK"
    };

    scenes[8] = {
        "Broken Gate",
        {
            "You throw yourself at the bars.",
            "The gate shudders; sirens start and torches can be seen coming towards you."
        },
        {}, true, "CAUGHT"
    };

    scenes[9] = {
        "The Narrow Ledge",
        {
            "You grab the ledge and make it to the outer wall.",
            "You limp away: free, but wounded."
        },
        {}, true, "ESCAPE"
    };

    // ============ Game Loop ============
    State state;
    int current = 0;

    rule('=');
    print_line("ESCAPE");
    rule('=');
    pause_ms(500);

    while (true) {
        rule();
        cout << "[" << scenes[current].title << "]\n";
        rule();

        // Pickup key automatically when entering the Cell
        if (current == 3 && !state.hasKey) {
            state.hasKey = true;
        }

        renderBody(scenes[current].body);
        pause_ms(300);

        // Armoury dynamic hinting
        if (current == 2) {
            print_line("");
            if (state.hasKey || state.foundHook) {
                print_line("With what you have, you could get the item in the chest safely now.");
            } else {
                print_line("Reaching in bare-handed looks risky.");
            }
            pause_ms(250);
        }

        // Handle endings
        if (scenes[current].isEnding) {
            rule();
            if (scenes[current].endingTag == "UNLOCK") {
                print_line("ENDING: FREEDOM");
            } else if (scenes[current].endingTag == "ESCAPE") {
                print_line("ENDING: THE LONG WALK");
            } else if (scenes[current].endingTag == "CAUGHT") {
                print_line("ENDING: TRAPPED");
            } else {
                print_line("ENDING");
            }
            rule();
            print_line("Thanks for playing!");
            break;
        }

        // Display options
        vector<Option> opts = scenes[current].options;
        cout << "\n";
        for (size_t i = 0; i < opts.size(); ++i) {
            cout << "  (" << (i+1) << ") " << opts[i].text << "\n";
        }

        int sel = getChoice((int)opts.size());
        cout << "You chose: (" << sel << ") " << opts[sel - 1].text << "\n\n" << flush;

        // ===== Dynamic consequences =====

        // Armoury option 1: Reach into the chest
        if (current == 2 && sel == 1) {
            if (state.hasKey || state.foundHook) {
                print_line("You snag the prize safely, it's a folded guard map.");
                print_line("It might help you go through the courtyard unseen...");
                pause_ms(400);
                current = 4; // move to Courtyard
                continue;
            } else {
                print_line("A hidden blade snaps. Your hand is cut. You wrap it.");
                state.injured = true;
                pause_ms(400);
                current = 3; // retreat to Cell
                continue;
            }
        }

        // Armoury option 2: Search the racks (context-sensitive)
        if (current == 2 && sel == 2) {
            if (state.hasKey) {
                print_line("You glance over the racks, but you already have what you need.");
            } 
            else if (!state.foundHook) {
                print_line("You rummage along the racks and find a bent iron hook—perfect for fishing things out.");
                state.foundHook = true;
            } 
            else {
                print_line("You check again. Nothing else but dust.");
            }
            current = 2;
            continue;
        }

        // Gate: try the lock
        if (current == 5 && sel == 1) {
            if (state.hasKey) {
                current = 7; // GOOD
                continue;
            } else {
                print_line("You fumble at the bars with empty hands. Torches shine in your face. It's too late.");
                current = 8; // BAD
                continue;
            }
        }

        // Roof: injured jump 
        if (current == 6 && sel == 1 && state.injured) {
            print_line("You're in pain but you jump.");
            pause_ms(350);
        }

        // Default transition
        current = opts[sel - 1].nextId;
    }

    return 0;
}
