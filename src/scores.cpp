#include "scores.h"

// ===== Circular Doubly List =====
ScoreCircularDoublyList::ScoreCircularDoublyList() : head(nullptr) {}

ScoreCircularDoublyList::~ScoreCircularDoublyList() { clear(); }

void ScoreCircularDoublyList::clear() {
    if (!head) return;
    Node* cur = head->next;
    while (cur != head) {
        Node* t = cur;
        cur = cur->next;
        delete t;
    }
    delete head;
    head = nullptr;
}

void ScoreCircularDoublyList::append(const Score& s) {
    Node* n = new Node(s);
    if (!head) {
        head = n;
        head->next = head;
        head->prev = head;
        return;
    }
    Node* tail = head->prev;
    tail->next = n;
    n->prev = tail;
    n->next = head;
    head->prev = n;
}

void ScoreCircularDoublyList::printAll() const {
    if (!head) return;
    Node* cur = head;
    do {
        cout << cur->data.player << " (" << cur->data.difficultyName << ") moves="
             << cur->data.moves << " result=" << (cur->data.win ? "Win" : "Loss") << "\n";
        cur = cur->next;
    } while (cur != head);
}

// ===== BST (recursion) =====
void insertScoreTree(ScoreTreeNode*& root, const Score& s) {
    if (!root) {
        root = new ScoreTreeNode(s);
        return;
    }
    if (s.moves < root->data.moves) insertScoreTree(root->left, s);
    else                            insertScoreTree(root->right, s);
}

void inorderPrintScores(ScoreTreeNode* root) {
    if (!root) return;
    inorderPrintScores(root->left);
    cout << root->data.player << " (" << root->data.difficultyName << ") moves="
         << root->data.moves << " result=" << (root->data.win ? "Win" : "Loss") << "\n";
    inorderPrintScores(root->right);
}

void deleteScoreTree(ScoreTreeNode*& root) {
    if (!root) return;
    deleteScoreTree(root->left);
    deleteScoreTree(root->right);
    delete root;
    root = nullptr;
}

// ===== Sorting =====
void bubbleSortScores(vector<Score>& scores) {
    int n = (int)scores.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - 1 - i; ++j) {
            if (scores[j].moves > scores[j + 1].moves) {
                mySwap(scores[j], scores[j + 1]);
            }
        }
    }
}

void insertionSortScores(vector<Score>& scores) {
    int n = (int)scores.size();
    for (int i = 1; i < n; ++i) {
        Score key = scores[i];
        int j = i - 1;
        while (j >= 0 && scores[j].moves > key.moves) {
            scores[j + 1] = scores[j];
            j--;
        }
        scores[j + 1] = key;
    }
}

static void mergeScores(vector<Score>& a, int l, int mid, int r, vector<Score>& tmp) {
    int i = l, j = mid + 1, k = l;
    while (i <= mid && j <= r) {
        if (a[i].moves <= a[j].moves) tmp[k++] = a[i++];
        else                          tmp[k++] = a[j++];
    }
    while (i <= mid) tmp[k++] = a[i++];
    while (j <= r)   tmp[k++] = a[j++];
    for (int x = l; x <= r; ++x) a[x] = tmp[x];
}

static void mergeSortRec(vector<Score>& a, int l, int r, vector<Score>& tmp) {
    if (l >= r) return;
    int mid = (l + r) / 2;
    mergeSortRec(a, l, mid, tmp);
    mergeSortRec(a, mid + 1, r, tmp);
    mergeScores(a, l, mid, r, tmp);
}

void mergeSortScores(vector<Score>& scores) {
    if (scores.empty()) return;
    vector<Score> tmp(scores.size());
    mergeSortRec(scores, 0, (int)scores.size() - 1, tmp);
}

// ===== File handling =====
void loadScoresFromFile(const string& filename,
                        vector<Score>& scores,
                        ScoreCircularDoublyList& list,
                        ScoreTreeNode*& treeRoot) {
    ifstream fin(filename.c_str());
    if (!fin) return;

    Score s;
    while (fin >> s.player >> s.difficultyName >> s.moves >> s.win) {
        scores.push_back(s);
        list.append(s);
        insertScoreTree(treeRoot, s);
    }
}

void saveScoresToFile(const string& filename, const vector<Score>& scores) {
    ofstream fout(filename.c_str());
    for (size_t i = 0; i < scores.size(); ++i) {
        fout << scores[i].player << ' ' << scores[i].difficultyName << ' '
             << scores[i].moves << ' ' << scores[i].win << "\n";
    }
}

// ===== Searching =====
bool findBestScoreForPlayer(const vector<Score>& scores, const string& player, Score& bestOut) {
    bool found = false;
    for (size_t i = 0; i < scores.size(); ++i) {
        if (scores[i].player == player && scores[i].win) {
            if (!found || scores[i].moves < bestOut.moves) {
                bestOut = scores[i];
                found = true;
            }
        }
    }
    return found;
}
