#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <iostream>
#include <vector>
#include <cstdlib>
#include "utils.h"
#include "types.h"
#include "settings.h"
#include "ds_structures.h"

using namespace std;

struct Cell {
    bool hasMine = false;
    bool revealed = false;
    bool flagged = false;
    int adjMines = 0;
};

struct MoveState {
    vector<vector<Cell>> boardState;
    bool gameOver;
    bool win;
    int moveCount;
};

class Minesweeper {
private:
    int rows, cols, mines;
    string difficultyName;

    vector<vector<Cell>> board;
    vector<vector<int>> adj;

    bool firstClickDone;
    bool gameOver;
    bool win;
    int moveCount;

    Settings settings;

    ArrayStack<MoveState> undoStack;
    LinkedStack<MoveState> redoStack;

public:
    Minesweeper(const Difficulty& d, const Settings& s);

    void play();

    int getMoveCount() const { return moveCount; }
    bool didWin() const { return win; }
    string getDifficultyName() const { return difficultyName; }

private:
    bool inBounds(int r, int c) const;
    int indexOf(int r, int c) const;
    Coord coordOf(int idx) const;

    void buildGraph();

    void saveStateForUndoIfEnabled();
    void restoreFromState(const MoveState& s);
    void undo();
    void redo();

    void printBoard(bool revealAll = false);

    int countAdjMines(int r, int c);
    void computeAllAdjCounts();

    void placeMinesUnbiasedNoSafety();
    void placeMinesFirstClickSafe(int safeR, int safeC);

    bool reveal(int r, int c);
    bool toggleFlag(int r, int c);
    bool chord(int r, int c);
    bool checkWin() const;

    void floodFillZeros_BFS(int N, int startId);
    void floodFillZeros_DFS(int N, int startId);

    bool hintRevealOneSafeCell();
};

#endif
