#pragma once
#include "axmol/axmol.h"
#include "GameController.h"
#include "BoardRenderer.h"
#include "LevelLoader.h"
#include "UIManager.h"

class GameScene : public ax::Scene {
public:
    CREATE_FUNC(GameScene);
    
    bool init() override;
    void update(float delta) override;

private:
    GameController* _game = nullptr;
    BoardRenderer* _board = nullptr;
    UIManager* _ui = nullptr;
    LevelData _levelData; // Lưu level data
};