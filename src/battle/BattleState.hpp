#pragma once
#include "core/State.hpp"
#include "core/AssetManager.hpp"
#include "shader/DeformableSprite.hpp"
#include "core/Resources.hpp"
#include "core/baseData.hpp"
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

    sf::Sprite background_;
    DeformableSprite sansLegSprite_;
    sf::Sprite sansHeadSprite_;
    sf::Sprite sansBodySprite_;
    sf::Sprite soulLightSprite_;
    sf::Sprite soulSprite_;

    DeformableSprite boxFrameSprite_;   // 边框
    DeformableSprite boxBgSprite_;      // 背景填充

    sf::Music music_;
    sf::Sound sfx_dong_;
    sf::Clock battleClock;

    baseData::player frisk;

    // ========== 工具函数 ==========
    static sf::Vector2f rotatePoint(float px, float py, float cx, float cy, float angleDeg);
    static float angleLerp(float a, float b, float t);
    
    // 获取 Box 四条边的内侧线（考虑边框厚度）
    void getInnerEdges(sf::Vector2f& tl, sf::Vector2f& tr, 
                       sf::Vector2f& bl, sf::Vector2f& br) const;
    
    // 点到线段的距离和最近点
    static float distToSegment(float px, float py, float x1, float y1, float x2, float y2);
    static sf::Vector2f closestOnSegment(float px, float py, float x1, float y1, float x2, float y2);

    // ========== Sans ==========
    struct SansConfig {
        float time_ = 0;
        float swayX_ = 0, swayY_ = 0;
        float swayScorpX_ = 2.0f / 1000.0f;
        float swayScorpY_ = 4.0f / 1000.0f;
        float swayFreqX_ = 4, swayFreqY_ = 2;
        float baseX_ = 0, baseY_ = 0;
        float legOffsetX_ = -43.0f, legOffsetY_ = -57.0f;
        float legBaseX_() const { return baseX_ + legOffsetX_; }
        float legBaseY_() const { return baseY_ + legOffsetY_; }
        float legWidth_ = 90, legHeight_ = 50;
        bool ifSmooth_ = false;
        float targetX_ = 320, targetY_ = 200;
        float smoothFactor_ = 0.1f;
    } sans_;

    void setSansPosition(float x, float y, bool ifSmooth, float factor = 0.1f);
    void updateSansAnimation(float t);
    void drawSans(sf::RenderWindow& window);

    // ========== Box ==========
    struct BoxConfig {
        float x = 320.0f, y = 310.0f;
        float left = 283.0f, right = 283.0f, up = 65.0f, down = 65.0f;
        float angle = 0.0f;
        float frameThickness = 5.0f;
        
        bool ifSmooth_ = true;
        float smoothFactor_ = 0.1f;
        
        float targetX = 320.0f, targetY = 240.0f;
        float targetLeft = 65.0f, targetRight = 65.0f;
        float targetUp = 65.0f, targetDown = 65.0f;
        float targetAngle = 0.0f;

        float width() const  { return left + right; }
        float height() const { return up + down; }
        
        // 相对中心的顶点（未旋转）
        sf::Vector2f tl() const { return {-left, -up}; }
        sf::Vector2f tr() const { return { right, -up}; }
        sf::Vector2f br() const { return { right,  down}; }
        sf::Vector2f bl() const { return {-left,  down}; }
    } box_;

    void setBoxPosition(float cx, float cy, bool ifSmooth = true);
    void setBoxSize(float left, float right, float up, float down, bool ifSmooth = true);
    void setBoxAngle(float angle, bool ifSmooth = true);
    void updateBox();
    void drawBox(sf::RenderWindow& window);

    // ========== Soul ==========
    const float ACC_GRAVITY = 0.1f;
    const float PI = 3.14159265359f;
    struct SoulConfig {
        bool ifDisplay_ = true;
        float x_ = 320.0f, y_ = 240.0f;

        float dir_ = 0;
        float targetDit_ = 0;
        bool rotationIfSmooth = false;
        float rotationSmoothFactor = 0.1f;

        int status_ = 1;

        float moveSpeed_ = 3.0f;
        float velocity_ = 0;
        float horizSpeed_ = 0.0f;   // 垂直于重力方向的速度
        float jumpSpeed_ = 4.0f;    // 起跳初速度（起跳范围）
        bool wasJumpPressed_ = false;   // 上一帧是否按住了跳跃键
        bool isJumping_ = false;        // 是否处于跳跃状态（上升阶段）
        bool ifPlaySfx = true;     //落地音效播放
        sf::Angle angle_(float d) { return sf::degrees(-d); }
    } soul_;

    void setSoulPosition(float x, float y, bool ifSmooth = false);
    void setSoulDir(float dir, bool ifSmooth = true, float factor = 0.5f);
    void updateSoul();
    void drawSoul(sf::RenderWindow& window);
};