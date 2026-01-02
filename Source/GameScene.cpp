#include "GameScene.h"
#include "LevelLoader.h"

using namespace ax;

bool GameScene::init()
{
    if (!Scene::init())
        return false;

    // Load level data
    if (!LevelLoader::loadFromFile("Level.json", _levelData))
    {
        printf("ERROR: Cannot load Level.json\n");
        return false;
    }

    printf("Level loaded: %dx%d\n", _levelData.rows, _levelData.cols);

    // Tạo BoardRenderer và truyền level data
    _board = BoardRenderer::create();
    if (!_board) return false;
    _board->setLevelData(_levelData);
    this->addChild(_board);
    _board->drawGrid();
    _board->drawDots();

    // Tạo UIManager
    _ui = UIManager::create();
    if (!_ui) return false;
    this->addChild(_ui);
    _ui->drawResetButton();

    // Tạo GameController và truyền board renderer + dots
    _game = GameController::create();
    if (!_game) return false;
    _game->setBoardRenderer(_board);
    _game->setDots(_board->_dots);
    this->addChild(_game);
    _game->setupMouseInput();

    // Enable update
    this->scheduleUpdate();

    return true;
}

void GameScene::update(float delta)
{
    Scene::update(delta);
}