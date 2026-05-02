#pragma once
#include "BattleUtils.hpp"
#include "BoxComponent.hpp"
#include "core/AssetManager.hpp"
#include "core/Resources.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

class SoulComponent {
public:
    SoulComponent(AssetManager& assets);

    void setPosition(float x, float y, bool smooth = false);
    void setDir(float dir, bool smooth = true, float factor = 0.5f);
    void update(const BoxComponent& box);
    void draw(sf::RenderWindow& window);

    void setStatus(int s) { status_ = s; }
    int getStatus() const { return status_; }

    float getX() const { return x_; }
    float getY() const { return y_; }

private:
    sf::Sprite sprite_;
    sf::Sprite lightSprite_;
    sf::Sound sfxDong_;

    bool display_ = true;
    float x_ = 320.0f, y_ = 240.0f;

    float dir_ = 0;
    float targetDir_ = 0;
    bool rotationSmooth_ = false;
    float rotationFactor_ = 0.1f;

    int status_ = 0;

    float moveSpeed_ = 2.0f;
    float velocity_ = 0;
    float horizSpeed_ = 0.0f;
    float jumpSpeed_ = 4.0f;
    bool wasJumpPressed_ = false;
    bool isJumping_ = false;
    bool playSfx_ = false;

    static constexpr float ACC_GRAVITY = 0.1f;
    static constexpr float PI = 3.14159265359f;

    sf::Angle toAngle(float d) { return sf::degrees(-d); }
};

inline SoulComponent::SoulComponent(AssetManager& assets)
    : sprite_(assets.getTexture(Res::soulTexture::SOUL))
    , lightSprite_(assets.getTexture(Res::soulTexture::SOUL_LIGHT))
    , sfxDong_(assets.getSound(Res::sfx::SFX_DONG))
{
    auto bounds = sprite_.getLocalBounds();
    sprite_.setOrigin({bounds.size.x / 2, bounds.size.y / 2});
    sprite_.setScale({1.0f, 1.0f});

    auto lightBounds = lightSprite_.getLocalBounds();
    lightSprite_.setOrigin({lightBounds.size.x / 2, lightBounds.size.y / 2});
    lightSprite_.setScale({0.05f, 0.05f});
}

inline void SoulComponent::setPosition(float x, float y, bool smooth) {
    x_ = x; y_ = y;
}

inline void SoulComponent::setDir(float dir, bool smooth, float factor) {
    if (smooth) {
        rotationSmooth_ = true;
        rotationFactor_ = factor;
        targetDir_ = dir;
    } else {
        rotationSmooth_ = false;
        dir_ = dir;
    }
}

