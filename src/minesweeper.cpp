#include "minesweeper.h"

Minesweeper::Minesweeper(const Difficulty& d, const Settings& s)
    : rows(d.rows), cols(d.cols), mines(d.mines), difficultyName(d.name),
      firstClickDone(false), gameOver(false), win(false), moveCount(0),
      settings(s), undoStack(80) {

    board.assign(rows, vector<Cell>(cols));
    buildGraph();

    if (!settings.firstClickSafe) {
        placeMinesUnbiasedNoSafety();
        firstClickDone = true;
    }
}

bool Minesweeper::inBounds(int r, int c) const {
    return r >= 0 && r < rows && c >= 0 && c < cols;
}

int Minesweeper::indexOf(int r, int c) const { return r * cols + c; }

Coord Minesweeper::coordOf(int idx) const {
    Coord cc;
    cc.r = idx / cols;
    cc.c = idx % cols;
    return cc;
}

void Minesweeper::buildGraph() {
    int n = rows * cols;
    adj.assign(n, vector<int>());

    int dr[8] = {-1,-1,-1, 0,0, 1,1,1};
    int dc[8] = {-1, 0, 1,-1,1,-1,0,1};

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int id = indexOf(r,c);
            for (int k = 0; k < 8; ++k) {
                int nr = r + dr[k];
                int nc = c + dc[k];
                if (inBounds(nr, nc)) adj[id].push_back(indexOf(nr, nc));
            }
        }
    }
}

void Minesweeper::saveStateForUndoIfEnabled() {
    if (!settings.enableUndoRedo) return;
    MoveState s;
    s.boardState = board;
    s.gameOver = gameOver;
    s.win = win;
    s.moveCount = moveCount;
    undoStack.push(s);
}

void Minesweeper::restoreFromState(const MoveState& s) {
    board = s.boardState;
    gameOver = s.gameOver;
    win = s.win;
    moveCount = s.moveCount;
}

void Minesweeper::undo() {
    if (undoStack.empty()) {
        cout << "Nothing to undo.\n";
        return;
    }
    MoveState cur;
    cur.boardState = board;
    cur.gameOver = gameOver;
    cur.win = win;
    cur.moveCount = moveCount;
    redoStack.push(cur);

    MoveState prev = undoStack.top();
    undoStack.pop();
    restoreFromState(prev);
}

void Minesweeper::redo() {
    if (redoStack.empty()) {
        cout << "Nothing to redo.\n";
        return;
    }
    MoveState cur;
    cur.boardState = board;
    cur.gameOver = gameOver;
    cur.win = win;
    cur.moveCount = moveCount;
    undoStack.push(cur);

    MoveState nxt = redoStack.top();
    redoStack.pop();
    restoreFromState(nxt);
}

void Minesweeper::printBoard(bool revealAll) {
    cout << "\n    ";
    for (int c = 0; c < cols; ++c) cout << (c % 10) << ' ';
    cout << "\n   ";
    for (int c = 0; c < cols; ++c) cout << "--";
    cout << "-\n";

    for (int r = 0; r < rows; ++r) {
        cout << (r % 10) << " | ";
        for (int c = 0; c < cols; ++c) {
            const Cell& cell = board[r][c];
            char ch = '#';

            if (revealAll || cell.revealed) {
                if (cell.hasMine) ch = '*';
                else if (cell.adjMines > 0) ch = char('0' + cell.adjMines);
                else ch = '.';
            } else {
                ch = (cell.flagged ? 'F' : '#');
            }
            cout << ch << ' ';
        }
        cout << "\n";
    }
}

int Minesweeper::countAdjMines(int r, int c) {
    int cnt = 0;
    int dr[8] = {-1,-1,-1, 0,0, 1,1,1};
    int dc[8] = {-1, 0, 1,-1,1,-1,0,1};
    for (int k = 0; k < 8; ++k) {
        int nr = r + dr[k], nc = c + dc[k];
        if (inBounds(nr, nc) && board[nr][nc].hasMine) cnt++;
    }
    return cnt;
}

void Minesweeper::computeAllAdjCounts() {
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            board[r][c].adjMines = countAdjMines(r, c);
}

