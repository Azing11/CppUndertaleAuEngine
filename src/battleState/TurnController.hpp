#pragma once
#include "BattleUtils.hpp"
#include "SansComponent.hpp"
#include "BoxComponent.hpp"
#include "SoulComponent.hpp"
#include "StatusBarComponent.hpp"
#include "ButtonComponent.hpp"
#include "core/baseData.hpp"

class TurnController {
public:
    explicit TurnController(AssetManager& assets);
    void init();
    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderWindow& window);

    void startTurn(baseData::Turn::Owner owner) {
        if (turn_.currentOwner == owner) return;  // 防重复
        turn_.currentOwner = owner;
        onTurnChanged();  // 回合切换时的设置
    }
    
    void endCurrentTurn() {
        turn_.currentOwner = (turn_.currentOwner == baseData::Turn::Owner::Player)
                             ? baseData::Turn::Owner::Enemy
                             : baseData::Turn::Owner::Player;
        ++turn_.currentRound;
        onTurnChanged();
    }
    
    void setTurnCount(int n) { turn_.currentRound = n; }
    int getTurnCount() const { return turn_.currentRound; }

private:
    void onTurnChanged();  // 回合切换回调
    void end();

    AssetManager& assets_;

    sf::Sprite background_;
    sf::Music music_;
    sf::Clock battleClock_;

    SansComponent sans_;
    BoxComponent box_;
    SoulComponent soul_;
    StatusBarComponent statusBar_;
    ButtonComponent button_;
    
    baseData::Turn turn_;

    using Owner = baseData::Turn::Owner;
};

TurnController::TurnController(AssetManager& assets)
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
    background_.setPosition(sf::Vector2f(320.0f, 240.0f));
}

void TurnController::init() {
    battleClock_.restart();
    statusBar_.init();

    if (!music_.openFromFile(Res::audio::BATTLE_MUSIC)) { std::cerr << "Fail to load music\n"; }

    setTurnCount(0);
    startTurn(Owner::Enemy);

    sans_.setPosition(320.0f, 320.0f - 65.0f - 40.0f, false);
    soul_.setPosition(320.0f, 310.0f, false);
    soul_.setDir(0.0f, false);
}

void TurnController::onTurnChanged() {
    if (turn_.currentOwner == Owner::Player) {
        soul_.setStatus(-1);
        button_.setSelectable(true);
        box_.setPosition(320.0f, 320.0f, true);
        box_.setSize(283.0f, 283.0f, 65.0f, 65.0f, true);
        box_.setAngle(0.0f, true);
    } else {
        button_.setSelectable(false);
    }
}

inline void TurnController::handleEvent(const sf::Event& event) {
    button_.handleEvent(event);
}

inline void TurnController::update(float dt) {
    float t = battleClock_.getElapsedTime().asMilliseconds();
    sans_.update(t);
    box_.update();
    button_.update(dt);
    soul_.update(box_);
    statusBar_.update(dt);

    if (turn_.isEnded()) { end(); }
}

inline void TurnController::render(sf::RenderWindow& window) {
    window.draw(background_);
    sans_.draw(window);
    box_.draw(window);
    soul_.draw(window);
    statusBar_.draw(window);
    button_.draw(window);
}

void TurnController::end() {
    
}