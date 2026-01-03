#include "GameController.h"
#include "LevelManager.h"
#include "GameScene.h"

using namespace ax;

bool GameController::init()
{
    if (!Node::init())
        return false;
    
    return true;
}

void GameController::setBoardRenderer(BoardRenderer* board)
{
    _board = board;
}

void GameController::setUIManager(UIManager* ui)
{
    _ui = ui;
}

void GameController::setDots(const std::vector<BoardRenderer::Dot>& dots)
{
    _dots = dots;
    printf("GameController received %zu dots\n", _dots.size());
}

void GameController::resetLevel()
{
    _fixedPaths.clear();
    _currentPath.clear();
    _isDragging = false;
    
    if (_ui)
    {
        _ui->hideCompletedPopup();
        _ui->_levelCompleted = false;
    }

    redrawPath();
    printf("Level reset\n");
}

void GameController::nextLevel()
{
    auto levelMgr = LevelManager::getInstance();
    levelMgr->nextLevel();
    
    printf("Loading next level: %d\n", levelMgr->getCurrentLevel() + 1);
    
    // Reload scene với level mới
    auto newScene = GameScene::create();
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.5f, newScene)
    );
}

void GameController::restartCurrentLevel()
{
    printf("Restarting current level\n");
    
    // Reload scene với level hiện tại
    auto newScene = GameScene::create();
    Director::getInstance()->replaceScene(
        TransitionFade::create(0.3f, newScene)
    );
}

void GameController::checkLevelComplete()
{
    // Đếm số màu cần hoàn thành
    std::set<ax::Color32, ColorLess> uniqueColors;
    for (auto& d : _dots)
        uniqueColors.insert(d.color);

    printf("Checking completion: %zu paths vs %zu colors needed\n", 
           _fixedPaths.size(), uniqueColors.size());

    // Kiểm tra số path đã hoàn thành
    if (_fixedPaths.size() != uniqueColors.size())
        return;

    // Kiểm tra tất cả ô đã được lấp đầy
    std::set<std::pair<int, int>> covered;
    for (auto& p : _fixedPaths)
        for (auto& c : p.cells)
            covered.insert({ (int)c.x, (int)c.y });

    int totalCells = _board->_levelRows * _board->_levelCols;
    printf("Covered cells: %zu vs total: %d\n", covered.size(), totalCells);

    if ((int)covered.size() != totalCells)
        return;

    // Hoàn thành level!
    if (_ui)
        _ui->showCompletedPopup();
    
    printf("🎉 LEVEL COMPLETED! 🎉\n");
}

void GameController::redrawPath()
{
    if (!_board || !_board->_drawNode)
        return;

    _board->_drawNode->clear();
    _board->drawGrid();

    // Vẽ dots
    for (auto& d : _dots)
    {
        Vec2 center = _board->gridToWorld(d.row, d.col);
        float radius = 15.0f;
        
        // Vẽ viền trắng
        _board->_drawNode->drawSolidCircle(
            center,
            radius + 2.0f,
            0.0f,
            32,
            Color32::WHITE
        );
        
        // Vẽ dot chính
        _board->_drawNode->drawSolidCircle(
            center,
            radius,
            0.0f,
            32,
            d.color
        );
    }

    // Vẽ fixed paths
    for (const auto& p : _fixedPaths)
    {
        for (size_t i = 1; i < p.cells.size(); i++)
        {
            Vec2 a = _board->gridToWorld(p.cells[i - 1].x, p.cells[i - 1].y);
            Vec2 b = _board->gridToWorld(p.cells[i].x, p.cells[i].y);
            _board->_drawNode->drawSegment(a, b, 8.0f, p.color);
        }
    }

    // Vẽ current path
    for (size_t i = 1; i < _currentPath.size(); i++)
    {
        Vec2 a = _board->gridToWorld(_currentPath[i - 1].x, _currentPath[i - 1].y);
        Vec2 b = _board->gridToWorld(_currentPath[i].x, _currentPath[i].y);
        _board->_drawNode->drawSegment(a, b, 8.0f, _currentColor);
    }
}

