#include "Game.hpp"
#include "battleState/BattleState.hpp"
#include "Resources.hpp"
#include <iostream>

Game::Game() 
    : window_(sf::VideoMode({WIDTH, HEIGHT}), TITLE) 
{
    window_.setFramerateLimit(60);
    
    // 预加载资源
    
    //=========sans=========
    assets_.loadTexture(Res::sansTexture::SANS_HEAD_IDLE, "resources/sprites/Sans/head/sans_head_idle.png");
    assets_.loadTexture(Res::sansTexture::SANS_HEAD_ONE_EYE_CLOSING, "resources/sprites/Sans/head/sans_head_one_eye_closing.png");

    assets_.loadTexture(Res::sansTexture::SANS_BODY_IDLE, "resources/sprites/Sans/body/sans_body_idle.png");

    assets_.loadTexture(Res::sansTexture::SANS_LEG, "resources/sprites/Sans/leg/sans_leg_.png");

    //=========soul=========
    assets_.loadTexture(Res::soulTexture::SOUL, "resources/sprites/battle/soul/soul_.png");
    assets_.loadTexture(Res::soulTexture::SOUL_LIGHT, "resources/sprites/battle/soul/soul_light.png");
    
    //=========ui=========
    assets_.loadTexture(Res::uiTexture::KR, "resources/sprites/battle/ui/ui_kr.png");
    assets_.loadTexture(Res::uiTexture::HP, "resources/sprites/battle/ui/ui_hp.png");
    assets_.loadTexture(Res::uiTexture::PIXEL, "resources/sprites/battle/ui/pixel.png");
    assets_.loadTexture(Res::uiTexture::BACKGROUND, "resources/sprites/battle/ui/background.png");
    assets_.loadTexture(Res::uiTexture::BOX_BG, "resources/sprites/battle/ui/box_background.png");

    assets_.loadTexture(Res::uiTexture::FIGHT, "resources/sprites/battle/button/fight_.png");
    assets_.loadTexture(Res::uiTexture::FIGHT_SELECTED, "resources/sprites/battle/button/fight_selected.png");

    assets_.loadTexture(Res::uiTexture::ACT, "resources/sprites/battle/button/act_.png");
    assets_.loadTexture(Res::uiTexture::ACT_SELECTED, "resources/sprites/battle/button/act_selected.png");

    assets_.loadTexture(Res::uiTexture::ITEM, "resources/sprites/battle/button/item_.png");
    assets_.loadTexture(Res::uiTexture::ITEM_SELECTED, "resources/sprites/battle/button/item_selected.png");

    assets_.loadTexture(Res::uiTexture::MERCY, "resources/sprites/battle/button/mercy_.png");
    assets_.loadTexture(Res::uiTexture::MERCY_SELECTED, "resources/sprites/battle/button/mercy_selected.png");

    //=========font=========
    assets_.loadFont(Res::fonts::STATUS_BAR, "resources/fonts/mars_needs_cunnilingus.ttf");
    assets_.loadFont(Res::fonts::MENU_FONT, "resources/fonts/menu.otf");
    assets_.loadFont(Res::fonts::SANS_FONT, "resources/fonts/sans.ttf");
    
    //======sfx======
    assets_.loadSound("sfx_dong", "resources/audio/sfx/snd_dong.wav");
    
    
    // 压入状态BattleState
    states_.push(std::make_unique<BattleState>(assets_));
}

void Game::run() {
    sf::Clock clock;

    while (window_.isOpen() && !states_.shouldQuit()) {
        float dt = clock.restart().asSeconds();
        
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    while (const auto event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F2)){
            //states_.restart();
            //std::cout<<"pressed f2";
        }
        states_.handleEvent(*event);
    }
}

void Game::update(float dt) {
    states_.update(dt);
}

void Game::render() {
    window_.clear(sf::Color::Black);
    states_.render(window_);
    window_.display();
}