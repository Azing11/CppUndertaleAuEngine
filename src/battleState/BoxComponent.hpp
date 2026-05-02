#pragma once
#include "BattleUtils.hpp"
#include "core/AssetManager.hpp"
#include "core/Resources.hpp"
#include "shader/DeformableSprite.hpp"
#include <SFML/Graphics.hpp>

class BoxComponent {
public:
    BoxComponent(AssetManager& assets);

    void setPosition(float cx, float cy, bool smooth = true);
    void setSize(float left, float right, float up, float down, bool smooth = true);
    void setAngle(float angle, bool smooth = true);
    void update();
    void draw(sf::RenderWindow& window);

    void getInnerEdges(sf::Vector2f& tl, sf::Vector2f& tr,
                       sf::Vector2f& bl, sf::Vector2f& br) const;

    float getX() const { return x; }
    float getY() const { return y; }
    float getAngle() const { return angle; }

private:
    DeformableSprite frameSprite_;
    DeformableSprite bgSprite_;

    float x = 320.0f, y = 310.0f;
    float left = 283.0f, right = 283.0f, up = 65.0f, down = 65.0f;
    float angle = 0.0f;
    float frameThickness = 5.0f;

    bool smooth_ = true;
    float smoothFactor_ = 0.1f;

    float targetX = 320.0f, targetY = 240.0f;
    float targetLeft = 65.0f, targetRight = 65.0f;
    float targetUp = 65.0f, targetDown = 65.0f;
    float targetAngle = 0.0f;

    float width() const  { return left + right; }
    float height() const { return up + down; }

    sf::Vector2f tl() const { return {-left, -up}; }
    sf::Vector2f tr() const { return { right, -up}; }
    sf::Vector2f br() const { return { right,  down}; }
    sf::Vector2f bl() const { return {-left,  down}; }
};

inline BoxComponent::BoxComponent(AssetManager& assets)
    : frameSprite_(assets.getTexture(Res::uiTexture::PIXEL))
    , bgSprite_(assets.getTexture(Res::uiTexture::BOX_BG))
{
    frameSprite_.setColor(sf::Color::White);
    bgSprite_.setColor(sf::Color(255, 255, 255, 200));
}

inline void BoxComponent::setPosition(float cx, float cy, bool smooth) {
    if (smooth) {
        smooth_ = true;
        targetX = cx;
        targetY = cy;
    } else {
        smooth_ = false;
        x = cx; y = cy;
        targetX = cx; targetY = cy;
    }
}

inline void BoxComponent::setSize(float l, float r, float u, float d, bool smooth) {
    if (smooth) {
        smooth_ = true;
        targetLeft = l; targetRight = r;
        targetUp = u; targetDown = d;
    } else {
        smooth_ = false;
        left = l; right = r; up = u; down = d;
        targetLeft = l; targetRight = r;
        targetUp = u; targetDown = d;
    }
}

inline void BoxComponent::setAngle(float a, bool smooth) {
    if (smooth) {
        smooth_ = true;
        targetAngle = a;
    } else {
        smooth_ = false;
        angle = a;
    }
}

inline void BoxComponent::update() {
    if (smooth_) {
        x += (targetX - x) * smoothFactor_;
        y += (targetY - y) * smoothFactor_;
        left   += (targetLeft   - left)   * smoothFactor_;
        right  += (targetRight  - right)  * smoothFactor_;
        up     += (targetUp     - up)     * smoothFactor_;
        down   += (targetDown   - down)   * smoothFactor_;
        angle  = BattleUtils::angleLerp(angle, targetAngle, smoothFactor_);
    }

    using BattleUtils::rotatePoint;

    sf::Vector2f otl = rotatePoint(x + tl().x, y + tl().y, x, y, angle);
    sf::Vector2f otr = rotatePoint(x + tr().x, y + tr().y, x, y, angle);
    sf::Vector2f obr = rotatePoint(x + br().x, y + br().y, x, y, angle);
    sf::Vector2f obl = rotatePoint(x + bl().x, y + bl().y, x, y, angle);

    float ft = -frameThickness;
    sf::Vector2f itl = rotatePoint(x + tl().x + ft, y + tl().y + ft, x, y, angle);
    sf::Vector2f itr = rotatePoint(x + tr().x - ft, y + tr().y + ft, x, y, angle);
    sf::Vector2f ibr = rotatePoint(x + br().x - ft, y + br().y - ft, x, y, angle);
    sf::Vector2f ibl = rotatePoint(x + bl().x + ft, y + bl().y - ft, x, y, angle);

    frameSprite_.setCorners(otl, otr, obl, obr);
    bgSprite_.setCorners(itl, itr, ibl, ibr);
}

inline void BoxComponent::draw(sf::RenderWindow& window) {
    bgSprite_.draw(window);

    using BattleUtils::rotatePoint;

    sf::Vector2f otl = rotatePoint(x + tl().x, y + tl().y, x, y, angle);
    sf::Vector2f otr = rotatePoint(x + tr().x, y + tr().y, x, y, angle);
    sf::Vector2f obr = rotatePoint(x + br().x, y + br().y, x, y, angle);
    sf::Vector2f obl = rotatePoint(x + bl().x, y + bl().y, x, y, angle);

    float thick = frameThickness;

    auto drawBorderLine = [&](const sf::Vector2f& a, const sf::Vector2f& b) {
        sf::RectangleShape line;
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float len = std::sqrt(dx * dx + dy * dy);
        float ang = std::atan2(dy, dx) * 180.0f / 3.14159265f;

        line.setSize({len + thick, thick});
        line.setOrigin({thick / 2, thick / 2});
        line.setPosition(a);
        line.setRotation(sf::degrees(ang));
        line.setFillColor(sf::Color::White);
        window.draw(line);
    };

    drawBorderLine(otl, otr);
    drawBorderLine(otr, obr);
    drawBorderLine(obr, obl);
    drawBorderLine(obl, otl);
}

inline void BoxComponent::getInnerEdges(sf::Vector2f& tl, sf::Vector2f& tr,
                                        sf::Vector2f& bl, sf::Vector2f& br) const {
    float ft = frameThickness;
    using BattleUtils::rotatePoint;
    tl = rotatePoint(x + this->tl().x + ft, y + this->tl().y + ft, x, y, angle);
    tr = rotatePoint(x + this->tr().x - ft, y + this->tr().y + ft, x, y, angle);
    br = rotatePoint(x + this->br().x - ft, y + this->br().y - ft, x, y, angle);
    bl = rotatePoint(x + this->bl().x + ft, y + this->bl().y - ft, x, y, angle);
}
