#pragma once
#include "BoardRenderer.h"
#include "axmol/axmol.h"
#include "UIManager.h"
#include <set>

enum class GameState {
    Playing,
    Win,
    Lose
};

class GameController : public ax::Node {
public:
    CREATE_FUNC(GameController);
    
    bool init() override;
    
    void setBoardRenderer(BoardRenderer* board);
    void setUIManager(UIManager* ui);
    void setDots(const std::vector<BoardRenderer::Dot>& dots);
    void setupMouseInput();
    void resetLevel();
    void checkLevelComplete();
    void nextLevel();
    void restartCurrentLevel();

    // Input state
    bool _isDragging = false;
    ax::Vec2 _startCell;
    ax::Vec2 _currentCell;
    ax::Color32 _currentColor = ax::Color32::WHITE;
    std::vector<ax::Vec2> _currentPath;

    struct FixedPath
    {
        ax::Color32 color;
        std::vector<ax::Vec2> cells;
    };

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

private:
    BoardRenderer* _board = nullptr;
    UIManager* _ui = nullptr;
    std::vector<BoardRenderer::Dot> _dots;
    std::vector<FixedPath> _fixedPaths;
    
    void redrawPath();
    bool getDotColorAt(int row, int col, ax::Color32& outColor) const;
    bool isSameColorDotEnd(int r, int c) const;
};