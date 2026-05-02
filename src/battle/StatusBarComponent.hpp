#pragma once
#include "core/AssetManager.hpp"
#include "core/Resources.hpp"
#include "core/baseData.hpp"
#include <SFML/Graphics.hpp>
#include <string>

class StatusBarComponent {
public:
    StatusBarComponent(AssetManager& assets);

    void init();
    void update();
    void draw(sf::RenderWindow& window);

    baseData::player& getPlayer() { return player_; }
    void setKr(bool kr) { showKr_ = kr; }

private:
    baseData::player player_;
    sf::Text nameText_;
    sf::Text hpText_;
    sf::Sprite hpSprite_;
    sf::Sprite krSprite_;
    sf::RectangleShape maxHpRect_;
    sf::RectangleShape currentHpRect_;

    bool showKr_ = true;
    float smoothFactor_ = 0.1f;
    float visualHp_ = 0;
};

inline StatusBarComponent::StatusBarComponent(AssetManager& assets)
    : nameText_(assets.getFont(Res::fonts::STATUS_BAR))
    , hpText_(assets.getFont(Res::fonts::STATUS_BAR))
    , hpSprite_(assets.getTexture(Res::uiTexture::HP))
    , krSprite_(assets.getTexture(Res::uiTexture::KR))
{
    nameText_.setCharacterSize(26);
    nameText_.setFillColor(sf::Color::White);
    nameText_.setPosition({35, 390});
    nameText_.setLineSpacing(1.0f);

    hpText_.setCharacterSize(26);
    hpText_.setFillColor(sf::Color::White);
    hpText_.setLineSpacing(1.0f);

    hpSprite_.setPosition({220, 402});
    hpSprite_.setColor(sf::Color::White);
    hpSprite_.setScale(sf::Vector2f(1.2f, 1.2f));

    krSprite_.setColor(sf::Color::White);
    krSprite_.setScale(sf::Vector2f(1.2f, 1.2f));

    auto maxHpBounds = maxHpRect_.getLocalBounds();
    maxHpRect_.setOrigin({maxHpBounds.size.x / 2, maxHpBounds.size.y / 2});
    maxHpRect_.setPosition({260, 397});
    maxHpRect_.setFillColor(sf::Color::Red);
    maxHpRect_.setSize(sf::Vector2f(1.0f, 1.0f));

    auto curHpBounds = currentHpRect_.getLocalBounds();
    currentHpRect_.setOrigin({curHpBounds.size.x / 2, curHpBounds.size.y / 2});
    currentHpRect_.setPosition({260, 397});
    currentHpRect_.setFillColor(sf::Color::Yellow);
    currentHpRect_.setSize(sf::Vector2f(1.0f, 1.0f));
}

inline void StatusBarComponent::init() {
    player_.setName("FRISK");
    player_.setLv(1);
    player_.setCurrentHp(player_.getMaxHp());
    maxHpRect_.setScale(sf::Vector2f(1.2f * player_.getMaxHp(), 20.0f));
}

inline void StatusBarComponent::update() {
    visualHp_ += (player_.getCurrentHp() - visualHp_) * smoothFactor_;
    currentHpRect_.setScale(sf::Vector2f(1.2f * visualHp_, 20.0f));

    showKr_ ? hpText_.setFillColor(sf::Color(255, 0, 255, 255))
            : hpText_.setFillColor(sf::Color::White);

    krSprite_.setPosition({270 + player_.getMaxHp() * 1.2f, 402});
    hpText_.setPosition({320 + player_.getMaxHp() * 1.2f, 390});

    nameText_.setString(player_.getName() + "  LV " + std::to_string(player_.getLv()));
    hpText_.setString(std::to_string(player_.getCurrentHp()) + " / " + std::to_string(player_.getMaxHp()));
}

inline void StatusBarComponent::draw(sf::RenderWindow& window) {
    window.draw(nameText_);
    window.draw(hpSprite_);
    window.draw(maxHpRect_);
    window.draw(currentHpRect_);
    window.draw(hpText_);
    window.draw(krSprite_);
}
