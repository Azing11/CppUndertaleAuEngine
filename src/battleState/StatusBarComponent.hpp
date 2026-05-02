#pragma once
#include "core/AssetManager.hpp"
#include "core/Resources.hpp"
#include "core/baseData.hpp"
#include <SFML/Graphics.hpp>
#include <string>

class StatusBarComponent {
public:
    explicit StatusBarComponent(AssetManager& assets);

    void init();
    void update(float dt);  // 传 dt 做帧率无关平滑
    void draw(sf::RenderWindow& window);

    void setPlayerName(const std::string& name) { player_.name = name; dirty_ = true; }
    void setPlayerLv(int lv) { player_.lv = lv; dirty_ = true; }
    void setPlayerHp(int hp) { player_.currentHp = hp; }
    void setShowKr(bool show) { showKr_ = show; }

private:
    baseData::Player player_;
    sf::Text nameText_;
    sf::Text hpText_;
    sf::Sprite hpSprite_;
    sf::Sprite krSprite_;
    sf::RectangleShape maxHpRect_;
    sf::RectangleShape currentHpRect_;

    bool showKr_ = true;
    bool dirty_ = true;  // 文本是否需要更新
    float visualHp_ = 0;
    float smoothSpeed_ = 15.0f;  // 每秒衰减系数
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

    hpText_.setCharacterSize(26);
    hpText_.setFillColor(sf::Color::White);

    hpSprite_.setPosition({220, 402});
    hpSprite_.setScale({1.2f, 1.2f});

    krSprite_.setScale({1.2f, 1.2f});

    maxHpRect_.setPosition({260, 397});
    maxHpRect_.setFillColor(sf::Color::Red);

    currentHpRect_.setPosition({260, 397});
    currentHpRect_.setFillColor(sf::Color::Yellow);
}

inline void StatusBarComponent::init() {
    player_.name = "FRISK";
    player_.lv = 1;
    player_.currentHp = player_.maxHp();
    visualHp_ = static_cast<float>(player_.currentHp);
    dirty_ = true;
    
    maxHpRect_.setSize(sf::Vector2f(1.2f * player_.maxHp(), 20.0f));
}

inline void StatusBarComponent::update(float dt) {
    // 帧率指数平滑
    float alpha = 1.0f - std::exp(-smoothSpeed_ * dt);
    visualHp_ += (player_.currentHp - visualHp_) * alpha;
    currentHpRect_.setSize(sf::Vector2f(1.2f * visualHp_, 20.0f));

    hpText_.setFillColor(showKr_ ? sf::Color(255, 0, 255) : sf::Color::White);

    krSprite_.setPosition({270 + player_.maxHp() * 1.2f, 402});
    hpText_.setPosition({320 + player_.maxHp() * 1.2f, 390});

    if (dirty_) {
        nameText_.setString(player_.name + "  LV " + std::to_string(player_.lv));
        hpText_.setString(std::to_string(player_.currentHp) + " / " + std::to_string(player_.maxHp()));
        dirty_ = false;
    }
}

inline void StatusBarComponent::draw(sf::RenderWindow& window) {
    window.draw(nameText_);
    window.draw(hpSprite_);
    window.draw(maxHpRect_);
    window.draw(currentHpRect_);
    window.draw(hpText_);
    if (showKr_) window.draw(krSprite_);
}