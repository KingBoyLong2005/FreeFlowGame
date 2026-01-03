#pragma once
#include "LevelLoader.h"
#include <vector>
#include <string>

class LevelManager {
public:
    static LevelManager* getInstance();
    
    bool loadLevel(int levelIndex, LevelData& outData);
    int getTotalLevels() const { return _levelFiles.size(); }
    int getCurrentLevel() const { return _currentLevel; }
    void setCurrentLevel(int level) { _currentLevel = level; }
    void nextLevel();
    void previousLevel();
    
private:
    LevelManager();
    static LevelManager* _instance;
    
    std::vector<std::string> _levelFiles;
    int _currentLevel = 0;
};