void GameController::setupMouseInput()
{
    auto listener = EventListenerMouse::create();

    listener->onMouseDown = [this](EventMouse* event) -> bool
    {
        Vec2 world = event->getLocation();
        
        // Kiểm tra click vào Next Level button
        if (_ui && _ui->_levelCompleted && _ui->_nextLevelBtnRect.containsPoint(world))
        {
            printf("Next Level button clicked\n");
            nextLevel();
            return true;
        }
        
        // Kiểm tra click vào Restart button
        if (_ui && _ui->_levelCompleted && _ui->_restartBtnRect.containsPoint(world))
        {
            printf("Restart button clicked\n");
            restartCurrentLevel();
            return true;
        }
        
        // Kiểm tra click vào reset button
        if (_ui && _ui->_resetBtnRect.containsPoint(world))
        {
            printf("Reset button clicked\n");
            resetLevel();
            return true;
        }

        Vec2 local = world - _board->_gridOffset;

        int r, c;
        if (!_board->nodeToGrid(local, r, c))
            return false;

        printf("Mouse down at grid (%d, %d)\n", r, c);

        _isDragging = false;
        _currentPath.clear();

        for (auto& d : _dots)
        {
            if (d.row == r && d.col == c)
            {
                // Xóa path cũ của màu này
                for (auto it = _fixedPaths.begin(); it != _fixedPaths.end(); ++it)
                {
                    if (it->color == d.color)
                    {
                        _fixedPaths.erase(it);
                        break;
                    }
                }

                printf("Started dragging from dot at (%d,%d)\n", r, c);

                _isDragging = true;
                _startCell = Vec2(r, c);
                _currentCell = _startCell;
                _currentColor = d.color;
                _currentPath.push_back(_startCell);

                redrawPath();
                break;
            }
        }

        return true;
    };

    listener->onMouseMove = [this](EventMouse* event) -> bool
    {
        if (!_isDragging)
            return false;

        Vec2 local = event->getLocation() - _board->_gridOffset;

        int r, c;
        if (!_board->nodeToGrid(local, r, c))
            return false;

        Vec2 cell(r, c);
        Vec2 last = _currentPath.back();

        if (cell == last)
            return true;

        // Đi lùi
        if (_currentPath.size() >= 2)
        {
            Vec2 prev = _currentPath[_currentPath.size() - 2];
            if (cell == prev)
            {
                _currentPath.pop_back();
                redrawPath();
                return true;
            }
        }

        // Chỉ đi vuông góc
        int dx = abs(static_cast<int>(cell.x - last.x));
        int dy = abs(static_cast<int>(cell.y - last.y));
        if (dx + dy != 1)
            return false;

        // Kiểm tra dot khác màu
        Color32 hitColor;
        if (getDotColorAt(r, c, hitColor))
        {
            if (hitColor != _currentColor)
                return false;
        }

        // Đi tiếp
        _currentPath.push_back(cell);
        redrawPath();

        // Kiểm tra hoàn thành
        if (isSameColorDotEnd(r, c) && Vec2(r, c) != _startCell)
        {
            printf("✅ PATH COMPLETE\n");

            _fixedPaths.push_back({
                _currentColor,
                _currentPath
            });

            _currentPath.clear();
            _isDragging = false;

            redrawPath();
            checkLevelComplete();
            return true;
        }

        return true;
    };

    listener->onMouseUp = [this](EventMouse*) -> bool
    {
        _isDragging = false;
        return true;
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    printf("Mouse input setup complete\n");
}

bool GameController::getDotColorAt(int row, int col, Color32& outColor) const
{
    for (const auto& d : _dots)
    {
        if (d.row == row && d.col == col)
        {
            outColor = d.color;
            return true;
        }
    }
    return false;
}

bool GameController::isSameColorDotEnd(int r, int c) const
{
    for (const auto& d : _dots)
    {
        if (d.row == r && d.col == c && d.color == _currentColor)
            return true;
    }
    return false;
}