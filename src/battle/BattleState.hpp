#pragma once
#include "core/State.hpp"
#include "core/AssetManager.hpp"
#include "shader/DeformableSprite.hpp"
#include "core/Resources.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
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

    // Sans 精灵
    DeformableSprite sansLegSprite_;
    sf::Sprite sansHeadSprite_;
    sf::Sprite sansBodySprite_;
    
    // Soul
    sf::Sprite soulLightSprite_;
    sf::Sprite soulSprite_;

    // Box
    sf::RectangleShape boxSprite_;

    sf::Music music_;
    sf::Clock battleClock;

    // ========== Sans 配置 ==========
    struct SansConfig {
        float time_ = 0;

        float swayX_ = 0;
        float swayY_ = 0;

        float swayScorpX_ = 2.0f / 1000.0f;
        float swayScorpY_ = 4.0f / 1000.0f;
        float swayFreqX_ = 4;
        float swayFreqY_ = 2;

        float baseX_ = 0;
        float baseY_ = 0;

        float legOffsetX_ = -43.0f;
        float legOffsetY_ = -56.0f;
        float legBaseX_() const { return baseX_ + legOffsetX_; }
        float legBaseY_() const { return baseY_ + legOffsetY_; }
        float legWidth_ = 90;
        float legHeight_ = 50;

        bool ifSmooth_ = false;
        float targetX_ = 320;
        float targetY_ = 200;
        float smoothFactor_ = 0.1f;
    } sans_;

    void setSansPosition(float x, float y, bool ifSmooth, float factor = 0.1f);
    void updateSansAnimation(float t);
    void drawSans(sf::RenderWindow& window);

    // ========== Box 配置 ==========
    struct BoxConfig {
        float x_ = 0;
        float y_ = 0;

        float width_ = 0;
        float height_ = 0;

        bool ifSmooth_ = true;
        float smoothFactor_ = 0.1f;

        float targetX_ = 0;
        float targetY_ = 0;

        float targetWidth_ = 0;
        float targetHeight_ = 0;

        // 边界（内部可用区域，已考虑边框厚度）
        float left = 0;
        float top = 0;
        float right = 0;
        float bottom = 0;

        void updateBounds(float x, float y, float w, float h, float thickness) {
            left   = x + thickness;
            top    = y + thickness;
            right  = x + w - thickness;
            bottom = y + h - thickness;
        }

        float innerWidth() const  { return right - left; }
        float innerHeight() const { return bottom - top; }
    } box_;
    
    void setBoxPosition(float x, float y, bool ifSmooth = false);
    void setBoxSize(float width, float height, bool ifSmooth = false);
    void updateBox();
    void drawBox(sf::RenderWindow& window);

    // ========== Soul 配置 ==========
    struct SoulConfig {
        bool ifDisplay_ = true;
        float x_ = 0;
        float y_ = 0;
        float dir_ = 0;
        int status_ = 0;
        float moveSpeed_ = 3;
    } soul_;

    void setSoulPosition(float x, float y, bool ifSmooth = false);
    void setSoulDir(float dir);
    void updateSoul();
    void drawSoul(sf::RenderWindow& window);
};