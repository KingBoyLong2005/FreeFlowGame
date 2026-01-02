#include "UIManager.h"

using namespace ax;

bool UIManager::init()
{
    if (!Node::init())
        return false;

    _uiNode = DrawNode::create();
    this->addChild(_uiNode, 20);
    
    return true;
}

void UIManager::drawResetButton()
{
    if (!_uiNode)
        return;

    auto director = Director::getInstance();
    auto visibleSize = director->getVisibleSize();
    auto origin = director->getVisibleOrigin();

    Vec2 posBtn(origin.x + 50, origin.y + visibleSize.height - 60);
    Size size(120, 40);
    
    _resetBtnRect = Rect(posBtn.x, posBtn.y, size.width, size.height);

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
    if (label)
    {
        label->setPosition(posBtn + Vec2(size.width / 2, size.height / 2));
        this->addChild(label, 21);
    }
    
    printf("Reset button drawn at (%.1f, %.1f)\n", posBtn.x, posBtn.y);
}