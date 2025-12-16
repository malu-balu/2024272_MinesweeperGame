#ifndef SETTINGS_H
#define SETTINGS_H

struct Settings {
    bool firstClickSafe = true;
    bool enableChord = true;
    bool enableUndoRedo = true;
    bool enableHints = false;

    // 0=AUTO, 1=BFS, 2=DFS
    int floodMode = 0;
};

#endif
