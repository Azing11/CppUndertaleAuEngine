#pragma once
#include "BattleUtils.hpp"
#include "BoxComponent.hpp"
#include "core/AssetManager.hpp"
#include "core/Resources.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

class SoulComponent {
public:
    enum class Mode : int8_t {
        Menu = -1,      // 菜单状态，不更新物理
        Normal = 0,     // 普通四向移动
        Gravity = 1     // 重力平台跳跃
    };

    SoulComponent(AssetManager& assets);

    void setPosition(float x, float y, bool smooth = false, float factor = 0.5f);
    void setDir(float dir, bool smooth = true, float factor = 0.5f);
    void update(const BoxComponent& box);
    void draw(sf::RenderWindow& window);

    void setMode(Mode m) { mode_ = m; }
    void setDisplayable(bool display) { display_ = display; }

    Mode getMode() const { return mode_; }
    float getX() const { return x_; }
    float getY() const { return y_; }

private:
    sf::Sprite sprite_;
    sf::Sprite lightSprite_;
    sf::Sound sfxDong_;

    bool display_ = true;
    float x_ = 320.0f, y_ = 240.0f;
    bool smoothPosition_ = true;
    float positionFactor_ = 0;
    float targetX_ = 0;
    float targetY_ = 0;

    float dir_ = 0.0f;              // 重力方向，角度制，0 = 向下
    float targetDir_ = 0.0f;
    bool smoothRotation_ = false;
    float rotationFactor_ = 0.1f;

    Mode mode_ = Mode::Normal;

    float moveSpeed_ = 2.0f;
    float velocity_ = 0.0f;         // 重力模式下的垂直速度
    float horizSpeed_ = 0.0f;       // 重力模式下的水平速度
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

inline void SoulComponent::setPosition(float x, float y, bool smooth, float factor) {
    if (smooth) {
        targetX_ = x;
        targetY_ = y;
        smoothPosition_ = true;
        positionFactor_ = factor;
    } else {
        x_ = x;
        y_ = y;
        smoothPosition_ = false;
    }
}

inline void SoulComponent::setDir(float dir, bool smooth, float factor) {
    if (smooth) {
        smoothRotation_ = true;
        rotationFactor_ = factor;
        targetDir_ = dir;
    } else {
        smoothRotation_ = false;
        dir_ = dir;
    }
}

inline void SoulComponent::update(const BoxComponent& box) {
    using BattleUtils::distToSegment;

    auto bounds = sprite_.getLocalBounds();
    float soulRadius = std::max(
        (bounds.size.x * sprite_.getScale().x) / 2.0f,
        (bounds.size.y * sprite_.getScale().y) / 2.0f
    );

    if (smoothRotation_) {
        dir_ += (targetDir_ - dir_) * rotationFactor_;
    }

    if (smoothPosition_) {
        x_ += (targetX_ - x_) * positionFactor_;
        y_ += (targetY_ - y_) * positionFactor_;
    }

    if (mode_ == Mode::Menu) {
        // 菜单模式：只更新位置和旋转，不做物理/绘制颜色处理
        sprite_.setPosition({x_, y_});
        lightSprite_.setPosition({x_, y_});
        sprite_.setRotation(toAngle(dir_));
        return;
    }

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

    switch (mode_) {
        case Mode::Normal:
            if (dTop < margin && dy < 0)    { if (nTopY < -0.1f) dy = 0; }
            if (dBottom < margin && dy > 0) { if (nBottomY > 0.1f) dy = 0; }
            if (dLeft < margin && dx < 0)   { if (nLeftX < -0.1f) dx = 0; }
            if (dRight < margin && dx > 0)  { if (nRightX > 0.1f) dx = 0; }
            break;

        case Mode::Gravity:
            if (dLeft < margin && dx < 0)   { if (nLeftX < -0.1f) dx = 0; }
            if (dRight < margin && dx > 0)  { if (nRightX > 0.1f) dx = 0; }
            break;

        case Mode::Menu:
            break; // 前面已处理，这里不会走到
    }

    float length = std::sqrt(dx * dx + dy * dy);

    switch (mode_) {
        case Mode::Normal:
            if (length > 0) {
                x_ += (dx / length) * moveSpeed_;
                y_ += (dy / length) * moveSpeed_;
            }
            break;

        case Mode::Gravity: {
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

        case Mode::Menu:
            break;
    }

    // 重新计算距离用于碰撞推开
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
        x_ += pushX / pushCount;
        y_ += pushY / pushCount;
    }

    if (onGround && mode_ == Mode::Gravity && velocity_ > 0) {
        velocity_ = 0;
    }

    switch (mode_) {
        case Mode::Normal:
            sprite_.setColor(sf::Color(255, 0, 0, 255));
            lightSprite_.setColor(sf::Color(255, 0, 0, 200));
            break;
        case Mode::Gravity:
            sprite_.setColor(sf::Color(0, 0, 255, 255));
            lightSprite_.setColor(sf::Color(0, 0, 255, 200));
            break;
        case Mode::Menu:
            sprite_.setColor(sf::Color(255, 0, 0, 255));
            lightSprite_.setColor(sf::Color(255, 0, 0, 200));
            break;
    }

    sprite_.setPosition({x_, y_});
    lightSprite_.setPosition({x_, y_});
    sprite_.setRotation(toAngle(dir_));
}

inline void SoulComponent::draw(sf::RenderWindow& window) {
    if (display_) {
        window.draw(lightSprite_);
        window.draw(sprite_);
    }
}