void Minesweeper::placeMinesUnbiasedNoSafety() {
    vector<Coord> positions;
    positions.reserve(rows * cols);

    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c) {
            Coord p; p.r = r; p.c = c;
            positions.push_back(p);
        }

    int n = (int)positions.size();
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        mySwap(positions[i], positions[j]);
    }

    for (int i = 0; i < mines && i < n; ++i) {
        board[positions[i].r][positions[i].c].hasMine = true;
    }
    computeAllAdjCounts();
}

void Minesweeper::placeMinesFirstClickSafe(int safeR, int safeC) {
    vector<Coord> positions;
    positions.reserve(rows * cols - 1);

    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c) {
            if (r == safeR && c == safeC) continue;
            Coord p; p.r = r; p.c = c;
            positions.push_back(p);
        }

    int n = (int)positions.size();
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        mySwap(positions[i], positions[j]);
    }

    for (int i = 0; i < mines && i < n; ++i) {
        board[positions[i].r][positions[i].c].hasMine = true;
    }
    computeAllAdjCounts();
}

void Minesweeper::floodFillZeros_BFS(int N, int startId) {
    if (N <= 400) {
        ArrayQueue<int> q(N + 10);
        vector<bool> visited(N, false);

        q.push(startId);
        visited[startId] = true;

        while (!q.empty()) {
            int id = q.front(); q.pop();

            for (size_t i = 0; i < adj[id].size(); ++i) {
                int nid = adj[id][i];
                Coord nc = coordOf(nid);
                Cell& neighbor = board[nc.r][nc.c];

                if (neighbor.revealed || neighbor.flagged || neighbor.hasMine) continue;
                neighbor.revealed = true;

                if (!visited[nid] && neighbor.adjMines == 0) {
                    visited[nid] = true;
                    q.push(nid);
                }
            }
        }
    } else {
        LinkedQueue<int> q;
        vector<bool> visited(N, false);

        q.push(startId);
        visited[startId] = true;

        while (!q.empty()) {
            int id = q.front(); q.pop();

            for (size_t i = 0; i < adj[id].size(); ++i) {
                int nid = adj[id][i];
                Coord nc = coordOf(nid);
                Cell& neighbor = board[nc.r][nc.c];

                if (neighbor.revealed || neighbor.flagged || neighbor.hasMine) continue;
                neighbor.revealed = true;

                if (!visited[nid] && neighbor.adjMines == 0) {
                    visited[nid] = true;
                    q.push(nid);
                }
            }
        }
    }
}

void Minesweeper::floodFillZeros_DFS(int N, int startId) {
    ArrayStack<int> st(200);
    vector<bool> visited(N, false);

    st.push(startId);
    visited[startId] = true;

    while (!st.empty()) {
        int id = st.top(); st.pop();

        for (size_t i = 0; i < adj[id].size(); ++i) {
            int nid = adj[id][i];
            Coord nc = coordOf(nid);
            Cell& neighbor = board[nc.r][nc.c];

            if (neighbor.revealed || neighbor.flagged || neighbor.hasMine) continue;
            neighbor.revealed = true;

            if (!visited[nid] && neighbor.adjMines == 0) {
                visited[nid] = true;
                st.push(nid);
            }
        }
    }
}

bool Minesweeper::reveal(int r, int c) {
    Cell& cell = board[r][c];

    if (!firstClickDone) {
        if (settings.firstClickSafe) placeMinesFirstClickSafe(r, c);
        else placeMinesUnbiasedNoSafety();
        firstClickDone = true;
    }

    if (cell.revealed || cell.flagged) return false;
    cell.revealed = true;

    if (cell.hasMine) {
        gameOver = true;
        cout << "Boom! You hit a mine.\n";
        printBoard(true);
        return true;
    }

    if (cell.adjMines == 0) {
        int N = rows * cols;
        if (settings.floodMode == 1) floodFillZeros_BFS(N, indexOf(r, c));
        else if (settings.floodMode == 2) floodFillZeros_DFS(N, indexOf(r, c));
        else {
            if (N <= 200) floodFillZeros_BFS(N, indexOf(r, c));
            else floodFillZeros_DFS(N, indexOf(r, c));
        }
    }
    return true;
}

