#include "Game.hpp"
#include "battle/BattleState.hpp"
#include <iostream>

Game::Game() 
    : window_(sf::VideoMode({WIDTH, HEIGHT}), TITLE) 
{
    window_.setFramerateLimit(60);
    
    // 预加载资源
    {
            //=========sans=========
        {
            assets_.loadTexture("sans_head_idle", "resources/sprites/Sans/head/sans_head_idle.png");
            assets_.loadTexture("sans_head_one_eye_closed", "resources/sprites/Sans/head/sans_head_one_eye_closed.png");

            assets_.loadTexture("sans_body_idle", "resources/sprites/Sans/body/sans_body_idle.png");

            assets_.loadTexture("sans_leg", "resources/sprites/Sans/leg/sans_leg_.png");
        }   
            //=========soul=========
        {
            assets_.loadTexture("soul_", "resources/sprites/battle/soul/soul_.png");
            assets_.loadTexture("soul_light", "resources/sprites/battle/soul/soul_light.png");
        }   
            //=========ui=========
        {
            assets_.loadTexture("ui_pixel", "resources/sprites/battle/ui/pixel.png");
            assets_.loadTexture("box_background", "resources/sprites/battle/ui/background.png");
        }   
            //=========font=========
        {

        }  
            //======sfx======
        {
            assets_.loadSound("sfx_dong", "resources/audio/sfx/snd_dong.wav");
        }   
    }
    // 初始状态
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