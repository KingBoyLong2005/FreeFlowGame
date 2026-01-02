#include "Scene1.h"
#include "axmol/platform/FileUtils.h"
#include "rapidjson/document.h"

using namespace ax;

Scene1::Scene1() {}
Scene1::~Scene1() {}

bool Scene1::init()
{
    if (!Scene::init())
        return false;

    _drawNode = ax::DrawNode::create();
    _uiNode = ax::DrawNode::create();
    this->addChild(_drawNode, 10);
    this->addChild(_uiNode, 20);

    LevelData level;
    if (!loadFromFile("Level.json", level))
        return false;

    auto visibleSize = _director->getVisibleSize();
    auto origin = _director->getVisibleOrigin();

    float gridWidth  = level.cols * _cellSize;
    float gridHeight = level.rows * _cellSize;

    _gridOffset.x = origin.x + (visibleSize.width  - gridWidth)  * 0.5f;
    _gridOffset.y = origin.y + (visibleSize.height - gridHeight) * 0.5f;

    _levelRows = level.rows;
    _levelCols = level.cols;

    // ===== DRAW GRID =====
    drawGrid();
    drawResetButton();

    // ===== DRAW DOTS =====
    for (int r = 0; r < level.rows; r++)
    {
        for (int c = 0; c < level.cols; c++)
        {
            if (!level.objects[r][c].empty())
            {
                ax::Color32 color = ax::Color32::RED;
                if (level.objects[r][c] == "A") color = ax::Color32::GREEN;
                if (level.objects[r][c] == "B") color = ax::Color32::BLUE;
                if (level.objects[r][c] == "C") color = ax::Color32::RED;
                if (level.objects[r][c] == "D") color = ax::Color32::MAGENTA;
                if (level.objects[r][c] == "E") color = ax::Color32::GRAY;

                _dots.push_back({ r, c, color });

                _drawNode->drawSolidCircle(
                    gridToWorld(r, c),
                    12.0f,
                    0,
                    24,
                    color
                );
            }
        }
    }

    // ⭐ INPUT PHẢI Ở CUỐI
    setupMouseInput();

    return true;
}

void Scene1::update(float delta)
{
    // sau này xử lý input + vẽ line realtime ở đây
}

bool Scene1::loadFromFile(const std::string& filename, LevelData& outData)
{
    std::string jsonStr = FileUtils::getInstance()->getStringFromFile(filename);
    if (jsonStr.empty())
        return false;

    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());

    if (!doc.IsObject())
        return false;

    outData.rows = doc["rows"].GetInt();
    outData.cols = doc["cols"].GetInt();

    // ===== TILES =====
    const rapidjson::Value& tiles = doc["tiles"];
    outData.tiles.resize(outData.rows);

    // ===== OBJECTS =====
    const rapidjson::Value& objects = doc["objects"];
    outData.objects.resize(outData.rows);

    for (int r = 0; r < outData.rows; r++)
    {
        outData.objects[r].resize(outData.cols);
        for (int c = 0; c < outData.cols; c++)
        {
            outData.objects[r][c] = objects[r][c].GetString();
        }
    }

    return true;
}

