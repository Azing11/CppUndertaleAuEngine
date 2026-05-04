#pragma once
#include "BattleUtils.hpp"
#include "core/AssetManager.hpp"
#include "core/Resources.hpp"
#include "shader/DeformableSprite.hpp"
#include <SFML/Graphics.hpp>
#include <string>

class ButtonComponent {
public:
    ButtonComponent(AssetManager& assets);

    void handleEvent(const sf::Event& event);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    // 启用/禁用选择功能
    void setSelectable(bool selectable);
    bool isSelectable() const;

    // 直接设置选中项（-1 表示无选择）
    void setSelection(int index);
    int getSelection() const;
    void setSelected(bool selected) { this->selected = selected; }
    bool getSelected();
    float getSelectionXPosition();
    float getSelectionYPosition();

private:
    void updateButtonVisuals();

    sf::Sprite& getSprite(int index);
    std::string getTexturePath(int index, bool selected);
    float getBaseY() const { return 450.0f; }

    AssetManager& assets_;

    sf::Sprite fight_, act_, item_, mercy_;

    int selection_ = 0;
    bool selected = false;
    bool selectable_ = true;

    // 每个按钮的当前 Y（用于平滑上浮）
    float currentY_[4] = {450.0f, 450.0f, 450.0f, 450.0f};
    static constexpr float FLOAT_OFFSET = 8.0f;
    static constexpr float SMOOTH_FACTOR = 0.25f;
};

inline ButtonComponent::ButtonComponent(AssetManager& assets)
    : assets_(assets)
    , fight_(assets.getTexture(Res::uiTexture::FIGHT))
    , act_(assets.getTexture(Res::uiTexture::ACT))
    , item_(assets.getTexture(Res::uiTexture::ITEM))
    , mercy_(assets.getTexture(Res::uiTexture::MERCY))
{
    struct BtnInfo {
        sf::Sprite& sprite;
        float x;
    };

    BtnInfo buttons[4] = {
        {fight_, 90},
        {act_, 240},
        {item_, 390},
        {mercy_, 540}
    };

    for (auto& btn : buttons) {
        auto bounds = btn.sprite.getLocalBounds();
        btn.sprite.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        btn.sprite.setScale({1.0f, 1.0f});
        btn.sprite.setPosition({btn.x, getBaseY()});
    }

    // 初始化选中状态
    fight_.setTexture(assets_.getTexture(Res::uiTexture::FIGHT_SELECTED));
}

inline void ButtonComponent::setSelectable(bool selectable) {
    selectable_ = selectable;

    if (!selectable_) {
        // 禁用时：全部恢复普通纹理，全部回落到底部
        fight_.setTexture(assets_.getTexture(Res::uiTexture::FIGHT));
        act_.setTexture(assets_.getTexture(Res::uiTexture::ACT));
        item_.setTexture(assets_.getTexture(Res::uiTexture::ITEM));
        mercy_.setTexture(assets_.getTexture(Res::uiTexture::MERCY));
    }
    else {
        // 启用时：恢复当前选中项的高亮
        fight_.setTexture(assets_.getTexture(getTexturePath(0, selection_ == 0)));
        act_.setTexture(assets_.getTexture(getTexturePath(1, selection_ == 1)));
        item_.setTexture(assets_.getTexture(getTexturePath(2, selection_ == 2)));
        mercy_.setTexture(assets_.getTexture(getTexturePath(3, selection_ == 3)));
    }
}

inline bool ButtonComponent::isSelectable() const {
    return selectable_;
}

inline void ButtonComponent::setSelection(int index) {
    if (index < -1 || index > 3) return;
    selection_ = (index == -1) ? 0 : index;

    if (selectable_) {
        fight_.setTexture(assets_.getTexture(getTexturePath(0, selection_ == 0)));
        act_.setTexture(assets_.getTexture(getTexturePath(1, selection_ == 1)));
        item_.setTexture(assets_.getTexture(getTexturePath(2, selection_ == 2)));
        mercy_.setTexture(assets_.getTexture(getTexturePath(3, selection_ == 3)));
    }
}

inline int ButtonComponent::getSelection() const {
    return selectable_ ? selection_ : -1;
}

inline bool ButtonComponent::getSelected() {
    return selected;
}

inline float ButtonComponent::getSelectionXPosition() {
    float xPositions[4] = {90, 240, 390, 540};
    return xPositions[getSelection()];
}

inline float ButtonComponent::getSelectionYPosition() {
    return currentY_[getSelection()];
}

inline void ButtonComponent::handleEvent(const sf::Event& event) {
    if (!selectable_) return;
    if (!event.is<sf::Event::KeyReleased>()) return;

    const auto* keyEvent = event.getIf<sf::Event::KeyReleased>();
    bool changed = false;

    if (keyEvent->code == sf::Keyboard::Key::Left) {
        selection_ = (selection_ - 1 + 4) % 4;
        changed = true;
    }
    else if (keyEvent->code == sf::Keyboard::Key::Right) {
        selection_ = (selection_ + 1) % 4;
        changed = true;
    }
    else if (keyEvent->code == sf::Keyboard::Key::Enter || keyEvent->code == sf::Keyboard::Key::Z) {
        selected = true;
    }

    if (changed) {
        fight_.setTexture(assets_.getTexture(getTexturePath(0, selection_ == 0)));
        act_.setTexture(assets_.getTexture(getTexturePath(1, selection_ == 1)));
        item_.setTexture(assets_.getTexture(getTexturePath(2, selection_ == 2)));
        mercy_.setTexture(assets_.getTexture(getTexturePath(3, selection_ == 3)));
    }
}

inline void ButtonComponent::updateButtonVisuals() {
    sf::Sprite* sprites[4] = {&fight_, &act_, &item_, &mercy_};
    float xPositions[4] = {90, 240, 390, 540};

    for (int i = 0; i < 4; ++i) {
        float targetY = getBaseY() - (selectable_ && i == selection_ ? FLOAT_OFFSET : 0.0f);

        currentY_[i] += (targetY - currentY_[i]) * SMOOTH_FACTOR;

        if (std::abs(targetY - currentY_[i]) < 0.3f) {
            currentY_[i] = targetY;
        }

        sprites[i]->setPosition({xPositions[i], currentY_[i]});
    }
}

inline void ButtonComponent::update(float deltaTime) {
    updateButtonVisuals();
}

inline sf::Sprite& ButtonComponent::getSprite(int index) {
    switch (index) {
        case 0: return fight_;
        case 1: return act_;
        case 2: return item_;
        case 3: return mercy_;
        default: return fight_;
    }
}

inline std::string ButtonComponent::getTexturePath(int index, bool selected) {
    switch (index) {
        case 0: return selected ? Res::uiTexture::FIGHT_SELECTED : Res::uiTexture::FIGHT;
        case 1: return selected ? Res::uiTexture::ACT_SELECTED   : Res::uiTexture::ACT;
        case 2: return selected ? Res::uiTexture::ITEM_SELECTED  : Res::uiTexture::ITEM;
        case 3: return selected ? Res::uiTexture::MERCY_SELECTED : Res::uiTexture::MERCY;
        default: return Res::uiTexture::FIGHT;
    }
}

inline void ButtonComponent::draw(sf::RenderWindow& window) {
    window.draw(fight_);
    window.draw(act_);
    window.draw(item_);
    window.draw(mercy_);
}
