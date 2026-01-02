#pragma once

#include "axmol/axmol.h"
#include <vector>
#include <string>

class Scene1 : public ax::Scene
{
public:
    struct LevelData
    {
        int rows = 0;
        int cols = 0;

        std::vector<std::vector<int>> tiles;
        std::vector<std::vector<std::string>> objects;
    };

    bool init() override;
    void update(float delta) override;

    static bool loadFromFile(const std::string& filename, LevelData& outData);

    CREATE_FUNC(Scene1);

    Scene1();
    ~Scene1() override;

private:
    ax::DrawNode* _drawNode = nullptr;
    float _cellSize = 64.0f;

    int _levelRows = 0;
    int _levelCols = 0;
    ax::Vec2 _gridOffset;

    // ===== INPUT STATE =====
    bool _isDragging = false;
    ax::Vec2 _startCell;
    ax::Vec2 _currentCell;

    ax::Color32 _currentColor = ax::Color32::WHITE;
    std::vector<ax::Vec2> _currentPath;
    // các path đã hoàn thành
    struct FixedPath
    {
        ax::Color32 color;
        std::vector<ax::Vec2> cells;
    };

    ax::DrawNode* _uiNode = nullptr;
    std::vector<FixedPath> _fixedPaths;
    struct Dot
    {
        int row;
        int col;
        ax::Color32 color;
    };
    // reset button
    ax::Rect _resetBtnRect; 
    std::vector<Dot> _dots;

    struct ColorLess
    {
        bool operator()(const ax::Color32& a, const ax::Color32& b) const
        {
            if (a.r != b.r) return a.r < b.r;
            if (a.g != b.g) return a.g < b.g;
            if (a.b != b.b) return a.b < b.b;
            return a.a < b.a;
        }
    };

    std::set<ax::Color32, ColorLess> requiredColors;
    // state
    bool _levelCompleted = false;
    void drawResetButton();
    void resetLevel();
    void checkLevelComplete();
    void showCompletedPopup();

    // ===== FUNCTIONS =====
    ax::Vec2 gridToWorld(int row, int col) const;
    bool nodeToGrid(const ax::Vec2& local, int& outRow, int& outCol) const;
    bool getDotColorAt(int row, int col, ax::Color32& outColor) const;
    bool isSameColorDotEnd(int r, int c) const;

    void drawGrid();
    void redrawPath();
    void setupMouseInput();
};
