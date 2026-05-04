#pragma once
#include "BattleUtils.hpp"
#include "SansComponent.hpp"
#include "BoxComponent.hpp"
#include "SoulComponent.hpp"
#include "StatusBarComponent.hpp"
#include "ButtonComponent.hpp"
#include "core/baseData.hpp"
#include "core/Typewriter.hpp"

class TurnController {
public:
    explicit TurnController(AssetManager& assets);
    void init();
    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderWindow& window);

    void startTurn(baseData::Turn::Owner owner) {
        if (turn_.currentOwner != owner) { // 防重复
            turn_.currentOwner = owner;
        }
        onTurnChanged();  // 回合切换时的设置
    }
    
    void setPlayerPhase(baseData::Turn::PlayerPhase phase) {
        turn_.currentPlayerPhase = phase;
        onPhaseChanged();  // 玩家阶段切换时的设置
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
    void onPhaseChanged(); // 玩家阶段切换回调
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
    Typewriter typer_;
    
    baseData::Turn turn_;

    using Owner = baseData::Turn::Owner;
    using PlayerPhase = baseData::Turn::PlayerPhase;
    const float SOUL_OFFSET_X = -40;
};

TurnController::TurnController(AssetManager& assets)
    : assets_(assets)
    , background_(assets.getTexture(Res::uiTexture::BACKGROUND))
    , sans_(assets)
    , box_(assets)
    , soul_(assets)
    , statusBar_(assets)
    , button_(assets)
    , typer_(assets.getFont(Res::fonts::MENU_FONT), 24)
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
    startTurn(Owner::Player);

    sans_.setPosition(320.0f, 320.0f - 65.0f - 40.0f, false);
    //soul_.setPosition(320.0f, 310.0f, false);
    //soul_.setDir(0.0f, false);
}

void TurnController::onTurnChanged() {
    if (turn_.currentOwner == Owner::Player) {
        soul_.setMode(SoulComponent::Mode::Menu);
        button_.setSelectable(true);
        box_.setPosition(320.0f, 320.0f, true);
        box_.setSize(283.0f, 283.0f, 65.0f, 65.0f, true);
        box_.setAngle(0.0f, true);
        setPlayerPhase(PlayerPhase::Selecting);
    } else {
        button_.setSelectable(false);
    }
}

void TurnController::onPhaseChanged() {
    if (turn_.currentOwner == Owner::Player) {
        if (turn_.currentPlayerPhase == PlayerPhase::Selecting) {
            button_.setSelectable(true);
        }
        else if(turn_.currentPlayerPhase == PlayerPhase::OnBranch) {
            button_.setSelected(false);
            button_.setSelectable(false);
            typer_.print("*you chosed " + std::to_string(button_.getSelection()), 320.0f, 200.0f);
        }
        else if(turn_.currentPlayerPhase == PlayerPhase::Result) {
            
        }
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
    typer_.update(dt);

    if (turn_.isEnded()) { end(); }

    if (turn_.currentOwner == Owner::Player)
    {
        if(turn_.currentPlayerPhase == PlayerPhase::Selecting){
            button_.setSelectable(true); 
            auto Selection = button_.getSelection();
            soul_.setDir(90.0f);
            soul_.setDisplayable(true);
            soul_.setPosition(button_.getSelectionXPosition() + SOUL_OFFSET_X, button_.getSelectionYPosition());
            if(button_.getSelected()){
                setPlayerPhase(PlayerPhase::OnBranch);
                printf("Player selected option %d\n", Selection);
            }
        }

    }
}

inline void TurnController::render(sf::RenderWindow& window) {
    window.draw(background_);
    sans_.draw(window);
    box_.draw(window);
    statusBar_.draw(window);
    button_.draw(window);
    soul_.draw(window);
    typer_.draw(window);
}

void TurnController::end() {

}