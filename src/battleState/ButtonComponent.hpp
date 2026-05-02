#pragma once
#include "BattleUtils.hpp"
#include "core/AssetManager.hpp"
#include "core/Resources.hpp"
#include "shader/DeformableSprite.hpp"
#include <SFML/Graphics.hpp>

class ButtonComponent {
public:
    ButtonComponent(AssetManager& assets);

    void update();
    void draw(sf::RenderWindow& window);

private:

    sf::Sprite fight_, act_, item_, mercy_;
};

inline ButtonComponent::ButtonComponent(AssetManager& assets)
    : fight_(assets.getTexture(Res::uiTexture::FIGHT))
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
        btn.sprite.setOrigin({bounds.size.x / 2, bounds.size.y / 2});
        btn.sprite.setScale({1.0f, 1.0f});
        btn.sprite.setPosition({btn.x, 450});
    }
}

inline void ButtonComponent::update() {

}

inline void ButtonComponent::draw(sf::RenderWindow& window) {
    window.draw(fight_);
    window.draw(act_);
    window.draw(item_);
    window.draw(mercy_);
}