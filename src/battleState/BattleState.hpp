#pragma once
#include "core/State.hpp"
#include "core/AssetManager.hpp"
#include "BattleUtils.hpp"
#include "SansComponent.hpp"
#include "BoxComponent.hpp"
#include "SoulComponent.hpp"
#include "StatusBarComponent.hpp"
#include "ButtonComponent.hpp"
#include "TurnController.hpp"
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

    TurnController turn_;
};

BattleState::BattleState(AssetManager& assets) : assets_(assets), turn_(assets) {}

void BattleState::enter() {
    turn_.init();
}

inline void BattleState::handleEvent(const sf::Event& event) {
    turn_.handleEvent(event);
}

inline void BattleState::update(float dt) {
    turn_.update(dt);
}

inline void BattleState::render(sf::RenderWindow& window) {
    turn_.render(window);
}
