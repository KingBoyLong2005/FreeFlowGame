#include "LevelManager.h"

LevelManager* LevelManager::_instance = nullptr;

LevelManager::LevelManager()
{
    // Danh sách các file level
    _levelFiles = {
        // "Level1_Easy.json",      // 5x5 - 3 colors
        // "Level2_Easy.json",      // 5x5 - 4 colors
        // "Level3_Medium.json",    // 6x6 - 4 colors
        // "Level4_Medium.json",    // 6x6 - 5 colors
        // "Level5_Hard.json",      // 7x7 - 6 colors
        // "Level6_Hard.json",      // 8x8 - 7 colors
        // "Level7_Expert.json",    // 9x9 - 8 colors
        // "LevelChallenge.json"    // 7x7 - challenge
        "Level.json"
    };
}

LevelManager* LevelManager::getInstance()
{
    if (!_instance)
        _instance = new LevelManager();
    return _instance;
}

bool LevelManager::loadLevel(int levelIndex, LevelData& outData)
{
    if (levelIndex < 0 || levelIndex >= (int)_levelFiles.size())
        return false;
    
    return LevelLoader::loadFromFile(_levelFiles[levelIndex], outData);
}

void LevelManager::nextLevel()
{
    _currentLevel++;
    if (_currentLevel >= (int)_levelFiles.size())
        _currentLevel = 0; // Loop back
}

void LevelManager::previousLevel()
{
    _currentLevel--;
    if (_currentLevel < 0)
        _currentLevel = _levelFiles.size() - 1;
}