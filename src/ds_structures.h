#ifndef DS_STRUCTURES_H
#define DS_STRUCTURES_H

#include <iostream>
#include "types.h"
using namespace std;

// ========================= Custom Hash Map (int -> Difficulty) =========================
class DifficultyHashMap {
private:
    struct Entry {
        int key;
        Difficulty value;
        bool used;
        bool deleted;
        Entry() : key(0), used(false), deleted(false) {}
    };

    Entry* table;
    int cap;
    int sz;

    int hashKey(int k) const { return (k < 0 ? -k : k) % cap; }

public:
    DifficultyHashMap(int capacity = 17) : cap(capacity), sz(0) {
        table = new Entry[cap];
    }

    ~DifficultyHashMap() { delete[] table; }

    void put(int key, const Difficulty& val) {
        int idx = hashKey(key);
        int firstDeleted = -1;

        for (int i = 0; i < cap; ++i) {
            int p = (idx + i) % cap;

            if (!table[p].used) {
                if (firstDeleted != -1) p = firstDeleted;
                table[p].key = key;
                table[p].value = val;
                table[p].used = true;
                table[p].deleted = false;
                sz++;
                return;
            }

            if (table[p].used && table[p].deleted && firstDeleted == -1) {
                firstDeleted = p;
            }

            if (table[p].used && !table[p].deleted && table[p].key == key) {
                table[p].value = val;
                return;
            }
        }
        cout << "HashMap full. Increase capacity.\n";
    }

    bool get(int key, Difficulty& out) const {
        int idx = hashKey(key);
        for (int i = 0; i < cap; ++i) {
            int p = (idx + i) % cap;
            if (!table[p].used) return false;
            if (table[p].used && !table[p].deleted && table[p].key == key) {
                out = table[p].value;
                return true;
            }
        }
        return false;
    }
};

// ========================= Custom Stack (Array-based) =========================
template <typename T>
class ArrayStack {
private:
    T* arr;
    int cap;
    int topIdx;

    void resize(int newCap) {
        T* newArr = new T[newCap];
        for (int i = 0; i <= topIdx; ++i) newArr[i] = arr[i];
        delete[] arr;
        arr = newArr;
        cap = newCap;
    }

public:
    ArrayStack(int capacity = 50) : cap(capacity), topIdx(-1) {
        arr = new T[cap];
    }

    ~ArrayStack() { delete[] arr; }

    bool empty() const { return topIdx < 0; }

    void push(const T& val) {
        if (topIdx + 1 >= cap) resize(cap * 2);
        arr[++topIdx] = val;
    }

    T top() const { return arr[topIdx]; }

    void pop() {
        if (!empty()) topIdx--;
    }

    void clear() { topIdx = -1; }
};

// ========================= Custom Stack (Linked-list based) =========================
template <typename T>
class LinkedStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };
    Node* head;

public:
    LinkedStack() : head(nullptr) {}
    ~LinkedStack() {
        while (head) {
            Node* t = head;
            head = head->next;
            delete t;
        }
    }

    bool empty() const { return head == nullptr; }

    void push(const T& val) {
        Node* n = new Node(val);
        n->next = head;
        head = n;
    }

    T top() const { return head->data; }

    void pop() {
        if (!head) return;
        Node* t = head;
        head = head->next;
        delete t;
    }

    void clear() { while (!empty()) pop(); }
};

// ========================= Custom Queue (Array-based circular) =========================
template <typename T>
class ArrayQueue {
private:
    T* arr;
    int cap;
    int frontIdx;
    int rearIdx;
    int cnt;

public:
    ArrayQueue(int capacity = 100) : cap(capacity), frontIdx(0), rearIdx(0), cnt(0) {
        arr = new T[cap];
    }

    ~ArrayQueue() { delete[] arr; }

    bool empty() const { return cnt == 0; }

    void push(const T& val) {
        if (cnt >= cap) {
            cout << "Queue overflow!\n";
            return;
        }
        arr[rearIdx] = val;
        rearIdx = (rearIdx + 1) % cap;
        cnt++;
    }

    T front() const { return arr[frontIdx]; }

    void pop() {
        if (empty()) return;
        frontIdx = (frontIdx + 1) % cap;
        cnt--;
    }
};

// ========================= Custom Queue (Linked-list based) =========================
template <typename T>
class LinkedQueue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };
    Node* frontNode;
    Node* rearNode;

public:
    LinkedQueue() : frontNode(nullptr), rearNode(nullptr) {}

    ~LinkedQueue() {
        while (frontNode) {
            Node* t = frontNode;
            frontNode = frontNode->next;
            delete t;
        }
        rearNode = nullptr;
    }

    bool empty() const { return frontNode == nullptr; }

    void push(const T& val) {
        Node* n = new Node(val);
        if (!rearNode) {
            frontNode = rearNode = n;
        } else {
            rearNode->next = n;
            rearNode = n;
        }
    }

    T front() const { return frontNode->data; }

    void pop() {
        if (!frontNode) return;
        Node* t = frontNode;
        frontNode = frontNode->next;
        if (!frontNode) rearNode = nullptr;
        delete t;
    }
};

#endif
