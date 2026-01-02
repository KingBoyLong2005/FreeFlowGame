#pragma once
#include "axmol/axmol.h"

class UIManager : public ax::Node {
public:
    CREATE_FUNC(UIManager);
    
    bool init() override;
    void drawResetButton();

    ax::DrawNode* _uiNode = nullptr;
    ax::Rect _resetBtnRect;

private:
    ax::Label* _label = nullptr;
};