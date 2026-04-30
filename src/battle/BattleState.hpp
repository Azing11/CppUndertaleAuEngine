#pragma once
#include "core/State.hpp"
#include "core/AssetManager.hpp"
#include "shader/DeformableSprite.hpp"
#include "core/Resources.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
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

    //sans精灵图像
    DeformableSprite sansLegSprite_;
    sf::Sprite sansHeadSprite_;
    sf::Sprite sansBodySprite_;

    //box
    sf::RectangleShape box;

    sf::Music music_;
    sf::Clock battleClock;  // 用于计时
    

    // SansConfig
    struct sansConfig {
        float time_ = 0;

        float swayX_ = 0;
        float swayY_ = 0;

        float swayScorpX_ = 2.0f/1000.0f;
        float swayScorpY_ = 4.0f/1000.0f;
        float swayFreqX_ = 4;
        float swayFreqY_ = 2;

        // 基础位置
        float baseX_ = 0; //320
        float baseY_ = 0; //160
    
        //腿偏移量
        float legOffsetX_ = -43.0f;
        float legOffsetY_ = -56.0f;
        float legBaseX_() const { return baseX_ + legOffsetX_; }
        float legBaseY_() const { return baseY_ + legOffsetY_; }
        float legWidth_ = 90;
        float legHeight_ = 50;

        //用于平滑移动
        bool ifSmooth_ = false;
        float targetX_ = 320;
        float targetY_ = 200;
        //平滑速率
        float SmoothFactor_ = 0.1f;  // 1/10
    }sans_; //可以设置多审

    //sans
    void setSansPosition(float x, float y, bool ifSmooth, float factor = 0.1f);
    void setSansSwayFactor(float x, float y);
    void updateSansAnimation(float t);
    void drawSans(sf::RenderWindow& window);

    //boxConfig
    struct boxConfig {
        float x_ = 0;
        float y_ = 0;

        float weight_ = 0;
        float height_ = 0;

        //Smooth
        bool ifSmooth_ = true;
        float SmoothFactor_ = 0.1f;

        float targetX_ = 0;
        float targetY_ = 0;

        float targetWeight_ = 0;
        float targetHeight_ = 0;
    }box_;
    
    //box
    void setBoxPosition(float x, float y, bool ifSmooth);
    void setBoxSize(float weight, float height, bool ifSmooth);
    void updateBox();
    void drawBox(sf::RenderWindow& window);

    //soul
    struct soulConfig {
        bool ifDisplay_ = true;
        float x_ = 0;
        float y_ = 0;

        float dir_ = 0;
        float size_ = 0;
    };
    void setSoulPosition(float x, float y, bool ifSmooth);
    void setSoulSize(float size, bool ifSmooth);
    void updateSoul();
    void drawSoul(sf::RenderWindow& window);
};