inline void SoulComponent::update(const BoxComponent& box) {
    using BattleUtils::distToSegment;
    using BattleUtils::closestOnSegment;

    auto bounds = sprite_.getLocalBounds();
    float soulRadius = std::max(
        (bounds.size.x * sprite_.getScale().x) / 2.0f,
        (bounds.size.y * sprite_.getScale().y) / 2.0f
    );

    float dx = 0, dy = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    dy -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  dy += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  dx -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dx += 1.0f;

    sf::Vector2f tl, tr, bl, br;
    box.getInnerEdges(tl, tr, bl, br);

    float dTop    = distToSegment(x_, y_, tl.x, tl.y, tr.x, tr.y);
    float dBottom = distToSegment(x_, y_, br.x, br.y, bl.x, bl.y);
    float dLeft   = distToSegment(x_, y_, bl.x, bl.y, tl.x, tl.y);
    float dRight  = distToSegment(x_, y_, tr.x, tr.y, br.x, br.y);

    float margin = soulRadius + 0.5f;

    auto getNormal = [&](const sf::Vector2f& a, const sf::Vector2f& b) {
        float nx = -(b.y - a.y);
        float ny = (b.x - a.x);
        float len = std::sqrt(nx * nx + ny * ny);
        if (len > 0) { nx /= len; ny /= len; }
        float midX = (a.x + b.x) / 2, midY = (a.y + b.y) / 2;
        float toCenterX = box.getX() - midX, toCenterY = box.getY() - midY;
        if (nx * toCenterX + ny * toCenterY < 0) { nx = -nx; ny = -ny; }
        return std::make_pair(nx, ny);
    };

    auto [nTopX, nTopY]       = getNormal(tl, tr);
    auto [nBottomX, nBottomY] = getNormal(br, bl);
    auto [nLeftX, nLeftY]     = getNormal(bl, tl);
    auto [nRightX, nRightY]   = getNormal(tr, br);

    float gravRad = dir_ * PI / 180.0f;
    float gravX = std::sin(gravRad);
    float gravY = std::cos(gravRad);
    float groundNX = -gravX;
    float groundNY = -gravY;

    struct EdgeInfo { float dist; float nX, nY; };
    EdgeInfo edges[4] = {
        {dTop,    nTopX,    nTopY},
        {dBottom, nBottomX, nBottomY},
        {dLeft,   nLeftX,   nLeftY},
        {dRight,  nRightX,  nRightY}
    };

    float maxDot = -2.0f;
    int groundEdgeIdx = -1;
    for (int i = 0; i < 4; ++i) {
        float dot = edges[i].nX * groundNX + edges[i].nY * groundNY;
        if (dot > maxDot) {
            maxDot = dot;
            groundEdgeIdx = i;
        }
    }

    bool onGround = (groundEdgeIdx >= 0)
                    && (edges[groundEdgeIdx].dist < margin)
                    && (maxDot > 0.7f);

    switch (status_) {
        case 0:
            if (dTop < margin && dy < 0)    { if (nTopY < -0.1f) dy = 0; }
            if (dBottom < margin && dy > 0) { if (nBottomY > 0.1f) dy = 0; }
            if (dLeft < margin && dx < 0)   { if (nLeftX < -0.1f) dx = 0; }
            if (dRight < margin && dx > 0)  { if (nRightX > 0.1f) dx = 0; }
            break;
        case 1:
            if (dLeft < margin && dx < 0)   { if (nLeftX < -0.1f) dx = 0; }
            if (dRight < margin && dx > 0)  { if (nRightX > 0.1f) dx = 0; }
            break;
    }

    float length = std::sqrt(dx * dx + dy * dy);

    switch (status_) {
        case 0:
            if (length > 0) {
                x_ += (dx / length) * moveSpeed_;
                y_ += (dy / length) * moveSpeed_;
            }
            break;

        case 1: {
            float perpX = -gravY;
            float perpY = gravX;
            float moveInput = dx * perpX + dy * perpY;
            float targetHorizSpeed = moveInput * moveSpeed_;
            float horizAccel = onGround ? 0.3f : 0.15f;
            horizSpeed_ += (targetHorizSpeed - horizSpeed_) * horizAccel;
            x_ += horizSpeed_ * perpX;
            y_ += horizSpeed_ * perpY;

            float jumpInput = -(dx * gravX + dy * gravY);
            bool jumpPressed = jumpInput > 0.5f;
            bool jumpJustPressed = jumpPressed && !wasJumpPressed_;

            if (onGround && jumpJustPressed) {
                velocity_ = -jumpSpeed_;
                isJumping_ = true;
            }
            wasJumpPressed_ = jumpPressed;

            if (isJumping_ && !jumpPressed && velocity_ < 0) {
                velocity_ *= 0.3f;
            }

            if (onGround && velocity_ >= 0) {
                velocity_ = 0;
                if (playSfx_) { sfxDong_.play(); playSfx_ = false; }
                isJumping_ = false;
            } else {
                velocity_ += ACC_GRAVITY;
            }

            x_ += velocity_ * gravX;
            y_ += velocity_ * gravY;
            break;
        }
    }

    dTop    = distToSegment(x_, y_, tl.x, tl.y, tr.x, tr.y);
    dBottom = distToSegment(x_, y_, br.x, br.y, bl.x, bl.y);
    dLeft   = distToSegment(x_, y_, bl.x, bl.y, tl.x, tl.y);
    dRight  = distToSegment(x_, y_, tr.x, tr.y, br.x, br.y);

    edges[0].dist = dTop;    edges[0].nX = nTopX;    edges[0].nY = nTopY;
    edges[1].dist = dBottom; edges[1].nX = nBottomX; edges[1].nY = nBottomY;
    edges[2].dist = dLeft;   edges[2].nX = nLeftX;   edges[2].nY = nLeftY;
    edges[3].dist = dRight;  edges[3].nX = nRightX;  edges[3].nY = nRightY;

    maxDot = -2.0f;
    groundEdgeIdx = -1;
    for (int i = 0; i < 4; ++i) {
        float dot = edges[i].nX * groundNX + edges[i].nY * groundNY;
        if (dot > maxDot) {
            maxDot = dot;
            groundEdgeIdx = i;
        }
    }
    onGround = (groundEdgeIdx >= 0)
            && (edges[groundEdgeIdx].dist < margin)
            && (maxDot > 0.7f);

    float pushX = 0, pushY = 0;
    int pushCount = 0;

    auto addPush = [&](float dist, const sf::Vector2f& a, const sf::Vector2f& b, float nx, float ny) {
        if (dist < soulRadius) {
            float overlap = soulRadius - dist;
            pushX += nx * overlap;
            pushY += ny * overlap;
            pushCount++;
        }
    };

    addPush(dTop,    tl, tr, nTopX,    nTopY);
    addPush(dBottom, br, bl, nBottomX, nBottomY);
    addPush(dLeft,   bl, tl, nLeftX,   nLeftY);
    addPush(dRight,  tr, br, nRightX,  nRightY);

    if (pushCount > 0) {
        x_ += pushX;
        y_ += pushY;
    }

    if (onGround && status_ == 1 && velocity_ > 0) {
        velocity_ = 0;
    }

    switch (status_) {
        case 0:
            sprite_.setColor(sf::Color(255, 0, 0, 255));
            lightSprite_.setColor(sf::Color(255, 0, 0, 200));
            break;
        case 1:
            sprite_.setColor(sf::Color(0, 0, 255, 255));
            lightSprite_.setColor(sf::Color(0, 0, 255, 200));
            break;
    }

    if (rotationSmooth_) {
        dir_ += (targetDir_ - dir_) * rotationFactor_;
        sprite_.setRotation(toAngle(dir_));
    } else {
        sprite_.setRotation(toAngle(dir_));
    }

    sprite_.setPosition({x_, y_});
    lightSprite_.setPosition({x_, y_});
}

inline void SoulComponent::draw(sf::RenderWindow& window) {
    if (display_) {
        window.draw(lightSprite_);
        window.draw(sprite_);
    }
}
