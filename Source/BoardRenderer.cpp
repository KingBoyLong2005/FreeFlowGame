#include "BoardRenderer.h"
#include "ColorPalette.h"

using namespace ax;

bool BoardRenderer::init()
{
    if (!Node::init())
        return false;

    _drawNode = DrawNode::create();
    this->addChild(_drawNode, 10);
    
    return true;
}

void BoardRenderer::setLevelData(const LevelData& data)
{
    _levelData = data;
    _levelRows = data.rows;
    _levelCols = data.cols;
}

void BoardRenderer::drawGrid()
{
    if (!_drawNode || _levelRows == 0 || _levelCols == 0)
    {
        printf("ERROR: Cannot draw grid - invalid state\n");
        return;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    float gridWidth  = _levelCols * _cellSize;
    float gridHeight = _levelRows * _cellSize;

    _gridOffset.x = origin.x + (visibleSize.width  - gridWidth)  * 0.5f;
    _gridOffset.y = origin.y + (visibleSize.height - gridHeight) * 0.5f;

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
    
    printf("Grid drawn: %dx%d at offset (%.1f, %.1f)\n", 
           _levelRows, _levelCols, _gridOffset.x, _gridOffset.y);
}

void BoardRenderer::drawDots()
{
    if (!_drawNode || _levelData.objects.empty())
    {
        printf("ERROR: Cannot draw dots - invalid state\n");
        return;
    }

    _dots.clear();

    for (int r = 0; r < _levelRows; r++)
    {
        for (int c = 0; c < _levelCols; c++)
        {
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
                
                printf("Dot drawn: %s at (%d,%d)\n", colorId.c_str(), r, c);
            }
        }
    }
    
    printf("Total dots drawn: %zu\n", _dots.size());
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