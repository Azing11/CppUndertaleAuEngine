#pragma once
#include "core/State.hpp"
#include "core/AssetManager.hpp"
#include "shader/DeformableSprite.hpp"
#include "core/Resources.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

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


    sf::Music music_;
    sf::Clock battleClock;  // 用于计时
    

    // Sans参数
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
        float baseY_ = 160; //160

        //用于平滑移动
        int ifSmooth = true;
        float vX_ = 320;
        float vY_ = 160;

        float legBaseX_ = (baseX_ - 43);
        float legBaseY_ = (baseY_ - 56);
        float legWidth_ = 90;
        float legHeight_ = 50;

        //腿偏移量
        float legOffsetX_ = -43.0f;
        float legOffsetY_ = -56.0f;

        //平滑速率
        float SmoothFactor_ = 0.1f;  // 1/10
    }sans_; //可以设置多审
    
    //辅助函数
    void updateSansAnimation(float t);
    void drawSans(sf::RenderWindow& window);
    void drawBox();
};