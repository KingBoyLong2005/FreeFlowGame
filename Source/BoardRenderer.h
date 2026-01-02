#pragma once
#include "axmol/axmol.h"
#include "LevelLoader.h"

class BoardRenderer : public ax::Node {
public:
    CREATE_FUNC(BoardRenderer);
    
    struct Dot
    {
        int row;
        int col;
        ax::Color32 color;
    };

    bool init() override;
    
    void setLevelData(const LevelData& data);
    ax::Vec2 gridToWorld(int row, int col) const;
    bool nodeToGrid(const ax::Vec2& local, int& outRow, int& outCol) const;

    void drawGrid();
    void drawDots();
    
    float _cellSize = 64.0f;
    std::vector<Dot> _dots;
    int _levelRows = 0;
    int _levelCols = 0;
    ax::Vec2 _gridOffset;
    ax::DrawNode* _drawNode = nullptr;

private:
    LevelData _levelData;
};