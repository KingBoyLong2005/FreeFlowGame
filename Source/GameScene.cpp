#include "GameScene.h"
#include "LevelLoader.h"
#include "LevelManager.h"

using namespace ax;

bool GameScene::init()
{
    if (!Scene::init())
        return false;

    auto levelMgr = LevelManager::getInstance();
    
    // Load level hiện tại
    if (!levelMgr->loadLevel(levelMgr->getCurrentLevel(), _levelData))
    {
        printf("Cannot load level\n");
        return false;
    }
    
    printf("Loading Level %d/%d\n", 
           levelMgr->getCurrentLevel() + 1, 
           levelMgr->getTotalLevels());
           
    // Load level data
    // if (!LevelLoader::loadFromFile("Level.json", _levelData))
    // {
    //     printf("ERROR: Cannot load Level.json\n");
    //     return false;
    // }

    printf("Level loaded: %dx%d\n", _levelData.rows, _levelData.cols);

    // Tạo BoardRenderer và truyền level data
    _board = BoardRenderer::create();
    if (!_board) 
    {
        printf("ERROR: Cannot create BoardRenderer\n");
        return false;
    }
    _board->setLevelData(_levelData);
    this->addChild(_board);
    _board->drawGrid();
    _board->drawDots();

    // Tạo UIManager
    _ui = UIManager::create();
    if (!_ui) 
    {
        printf("ERROR: Cannot create UIManager\n");
        return false;
    }
    this->addChild(_ui);
    _ui->drawResetButton();

    // Tạo GameController và truyền board renderer + dots + ui
    _game = GameController::create();
    if (!_game) 
    {
        printf("ERROR: Cannot create GameController\n");
        return false;
    }
    _game->setBoardRenderer(_board);
    _game->setUIManager(_ui);  // ← QUAN TRỌNG: Thiếu dòng này!
    _game->setDots(_board->_dots);
    this->addChild(_game);
    _game->setupMouseInput();

    // Enable update
    this->scheduleUpdate();

    printf("GameScene initialization complete!\n");
    return true;
}

void GameScene::update(float delta)
{
    Scene::update(delta);
}