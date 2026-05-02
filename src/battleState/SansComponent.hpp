#pragma once
#include "BattleUtils.hpp"
#include "core/AssetManager.hpp"
#include "core/Resources.hpp"
#include "shader/DeformableSprite.hpp"
#include <SFML/Graphics.hpp>

class SansComponent {
public:
    SansComponent(AssetManager& assets);

    void setPosition(float x, float y, bool smooth = false, float factor = 0.1f);
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