bool Minesweeper::toggleFlag(int r, int c) {
    Cell& cell = board[r][c];
    if (cell.revealed) return false;
    cell.flagged = !cell.flagged;
    return true;
}

bool Minesweeper::chord(int r, int c) {
    Cell& cell = board[r][c];
    if (!cell.revealed || cell.adjMines <= 0) {
        cout << "Chord only works on revealed numbered cells.\n";
        return false;
    }

    int flagCount = 0;
    int dr[8] = {-1,-1,-1, 0,0, 1,1,1};
    int dc[8] = {-1, 0, 1,-1,1,-1,0,1};

    for (int k = 0; k < 8; ++k) {
        int nr = r + dr[k], nc = c + dc[k];
        if (!inBounds(nr, nc)) continue;
        if (board[nr][nc].flagged) flagCount++;
    }

    if (flagCount != cell.adjMines) {
        cout << "Flags don't match the number.\n";
        return false;
    }

    bool changed = false;
    for (int k = 0; k < 8; ++k) {
        int nr = r + dr[k], nc = c + dc[k];
        if (!inBounds(nr, nc)) continue;
        if (board[nr][nc].flagged) continue;

        if (reveal(nr, nc)) changed = true;
        if (gameOver) return true;
    }
    return changed;
}

bool Minesweeper::checkWin() const {
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            if (!board[r][c].hasMine && !board[r][c].revealed) return false;
    return true;
}

bool Minesweeper::hintRevealOneSafeCell() {
    if (!firstClickDone) {
        cout << "Hint only works after first reveal.\n";
        return false;
    }
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Cell& cell = board[r][c];
            if (!cell.revealed && !cell.flagged && !cell.hasMine) {
                return reveal(r, c);
            }
        }
    }
    cout << "No safe hint found.\n";
    return false;
}

void Minesweeper::play() {
    while (!gameOver && !win) {
        printBoard();

        cout << "\nCommands:\n";
        cout << "  r row col   -> reveal\n";
        cout << "  f row col   -> flag/unflag\n";
        if (settings.enableChord) cout << "  c row col   -> chord\n";
        if (settings.enableUndoRedo) {
            cout << "  u           -> undo\n";
            cout << "  y           -> redo\n";
        }
        if (settings.enableHints) cout << "  h           -> hint\n";
        cout << "  q           -> quit\n";
        cout << "Enter command: ";

        char cmd;
        if (!(cin >> cmd)) return;

        if (cmd == 'q') {
            cout << "Quitting.\n";
            return;
        }

        if (cmd == 'h' && settings.enableHints) {
            saveStateForUndoIfEnabled();
            bool ok = hintRevealOneSafeCell();
            if (ok) {
                moveCount++;
                if (settings.enableUndoRedo) redoStack.clear();
            }
        } else if (cmd == 'u') {
            if (!settings.enableUndoRedo) cout << "Undo disabled.\n";
            else undo();
        } else if (cmd == 'y') {
            if (!settings.enableUndoRedo) cout << "Redo disabled.\n";
            else redo();
        } else if (cmd == 'r' || cmd == 'f' || cmd == 'c') {
            int r, c;
            cin >> r >> c;
            if (!inBounds(r, c)) {
                cout << "Invalid cell.\n";
                continue;
            }
            if (cmd == 'c' && !settings.enableChord) {
                cout << "Chord disabled.\n";
                continue;
            }

            saveStateForUndoIfEnabled();

            bool changed = false;
            if (cmd == 'r') changed = reveal(r, c);
            else if (cmd == 'f') changed = toggleFlag(r, c);
            else changed = chord(r, c);

            if (changed) {
                moveCount++;
                if (settings.enableUndoRedo) redoStack.clear();
            }
        } else {
            cout << "Unknown command.\n";
        }

        if (!gameOver) {
            win = checkWin();
            if (win) {
                printBoard(true);
                cout << "Congratulations! You win!\n";
            }
        }
    }
}
