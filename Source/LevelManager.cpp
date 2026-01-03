#include "LevelManager.h"

LevelManager* LevelManager::_instance = nullptr;

LevelManager::LevelManager()
{
    // Danh sách các file level
    _levelFiles = {
        "Level.json",            // Level mặc định
        "Level1_Easy.json",      // 5x5 - 3 colors
        "Level2_Medium.json",    // 6x6 - 5 colors
        "Level3_Hard.json",      // 8x8 - 5 colors
        "Level4_Expert.json"     // 9x9 - 5 colors
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
    {
        printf("Invalid level index: %d\n", levelIndex);
        return false;
    }
    
    printf("Loading level %d: %s\n", levelIndex + 1, _levelFiles[levelIndex].c_str());
    return LevelLoader::loadFromFile(_levelFiles[levelIndex], outData);
}

void LevelManager::nextLevel()
{
    _currentLevel++;
    if (_currentLevel >= (int)_levelFiles.size())
        _currentLevel = 0; // Loop về level đầu
    
    printf("Next level: %d/%d\n", _currentLevel + 1, (int)_levelFiles.size());
}

void LevelManager::previousLevel()
{
    _currentLevel--;
    if (_currentLevel < 0)
        _currentLevel = _levelFiles.size() - 1;
    
    printf("Previous level: %d/%d\n", _currentLevel + 1, (int)_levelFiles.size());
}