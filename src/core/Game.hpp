#pragma once
#include <SFML/Graphics.hpp>
#include "StateMachine.hpp"
#include "AssetManager.hpp"

class Game {
public:
    Game();
    void run();
    
    sf::RenderWindow& getWindow() { return window_; }
    AssetManager& getAssets() { return assets_; }
    StateMachine& getStates() { return states_; }
    
    static constexpr unsigned WIDTH = 640;
    static constexpr unsigned HEIGHT = 480;
    static constexpr const char* TITLE = "Undertale Aure";
    static constexpr const char* ASSET_PATH = "resources/";

private:
    sf::RenderWindow window_;
    AssetManager assets_;
    StateMachine states_;
    
    void processEvents();
    void update(float dt);
    void render();
    void loadResources();
};