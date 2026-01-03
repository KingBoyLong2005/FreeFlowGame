#include "BoardRenderer.h"
#include "ColorPalette.h"

using namespace ax;

bool BoardRenderer::init()
{
    if (!Node::init())
    {
        printf("ERROR: BoardRenderer Node::init() failed\n");
        return false;
    }

    _drawNode = DrawNode::create();
    if (!_drawNode)
    {
        printf("ERROR: Cannot create DrawNode\n");
        return false;
    }
    
    this->addChild(_drawNode, 10);
    printf("✓ DrawNode created and added\n");
    
    return true;
}

void BoardRenderer::setLevelData(const LevelData& data)
{
    _levelData = data;
    _levelRows = data.rows;
    _levelCols = data.cols;
    
    printf("BoardRenderer::setLevelData - rows: %d, cols: %d, objects size: %zu\n",
           _levelRows, _levelCols, _levelData.objects.size());
}

void BoardRenderer::drawGrid()
{
    printf("BoardRenderer::drawGrid() called\n");
    printf("  _drawNode: %p\n", _drawNode);
    printf("  _levelRows: %d\n", _levelRows);
    printf("  _levelCols: %d\n", _levelCols);
    
    if (!_drawNode)
    {
        printf("ERROR: _drawNode is null!\n");
        return;
    }
    
    if (_levelRows == 0 || _levelCols == 0)
    {
        printf("ERROR: Invalid grid size: %dx%d\n", _levelRows, _levelCols);
        return;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    float gridWidth  = _levelCols * _cellSize;
    float gridHeight = _levelRows * _cellSize;

    _gridOffset.x = origin.x + (visibleSize.width  - gridWidth)  * 0.5f;
    _gridOffset.y = origin.y + (visibleSize.height - gridHeight) * 0.5f;

    printf("Grid offset: (%.1f, %.1f)\n", _gridOffset.x, _gridOffset.y);

    float half = _cellSize * 0.5f;

    for (int r = 0; r < _levelRows; r++)
    {
        for (int c = 0; c < _levelCols; c++)
        {
            Vec2 center = gridToWorld(r, c);
            Vec2 bl(center.x - half, center.y - half);
            Vec2 tr(center.x + half, center.y + half);

            _drawNode->drawSolidRect(bl, tr, Color32(40, 40, 40, 255));
            _drawNode->drawRect(bl, tr, Color32(80, 80, 80, 255));
        }
    }
    
    printf("✓ Grid drawn successfully: %dx%d at offset (%.1f, %.1f)\n", 
           _levelRows, _levelCols, _gridOffset.x, _gridOffset.y);
}

void BoardRenderer::drawDots()
{
    printf("BoardRenderer::drawDots() called\n");
    printf("  _drawNode: %p\n", _drawNode);
    printf("  _levelData.objects.size(): %zu\n", _levelData.objects.size());
    printf("  _levelRows: %d, _levelCols: %d\n", _levelRows, _levelCols);
    
    if (!_drawNode)
    {
        printf("ERROR: _drawNode is null!\n");
        return;
    }
    
    if (_levelData.objects.empty())
    {
        printf("ERROR: _levelData.objects is empty!\n");
        return;
    }

    _dots.clear();

    for (int r = 0; r < _levelRows; r++)
    {
        if (r >= (int)_levelData.objects.size())
        {
            printf("WARNING: Row %d exceeds objects size %zu\n", r, _levelData.objects.size());
            continue;
        }
        
        for (int c = 0; c < _levelCols; c++)
        {
            if (c >= (int)_levelData.objects[r].size())
            {
                printf("WARNING: Col %d exceeds row %d size %zu\n", c, r, _levelData.objects[r].size());
                continue;
            }
            
            if (!_levelData.objects[r][c].empty())
            {
                std::string colorId = _levelData.objects[r][c];
                
                // Dùng bảng màu mới
                Color32 color = ColorPalette::getColor(colorId);
                
                _dots.push_back({ r, c, color });

                Vec2 center = gridToWorld(r, c);
                float radius = 15.0f;
                
                // Vẽ viền trắng (vòng tròn lớn hơn)
                _drawNode->drawSolidCircle(
                    center,
                    radius + 2.0f,  // Viền dày 2px
                    0.0f,
                    32,
                    Color32::WHITE
                );
                
                // Vẽ dot chính (đè lên viền)
                _drawNode->drawSolidCircle(
                    center,
                    radius,
                    0.0f,
                    32,
                    color
                );
                
                printf("  Dot drawn: %s at (%d,%d) - color: (%d,%d,%d)\n", 
                       colorId.c_str(), r, c, color.r, color.g, color.b);
            }
        }
    }
    
    printf("✓ Total dots drawn: %zu\n", _dots.size());
}

Vec2 BoardRenderer::gridToWorld(int row, int col) const
{
    float x = _gridOffset.x + col * _cellSize + _cellSize * 0.5f;
    float y = _gridOffset.y + (_levelRows - row - 1) * _cellSize + _cellSize * 0.5f;
    return Vec2(x, y);
}

bool BoardRenderer::nodeToGrid(const Vec2& local, int& outRow, int& outCol) const
{
    if (local.x < 0 || local.y < 0)
        return false;

    int col = static_cast<int>(local.x / _cellSize);
    int row = _levelRows - 1 - static_cast<int>(local.y / _cellSize);

    if (row < 0 || row >= _levelRows || col < 0 || col >= _levelCols)
        return false;

    outRow = row;
    outCol = col;
    return true;
}