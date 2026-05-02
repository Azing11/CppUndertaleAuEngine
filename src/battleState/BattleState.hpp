#pragma once
#include "core/State.hpp"
#include "core/AssetManager.hpp"
#include "BattleUtils.hpp"
#include "SansComponent.hpp"
#include "BoxComponent.hpp"
#include "SoulComponent.hpp"
#include "StatusBarComponent.hpp"
#include "ButtonComponent.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

class BattleState : public State {
public:
    explicit BattleState(AssetManager& assets);

    void enter() override;
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

private:
    AssetManager& assets_;

    sf::Sprite background_;
    sf::Music music_;
    sf::Clock battleClock_;

    SansComponent sans_;
    BoxComponent box_;
    SoulComponent soul_;
    StatusBarComponent statusBar_;
    ButtonComponent button_;
};

inline BattleState::BattleState(AssetManager& assets)
    : assets_(assets)
    , background_(assets.getTexture(Res::uiTexture::BACKGROUND))
    , sans_(assets)
    , box_(assets)
    , soul_(assets)
    , statusBar_(assets)
    , button_(assets)
{
    auto bg = background_.getLocalBounds();
    background_.setOrigin({bg.size.x / 2, bg.size.y / 2});
    background_.setScale({1.0f, 0.5f});
    background_.setColor(sf::Color::White);
    background_.setPosition(sf::Vector2f(320.0f, 240.0f));
}

void BattleState::enter() {
    battleClock_.restart();
    statusBar_.init();

    if (!music_.openFromFile(Res::audio::BATTLE_MUSIC)) {
        std::cerr << "Fail to load music\n";
    }

    box_.setPosition(320.0f, 320.0f, false);
    box_.setSize(283.0f, 283.0f, 65.0f, 65.0f, false);
    box_.setAngle(90.0f, true);
    box_.setSize(65.0f, 65.0f, 65.0f, 65.0f, true);

    sans_.setPosition(320.0f, 320.0f - 65.0f - 40.0f, false);

    soul_.setPosition(320.0f, 310.0f, false);
    soul_.setDir(0.0f, false);

    sans_.setTex(SansComponent::Part::Head, assets_.getTexture(Res::sansTexture::SANS_HEAD_ONE_EYE_CLOSING));
    button_.setSelectable(false);
}

inline void BattleState::handleEvent(const sf::Event& event) {
    button_.handleEvent(event);
}

inline void BattleState::update(float dt) {
    float t = battleClock_.getElapsedTime().asMilliseconds();
    sans_.update(t);
    box_.update();
    soul_.update(box_);
    statusBar_.update();
    button_.update(dt);
}

inline void BattleState::render(sf::RenderWindow& window) {
    window.draw(background_);
    sans_.draw(window);
    box_.draw(window);
    soul_.draw(window);
    statusBar_.draw(window);
    button_.draw(window);
}
