#include "GameScene.h"
#include "LevelLoader.h"
#include "LevelManager.h"

using namespace ax;

bool GameScene::init()
{
    printf("\n=== GameScene::init() START ===\n");
    
    if (!Scene::init())
    {
        printf("ERROR: Scene::init() failed\n");
        return false;
    }
    printf("✓ Scene::init() success\n");

    // Vẽ background
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    printf("VisibleSize: %.1f x %.1f, Origin: %.1f x %.1f\n", 
           visibleSize.width, visibleSize.height, origin.x, origin.y);

    auto bgDraw = DrawNode::create();
    bgDraw->drawSolidRect(
        origin,
        origin + visibleSize,
        Color32(30, 30, 50, 255)
    );
    this->addChild(bgDraw, -1);
    printf("✓ Background drawn\n");

    // ========================================
    // LOAD LEVEL DATA TRƯỚC TIÊN
    // ========================================
    auto levelMgr = LevelManager::getInstance();
    printf("\n--- Loading Level Data ---\n");
    printf("Current level index: %d/%d\n", 
           levelMgr->getCurrentLevel(), 
           levelMgr->getTotalLevels());
    
    if (!levelMgr->loadLevel(levelMgr->getCurrentLevel(), _levelData))
    {
        printf("❌ ERROR: Cannot load level from LevelManager\n");
        printf("Creating default level for testing...\n");
        
        // Tạo level mặc định
        _levelData.rows = 5;
        _levelData.cols = 5;
        _levelData.objects.resize(5);
        for (int i = 0; i < 5; i++)
        {
            _levelData.objects[i].resize(5, "");
        }
        _levelData.objects[0][0] = "A";
        _levelData.objects[0][4] = "A";
        _levelData.objects[2][0] = "B";
        _levelData.objects[2][4] = "B";
        _levelData.objects[4][0] = "C";
        _levelData.objects[4][4] = "C";
        printf("✓ Default level created: 5x5\n");
    }
    
    printf("✓ Level data ready: %dx%d, objects.size=%zu\n", 
           _levelData.rows, _levelData.cols, _levelData.objects.size());
    
    // Debug: In ra một vài objects
    if (!_levelData.objects.empty())
    {
        printf("Sample objects:\n");
        for (int r = 0; r < std::min(3, (int)_levelData.objects.size()); r++)
        {
            for (int c = 0; c < std::min(3, (int)_levelData.objects[r].size()); c++)
            {
                if (!_levelData.objects[r][c].empty())
                    printf("  [%d][%d] = '%s'\n", r, c, _levelData.objects[r][c].c_str());
            }
        }
    }

    // ========================================
    // TẠO BOARDRENDERER
    // ========================================
    printf("\n--- Creating BoardRenderer ---\n");
    _board = BoardRenderer::create();
    if (!_board) 
    {
        printf("❌ ERROR: Cannot create BoardRenderer\n");
        return false;
    }
    printf("✓ BoardRenderer created\n");
    
    // QUAN TRỌNG: Set level data NGAY SAU KHI TẠO
    printf("Setting level data to BoardRenderer...\n");
    _board->setLevelData(_levelData);
    printf("✓ Level data set to BoardRenderer\n");
    
    this->addChild(_board, 1);
    printf("✓ BoardRenderer added to scene\n");
    
    printf("Drawing grid...\n");
    _board->drawGrid();
    
    printf("Drawing dots...\n");
    _board->drawDots();
    printf("✓ Board setup complete, dots count: %zu\n", _board->_dots.size());

    // ========================================
    // TẠO UIMANAGER
    // ========================================
    printf("\n--- Creating UIManager ---\n");
    _ui = UIManager::create();
    if (!_ui) 
    {
        printf("❌ ERROR: Cannot create UIManager\n");
        return false;
    }
    printf("✓ UIManager created\n");
    
    this->addChild(_ui, 2);
    _ui->drawResetButton();
    printf("✓ UIManager setup complete\n");

    // Hiển thị level number
    auto levelLabel = Label::createWithTTF(
        "Level " + std::to_string(levelMgr->getCurrentLevel() + 1),
        "fonts/Marker Felt.ttf", 
        28
    );
    if (levelLabel)
    {
        levelLabel->setPosition(Vec2(
            origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - 30
        ));
        levelLabel->setColor(Color32::WHITE);
        this->addChild(levelLabel, 3);
        printf("✓ Level label created\n");
    }

    // ========================================
    // TẠO GAMECONTROLLER
    // ========================================
    printf("\n--- Creating GameController ---\n");
    _game = GameController::create();
    if (!_game) 
    {
        printf("❌ ERROR: Cannot create GameController\n");
        return false;
    }
    printf("✓ GameController created\n");
    
    _game->setBoardRenderer(_board);
    _game->setUIManager(_ui);
    _game->setDots(_board->_dots);
    printf("✓ GameController configured with %zu dots\n", _board->_dots.size());
    
    this->addChild(_game, 0);
    _game->setupMouseInput();
    printf("✓ GameController setup complete\n");

    this->scheduleUpdate();
    
    printf("\n=== GameScene::init() COMPLETE ===\n\n");
    return true;
}

void GameScene::update(float delta)
{
    Scene::update(delta);
}