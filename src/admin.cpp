#include "admin.h"
#include <iostream>
using namespace std;

void printCurrentDifficulties(const DifficultyHashMap& diffMap) {
    Difficulty d;
    cout << "\n--- Current Difficulty Presets ---\n";
    for (int k = 1; k <= 3; ++k) {
        if (diffMap.get(k, d)) {
            cout << k << ". " << d.name << " (" << d.rows << "x" << d.cols
                 << ", mines=" << d.mines << ")\n";
        }
    }
}

void adminMenu(DifficultyHashMap& diffMap, Settings& settings, const vector<Score>& scores) {
    while (true) {
        cout << "\n===== ADMIN MENU =====\n";
        cout << "1) View difficulty presets\n";
        cout << "2) Edit a difficulty preset\n";
        cout << "3) Toggle settings (first-click-safe, chord, undo/redo, hints)\n";
        cout << "4) Flood mode (AUTO/BFS/DFS)\n";
        cout << "5) View basic stats\n";
        cout << "6) Search player's best WIN score\n";
        cout << "0) Exit admin\n";
        cout << "Choice: ";

        int ch;
        cin >> ch;

        if (ch == 0) break;

        if (ch == 1) {
            printCurrentDifficulties(diffMap);

        } else if (ch == 2) {
            printCurrentDifficulties(diffMap);
            cout << "Which one to edit (1/2/3)? ";
            int k; cin >> k;

            Difficulty d;
            if (!diffMap.get(k, d)) {
                cout << "Invalid.\n";
                continue;
            }

            cout << "Enter new rows cols mines (e.g. 9 9 10): ";
            int r, c, m;
            cin >> r >> c >> m;

            if (r <= 0 || c <= 0 || m < 0 || m >= r * c) {
                cout << "Invalid values.\n";
                continue;
            }

            d.rows = r; d.cols = c; d.mines = m;
            diffMap.put(k, d);
            cout << "Updated.\n";

        } else if (ch == 3) {
            cout << "\n--- Settings ---\n";
            cout << "1) firstClickSafe = " << (settings.firstClickSafe ? "ON" : "OFF") << "\n";
            cout << "2) enableChord    = " << (settings.enableChord ? "ON" : "OFF") << "\n";
            cout << "3) enableUndoRedo = " << (settings.enableUndoRedo ? "ON" : "OFF") << "\n";
            cout << "4) enableHints    = " << (settings.enableHints ? "ON" : "OFF") << "\n";
            cout << "Toggle which (1-4)? ";
            int t; cin >> t;

            if (t == 1) settings.firstClickSafe = !settings.firstClickSafe;
            else if (t == 2) settings.enableChord = !settings.enableChord;
            else if (t == 3) settings.enableUndoRedo = !settings.enableUndoRedo;
            else if (t == 4) settings.enableHints = !settings.enableHints;
            else cout << "Invalid.\n";

        } else if (ch == 4) {
            cout << "Flood mode: 0=AUTO, 1=BFS, 2=DFS. Enter: ";
            int fm; cin >> fm;
            if (fm < 0 || fm > 2) cout << "Invalid.\n";
            else settings.floodMode = fm;

        } else if (ch == 5) {
            int total = (int)scores.size();
            int wins = 0;
            for (size_t i = 0; i < scores.size(); ++i) if (scores[i].win) wins++;

            cout << "\n--- Stats ---\n";
            cout << "Total games recorded: " << total << "\n";
            cout << "Wins: " << wins << "\n";
            cout << "Losses: " << (total - wins) << "\n";

        } else if (ch == 6) {
            cout << "Enter player name (no spaces): ";
            string p; cin >> p;

            Score best;
            if (findBestScoreForPlayer(scores, p, best)) {
                cout << "Best WIN for " << p << ": " << best.difficultyName
                     << " in " << best.moves << " moves.\n";
            } else {
                cout << "No winning score found.\n";
            }

        } else {
            cout << "Invalid.\n";
        }
    }
}
