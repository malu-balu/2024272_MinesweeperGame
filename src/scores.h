#ifndef SCORES_H
#define SCORES_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "utils.h"

using namespace std;

struct Score {
    string player;
    string difficultyName;
    int moves;
    bool win;
};

// Doubly + Circular linked list
class ScoreCircularDoublyList {
private:
    struct Node {
        Score data;
        Node* next;
        Node* prev;
        Node(const Score& s) : data(s), next(nullptr), prev(nullptr) {}
    };
    Node* head;

public:
    ScoreCircularDoublyList();
    ~ScoreCircularDoublyList();
    void append(const Score& s);
    void printAll() const;
    void clear();
};

// BST node
class ScoreTreeNode {
public:
    Score data;
    ScoreTreeNode* left;
    ScoreTreeNode* right;
    ScoreTreeNode(const Score& s) : data(s), left(nullptr), right(nullptr) {}
};

void insertScoreTree(ScoreTreeNode*& root, const Score& s);
void inorderPrintScores(ScoreTreeNode* root);
void deleteScoreTree(ScoreTreeNode*& root);

// Sorting
void bubbleSortScores(vector<Score>& scores);
void insertionSortScores(vector<Score>& scores);
void mergeSortScores(vector<Score>& scores);

// File IO
void loadScoresFromFile(const string& filename,
                        vector<Score>& scores,
                        ScoreCircularDoublyList& list,
                        ScoreTreeNode*& treeRoot);

void saveScoresToFile(const string& filename, const vector<Score>& scores);

// Searching
bool findBestScoreForPlayer(const vector<Score>& scores, const string& player, Score& bestOut);

#endif
