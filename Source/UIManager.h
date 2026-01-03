#pragma once
#include "axmol/axmol.h"

class UIManager : public ax::Node {
public:
    CREATE_FUNC(UIManager);
    
    bool init() override;
    void drawResetButton();
    void showCompletedPopup();
    void hideCompletedPopup();

    ax::DrawNode* _uiNode = nullptr;
    ax::Rect _resetBtnRect;
    ax::Rect _nextLevelBtnRect;
    ax::Rect _restartBtnRect;
    bool _levelCompleted = false;

private:
    ax::Label* _label = nullptr;
    ax::Node* _popupNode = nullptr;  // Node chứa popup để dễ xóa
};