Vec2 Scene1::gridToWorld(int row, int col) const
{
    float x = _gridOffset.x + col * _cellSize + _cellSize * 0.5f;
    float y = _gridOffset.y
            + ( _levelRows - row - 1 ) * _cellSize
            + _cellSize * 0.5f;

    return Vec2(x, y);
}
void Scene1::drawGrid()
{
    // Color32 fillColor(0.1f, 0.1f, 0.1f, 0.3f);   // nền ô
    // Color32 borderColor(0.5f, 0.5f, 0.5f, 1.0f); // viền ô

    float half = _cellSize * 0.5f;

    for (int r = 0; r < _levelRows; r++)
    {
        for (int c = 0; c < _levelCols; c++)
        {
            Vec2 center = gridToWorld(r, c);

            Vec2 bl(center.x - half, center.y - half); // bottom-left
            Vec2 tr(center.x + half, center.y + half); // top-right

            // nền ô
            _drawNode->drawSolidRect(bl, tr, Color32::YELLOW);

            // viền ô
            _drawNode->drawRect(bl, tr, Color32::BLUE);
        }
    }
}
bool Scene1::nodeToGrid(const ax::Vec2& local, int& outRow, int& outCol) const
{
    printf("[nodeToGrid] local = (%.2f, %.2f)\n", local.x, local.y);

    if (local.x < 0 || local.y < 0)
    {
        printf("  -> local < 0, reject\n");
        return false;
    }

    int col = static_cast<int>(local.x / _cellSize);

    // ⭐ SỬA DÒNG NÀY
    int row = _levelRows - 1 - static_cast<int>(local.y / _cellSize);

    if (row < 0 || row >= _levelRows ||
        col < 0 || col >= _levelCols)
        return false;

    outRow = row;
    outCol = col;
    return true;
}

