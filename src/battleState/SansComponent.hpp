#pragma once
#include "BattleUtils.hpp"
#include "core/AssetManager.hpp"
#include "core/Resources.hpp"
#include "shader/DeformableSprite.hpp"
#include <SFML/Graphics.hpp>

class SansComponent {
public:
    enum class Part {
        Head = 1,
        Body = 2,
        Leg = 3
    };

    SansComponent(AssetManager& assets);

    void setPosition(float x, float y, bool smooth = false, float factor = 0.1f);
    
    // 按部位设置纹理
    void setTex(Part part, const sf::Texture& tex);
    // n: 1=头, 2=身体, 3=腿
    void setTex(int n, const sf::Texture& tex);
    
    void update(float t);
    void draw(sf::RenderWindow& window);

    float getX() const { return baseX_ + swayX_; }
    float getY() const { return baseY_ + swayY_; }

private:
    sf::Sprite headSprite_;
    sf::Sprite bodySprite_;
    DeformableSprite legSprite_;

    float time_ = 0;
    float swayX_ = 0, swayY_ = 0;
    float swayScorpX_ = 2.0f / 1000.0f;
    float swayScorpY_ = 4.0f / 1000.0f;
    float swayFreqX_ = 4, swayFreqY_ = 2;

    float baseX_ = 320, baseY_ = 200;
    float legOffsetX_ = -43.0f, legOffsetY_ = -57.0f;
    float legWidth_ = 90, legHeight_ = 50;

    bool smooth_ = false;
    float targetX_ = 320, targetY_ = 200;
    float smoothFactor_ = 0.1f;
};

inline SansComponent::SansComponent(AssetManager& assets)
    : headSprite_(assets.getTexture(Res::sansTexture::SANS_HEAD_IDLE))
    , bodySprite_(assets.getTexture(Res::sansTexture::SANS_BODY_IDLE))
    , legSprite_(assets.getTexture(Res::sansTexture::SANS_LEG))
{
    auto headBounds = headSprite_.getLocalBounds();
    headSprite_.setOrigin({headBounds.size.x / 2, headBounds.size.y / 1.5f});
    headSprite_.setScale({0.5f, 0.5f});

    auto bodyBounds = bodySprite_.getLocalBounds();
    bodySprite_.setOrigin({bodyBounds.size.x / 2, bodyBounds.size.y / 1.5f});
    bodySprite_.setScale({0.5f, 0.5f});
}

inline void SansComponent::setPosition(float x, float y, bool smooth, float factor) {
    if (smooth) {
        smooth_ = true;
        smoothFactor_ = factor;
        targetX_ = x;
        targetY_ = y;
    } else {
        smooth_ = false;
        baseX_ = x;
        baseY_ = y;
    }
}

inline void SansComponent::setTex(Part part, const sf::Texture& tex) {
    switch (part) {
        case Part::Head:
            headSprite_.setTexture(tex, true);  // true = 重置矩形
            {
                auto bounds = headSprite_.getLocalBounds();
                headSprite_.setOrigin({bounds.size.x / 2, bounds.size.y / 1.5f});
                headSprite_.setScale({0.5f, 0.5f});
            }
            break;
            
        case Part::Body:
            bodySprite_.setTexture(tex, true);
            {
                auto bounds = bodySprite_.getLocalBounds();
                bodySprite_.setOrigin({bounds.size.x / 2, bounds.size.y / 1.5f});
                bodySprite_.setScale({0.5f, 0.5f});
            }
            break;
            
        case Part::Leg:
            legSprite_.setTexture(tex);
            break;
    }
}

inline void SansComponent::setTex(int n, const sf::Texture& tex) {
    switch (n) {
        case 1: setTex(Part::Head, tex); break;
        case 2: setTex(Part::Body, tex); break;
        case 3: setTex(Part::Leg, tex); break;
        default:
            
            break;
    }
}

inline void SansComponent::update(float t) {
    time_ = t;
    swayX_ = std::sin(time_ * swayScorpX_) * swayFreqX_;
    swayY_ = std::sin(time_ * swayScorpY_) * swayFreqY_;

    if (smooth_) {
        baseX_ += (targetX_ - baseX_) * smoothFactor_;
        baseY_ += (targetY_ - baseY_) * smoothFactor_;
    }
}

inline void SansComponent::draw(sf::RenderWindow& window) {
    float sx = baseX_ + swayX_;
    float sy = baseY_ + swayY_;

    headSprite_.setPosition({sx, sy});
    bodySprite_.setPosition({sx, sy});

    float legBaseX = baseX_ + legOffsetX_;
    float legBaseY = baseY_ + legOffsetY_;

    legSprite_.setCorners(
        {legBaseX + swayX_, legBaseY + swayY_},
        {legBaseX + legWidth_ + swayX_, legBaseY + swayY_},
        {legBaseX, legBaseY + legHeight_},
        {legBaseX + legWidth_, legBaseY + legHeight_}
    );

    legSprite_.draw(window);
    window.draw(bodySprite_);
    window.draw(headSprite_);
}