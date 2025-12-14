#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "ds_structures.h"
#include "admin.h"
#include "minesweeper.h"
#include "scores.h"
#include "settings.h"

using namespace std;

int main() {
    srand((unsigned int)time(nullptr));

    // Difficulty presets in HashMap
    DifficultyHashMap diffMap(17);
    diffMap.put(1, {9, 9, 10,  "Easy"});
    diffMap.put(2, {16, 16, 40, "Medium"});
    diffMap.put(3, {16, 30, 99, "Hard"});

    // Load scores
    vector<Score> scores;
    ScoreCircularDoublyList scoreList;
    ScoreTreeNode* scoreTreeRoot = nullptr;

    loadScoresFromFile("scores.txt", scores, scoreList, scoreTreeRoot);

    // Settings (admin can modify)
    Settings settings;

    cout << "Mode:\n";
    cout << "1) Player\n";
    cout << "2) Admin\n";
    cout << "Choice: ";
    int mode;
    cin >> mode;

    if (mode == 2) {
        adminMenu(diffMap, settings, scores);
    }

    string playerName;
    cout << "\nEnter your name (no spaces): ";
    cin >> playerName;

    cout << "Select difficulty:\n";
    printCurrentDifficulties(diffMap);
    cout << "Choice: ";
    int choice;
    cin >> choice;

    Difficulty d;
    if (!diffMap.get(choice, d)) {
        diffMap.get(1, d);
        cout << "Invalid choice. Defaulting to Easy.\n";
    }

    Minesweeper game(d, settings);
    game.play();

    // Record score
    Score currentScore;
    currentScore.player = playerName;
    currentScore.difficultyName = game.getDifficultyName();
    currentScore.moves = game.getMoveCount();
    currentScore.win = game.didWin();

    scores.push_back(currentScore);
    scoreList.append(currentScore);
    insertScoreTree(scoreTreeRoot, currentScore);

    // Use merge sort as official saved order
    mergeSortScores(scores);
    saveScoresToFile("scores.txt", scores);

    cout << "\n=== High Scores (BST inorder) ===\n";
    inorderPrintScores(scoreTreeRoot);

    cout << "\n=== All Scores (Circular Doubly List) ===\n";
    scoreList.printAll();

    deleteScoreTree(scoreTreeRoot);
    return 0;
}
