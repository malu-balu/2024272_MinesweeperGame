#ifndef ADMIN_H
#define ADMIN_H

#include <vector>
#include "ds_structures.h"
#include "settings.h"
#include "scores.h"

void printCurrentDifficulties(const DifficultyHashMap& diffMap);
void adminMenu(DifficultyHashMap& diffMap, Settings& settings, const vector<Score>& scores);

#endif
