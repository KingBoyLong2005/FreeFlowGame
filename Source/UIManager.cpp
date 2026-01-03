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

    // Tạo node riêng cho popup để dễ quản lý
    _popupNode = Node::create();
    this->addChild(_popupNode, 100);

    // Vẽ overlay tối (nền mờ)
    auto overlayDraw = DrawNode::create();
    overlayDraw->drawSolidRect(
        origin,
        origin + visibleSize,
        Color32(0, 0, 0, 150)  // Nền đen mờ
    );
    _popupNode->addChild(overlayDraw, 0);

    // Vẽ popup box lớn hơn
    auto popupDraw = DrawNode::create();
    Vec2 popupSize(300, 200);
    Vec2 popupTL = center - Vec2(popupSize.x / 2, popupSize.y / 2);
    Vec2 popupBR = center + Vec2(popupSize.x / 2, popupSize.y / 2);

    // Background popup
    popupDraw->drawSolidRect(
        popupTL,
        popupBR,
        Color32(40, 40, 60, 255)
    );

    // Border popup
    popupDraw->drawRect(
        popupTL,
        popupBR,
        Color32(255, 215, 0, 255)  // Viền vàng
    );
    _popupNode->addChild(popupDraw, 1);

    // Title "LEVEL COMPLETED!"
    auto titleLabel = Label::createWithTTF("LEVEL COMPLETED!", "fonts/Marker Felt.ttf", 32);
    if (titleLabel)
    {
        titleLabel->setPosition(center + Vec2(0, 60));
        titleLabel->setColor(Color32(255, 215, 0));  // Màu vàng
        _popupNode->addChild(titleLabel, 2);
    }

    // Subtitle "Great job!"
    auto subtitleLabel = Label::createWithTTF("Great job!", "fonts/Marker Felt.ttf", 20);
    if (subtitleLabel)
    {
        subtitleLabel->setPosition(center + Vec2(0, 20));
        subtitleLabel->setColor(Color32::WHITE);
        _popupNode->addChild(subtitleLabel, 2);
    }

    // ===== NÚT NEXT LEVEL =====
    Vec2 nextBtnPos = center + Vec2(-80, -50);
    Size btnSize(140, 45);
    
    _nextLevelBtnRect = Rect(
        nextBtnPos.x - btnSize.width / 2,
        nextBtnPos.y - btnSize.height / 2,
        btnSize.width,
        btnSize.height
    );

    // Vẽ nút Next Level (xanh lá)
    popupDraw->drawSolidRect(
        Vec2(_nextLevelBtnRect.getMinX(), _nextLevelBtnRect.getMinY()),
        Vec2(_nextLevelBtnRect.getMaxX(), _nextLevelBtnRect.getMaxY()),
        Color32(52, 199, 89, 255)
    );

    popupDraw->drawRect(
        Vec2(_nextLevelBtnRect.getMinX(), _nextLevelBtnRect.getMinY()),
        Vec2(_nextLevelBtnRect.getMaxX(), _nextLevelBtnRect.getMaxY()),
        Color32::WHITE
    );

    auto nextLabel = Label::createWithTTF("Next Level", "fonts/Marker Felt.ttf", 22);
    if (nextLabel)
    {
        nextLabel->setPosition(nextBtnPos);
        nextLabel->setColor(Color32::WHITE);
        _popupNode->addChild(nextLabel, 2);
    }

    // ===== NÚT RESTART =====
    Vec2 restartBtnPos = center + Vec2(80, -50);
    
    _restartBtnRect = Rect(
        restartBtnPos.x - btnSize.width / 2,
        restartBtnPos.y - btnSize.height / 2,
        btnSize.width,
        btnSize.height
    );

    // Vẽ nút Restart (cam)
    popupDraw->drawSolidRect(
        Vec2(_restartBtnRect.getMinX(), _restartBtnRect.getMinY()),
        Vec2(_restartBtnRect.getMaxX(), _restartBtnRect.getMaxY()),
        Color32(255, 149, 0, 255)
    );

    popupDraw->drawRect(
        Vec2(_restartBtnRect.getMinX(), _restartBtnRect.getMinY()),
        Vec2(_restartBtnRect.getMaxX(), _restartBtnRect.getMaxY()),
        Color32::WHITE
    );

    auto restartLabel = Label::createWithTTF("Restart", "fonts/Marker Felt.ttf", 22);
    if (restartLabel)
    {
        restartLabel->setPosition(restartBtnPos);
        restartLabel->setColor(Color32::WHITE);
        _popupNode->addChild(restartLabel, 2);
    }

    printf("🎉 LEVEL COMPLETED POPUP SHOWN 🎉\n");
    printf("Next Level Button: (%.1f, %.1f, %.1f, %.1f)\n", 
           _nextLevelBtnRect.getMinX(), _nextLevelBtnRect.getMinY(),
           _nextLevelBtnRect.getMaxX(), _nextLevelBtnRect.getMaxY());
}

void UIManager::hideCompletedPopup()
{
    if (_popupNode)
    {
        _popupNode->removeFromParent();
        _popupNode = nullptr;
    }
    _levelCompleted = false;
}