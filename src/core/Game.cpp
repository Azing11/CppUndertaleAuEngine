#include "Game.hpp"
#include "battleState/BattleState.hpp"
#include "Resources.hpp"
#include <array>

namespace {
    using AssetEntry = std::pair<const char*, const char*>;

    constexpr std::array<AssetEntry, 19> textureEntries = {
        AssetEntry{Res::sansTexture::SANS_HEAD_IDLE, "resources/sprites/Sans/head/sans_head_idle.png"},
        AssetEntry{Res::sansTexture::SANS_HEAD_ONE_EYE_CLOSING, "resources/sprites/Sans/head/sans_head_one_eye_closing.png"},
        AssetEntry{Res::sansTexture::SANS_BODY_IDLE, "resources/sprites/Sans/body/sans_body_idle.png"},
        AssetEntry{Res::sansTexture::SANS_LEG, "resources/sprites/Sans/leg/sans_leg_.png"},
        AssetEntry{Res::soulTexture::SOUL, "resources/sprites/battle/soul/soul_.png"},
        AssetEntry{Res::soulTexture::SOUL_LIGHT, "resources/sprites/battle/soul/soul_light.png"},
        AssetEntry{Res::uiTexture::KR, "resources/sprites/battle/ui/ui_kr.png"},
        AssetEntry{Res::uiTexture::HP, "resources/sprites/battle/ui/ui_hp.png"},
        AssetEntry{Res::uiTexture::PIXEL, "resources/sprites/battle/ui/pixel.png"},
        AssetEntry{Res::uiTexture::BACKGROUND, "resources/sprites/battle/ui/background.png"},
        AssetEntry{Res::uiTexture::BOX_BG, "resources/sprites/battle/ui/box_background.png"},
        AssetEntry{Res::uiTexture::FIGHT, "resources/sprites/battle/button/fight_.png"},
        AssetEntry{Res::uiTexture::FIGHT_SELECTED, "resources/sprites/battle/button/fight_selected.png"},
        AssetEntry{Res::uiTexture::ACT, "resources/sprites/battle/button/act_.png"},
        AssetEntry{Res::uiTexture::ACT_SELECTED, "resources/sprites/battle/button/act_selected.png"},
        AssetEntry{Res::uiTexture::ITEM, "resources/sprites/battle/button/item_.png"},
        AssetEntry{Res::uiTexture::ITEM_SELECTED, "resources/sprites/battle/button/item_selected.png"},
        AssetEntry{Res::uiTexture::MERCY, "resources/sprites/battle/button/mercy_.png"},
        AssetEntry{Res::uiTexture::MERCY_SELECTED, "resources/sprites/battle/button/mercy_selected.png"}
    };

    constexpr std::array<AssetEntry, 3> fontEntries = {
        AssetEntry{Res::fonts::STATUS_BAR, "resources/fonts/mars_needs_cunnilingus.ttf"},
        AssetEntry{Res::fonts::MENU_FONT, "resources/fonts/menu.otf"},
        AssetEntry{Res::fonts::SANS_FONT, "resources/fonts/sans.ttf"}
    };

    constexpr std::array<AssetEntry, 1> soundEntries = {
        AssetEntry{Res::sfx::SFX_DONG, "resources/audio/sfx/snd_dong.wav"}
    };
}

Game::Game()
    : window_(sf::VideoMode({WIDTH, HEIGHT}), TITLE)
{
    window_.setFramerateLimit(60);
    loadResources();
    states_.push(std::make_unique<BattleState>(assets_));
}

void Game::run() {
    sf::Clock clock;

    while (window_.isOpen() && !states_.shouldQuit()) {
        const float dt = clock.restart().asSeconds();
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

void Game::loadResources() {
    for (const auto& [id, path] : textureEntries) {
        assets_.loadTexture(id, path);
    }

    for (const auto& [id, path] : fontEntries) {
        assets_.loadFont(id, path);
    }

    for (const auto& [id, path] : soundEntries) {
        assets_.loadSound(id, path);
    }
}