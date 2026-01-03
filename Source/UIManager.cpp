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

void UIManager::showCompletedPopup()
{
    if (_levelCompleted) 
        return;
        
    _levelCompleted = true;

    auto director = Director::getInstance();
    auto visibleSize = director->getVisibleSize();
    auto origin = director->getVisibleOrigin();
    
    Vec2 center = origin + visibleSize / 2;

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

    auto label = Label::createWithTTF("LEVEL COMPLETED!", "fonts/Marker Felt.ttf", 28);
    if (label)
    {
        label->setPosition(center);
        this->addChild(label, 22);
    }

    printf("🎉 LEVEL COMPLETED POPUP SHOWN 🎉\n");
}