void Scene1::setupMouseInput()
{
    auto listener = ax::EventListenerMouse::create();

    // ===== MOUSE DOWN =====
    listener->onMouseDown = [this](ax::EventMouse* event) -> bool
    {
        Vec2 world = event->getLocation();
        Vec2 local = world - _gridOffset;

        printf("\n[MOUSE DOWN]\n");
        printf(" world = (%.2f, %.2f)\n", world.x, world.y);
        printf(" local = (%.2f, %.2f)\n", local.x, local.y);

        int r, c;
        if (!nodeToGrid(local, r, c))
        {
            printf(" -> nodeToGrid FAILED\n");
            return false;
        }

        printf(" -> grid cell = (%d, %d)\n", r, c);

        _isDragging = false;
        _currentPath.clear();

        for (auto& d : _dots)
        {
            printf("   check dot (%d,%d)\n", d.row, d.col);

            if (d.row == r && d.col == c)
            {
                for (auto it = _fixedPaths.begin(); it != _fixedPaths.end(); ++it)
                {
                    if (it->color == d.color)
                    {
                        _fixedPaths.erase(it);
                        break;
                    }
                }

                printf("   >>> HIT DOT <<<\n");

                _isDragging = true;
                _startCell = Vec2(r, c);
                _currentCell = _startCell;
                _currentColor = d.color;
                _currentPath.push_back(_startCell);

                redrawPath();
                break;
            }
        }

        if (!_isDragging)
            printf(" -> click NOT on dot\n");
        if (_resetBtnRect.containsPoint(world))
        {
            resetLevel();
            return true;
        }
        return true;
    };

    // ===== MOUSE MOVE =====
    listener->onMouseMove = [this](ax::EventMouse* event) -> bool
    {
        if (!_isDragging)
            return false;

        Vec2 local = event->getLocation() - _gridOffset;

        int r, c;
        if (!nodeToGrid(local, r, c))
            return false;

        Vec2 cell(r, c);
        Vec2 last = _currentPath.back();

        // 1️⃣ ĐỨNG NGUYÊN
        if (cell == last)
            return true;

        // 2️⃣ ĐI LÙI → POP
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

        // 3️⃣ CHỈ ĐI VUÔNG GÓC
        int dx = abs(static_cast<int>(cell.x - last.x));
        int dy = abs(static_cast<int>(cell.y - last.y));
        if (dx + dy != 1)
            return false;

        // 4️⃣ KIỂM TRA DOT KHÁC MÀU
        ax::Color32 hitColor;
        if (getDotColorAt(r, c, hitColor))
        {
            if (hitColor != _currentColor)
            {
                printf("❌ BLOCK: khác màu\n");
                return false;
            }
        }

        // 5️⃣ ĐI TIẾP
        _currentPath.push_back(cell);
        redrawPath();
        // sau khi push_back(cell)
        if (isSameColorDotEnd(r, c) && Vec2(r, c) != _startCell)
        {
            printf("✅ PATH COMPLETE\n");

            // lưu path cố định
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
    // ===== MOUSE UP =====
    listener->onMouseUp = [this](ax::EventMouse*) -> bool
    {
        _isDragging = false;
        return true;
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void Scene1::redrawPath()
{
    _drawNode->clear();
    drawGrid();

    // 1️⃣ VẼ DOT
    for (auto& d : _dots)
    {
        _drawNode->drawSolidCircle(
            gridToWorld(d.row, d.col),
            12.0f,
            0,
            24,
            d.color
        );
    }

    // 2️⃣ VẼ FIXED PATHS (KHÔNG BAO GIỜ MẤT)
    for (const auto& p : _fixedPaths)
    {
        for (size_t i = 1; i < p.cells.size(); i++)
        {
            Vec2 a = gridToWorld(p.cells[i - 1].x, p.cells[i - 1].y);
            Vec2 b = gridToWorld(p.cells[i].x, p.cells[i].y);
            _drawNode->drawSegment(a, b, 6.0f, p.color);
        }
    }

    // 3️⃣ VẼ CURRENT PATH (ĐANG KÉO)
    for (size_t i = 1; i < _currentPath.size(); i++)
    {
        Vec2 a = gridToWorld(_currentPath[i - 1].x, _currentPath[i - 1].y);
        Vec2 b = gridToWorld(_currentPath[i].x, _currentPath[i].y);
        _drawNode->drawSegment(a, b, 6.0f, _currentColor);
    }
}
bool Scene1::getDotColorAt(int row, int col, ax::Color32& outColor) const
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
bool Scene1::isSameColorDotEnd(int r, int c) const
{
    for (const auto& d : _dots)
    {
        if (d.row == r && d.col == c && d.color == _currentColor)
            return true;
    }
    return false;
}

void Scene1::drawResetButton()
{
    // Vec2 pos(50, 50);
    Vec2 posBtn(_director->getVisibleOrigin().x/2,_director->getVisibleOrigin().y*4);
    Size size(120, 40);
    _resetBtnRect = Rect(posBtn.x, posBtn.y , 
                    size.width, size.height);
    // Vec2 posBtn(_director->getVisibleOrigin().x/2,_director->getVisibleOrigin().y*4);
    _uiNode->drawSolidRect(
        posBtn,
        posBtn + Vec2(size.width, size.height),
        Color32(80, 80, 80, 255)
    );

    _uiNode->drawRect(
        posBtn,
        posBtn + Vec2(size.width, size.height),
        Color32::WHITE
    );
    auto label = Label::createWithTTF("Reset", "fonts/Marker Felt.ttf", 24);
    this->addChild(_uiNode, 21);
}
void Scene1::resetLevel()
{
    _fixedPaths.clear();
    _currentPath.clear();
    _isDragging = false;
    _levelCompleted = false;

    redrawPath();
}

void Scene1::checkLevelComplete()
{
    std::set<char> requiredIds;
    for (auto& d : _dots)
        requiredIds.insert(d.col);

    if (_fixedPaths.size() != requiredIds.size())
        return;

    std::set<std::pair<int, int>> covered;
    for (auto& p : _fixedPaths)
        for (auto& c : p.cells)
            covered.insert({ (int)c.x, (int)c.y });

    if ((int)covered.size() != _levelRows * _levelCols)
        return;

    showCompletedPopup();
}

void Scene1::showCompletedPopup()
{
    if (_levelCompleted) return;
    _levelCompleted = true;

    Vec2 center = _director->getVisibleSize() / 2;

    _uiNode->drawSolidRect(
        center - Vec2(140, 60),
        center + Vec2(140, 60),
        Color32(0, 0, 0, 180)
    );

    _uiNode->drawRect(
        center - Vec2(140, 60),
        center + Vec2(140, 60),
        Color32::WHITE
    );

    printf("🎉 LEVEL COMPLETED 🎉");
}
