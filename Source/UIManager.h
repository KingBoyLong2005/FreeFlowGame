#pragma once
#include "axmol/axmol.h"

class UIManager : public ax::Node {
public:
    CREATE_FUNC(UIManager);
    
    bool init() override;
    void drawResetButton();
    void showCompletedPopup();

    ax::DrawNode* _uiNode = nullptr;
    ax::Rect _resetBtnRect;
    bool _levelCompleted = false;

private:
    ax::Label* _label = nullptr;
};