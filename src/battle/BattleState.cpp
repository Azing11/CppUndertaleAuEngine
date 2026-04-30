#include "BattleState.hpp"

BattleState::BattleState(AssetManager& assets) 
    : assets_(assets)
    , sansHeadSprite_(assets.getTexture(Res::sansTexture::SANS_HEAD_IDLE))
    , sansBodySprite_(assets.getTexture(Res::sansTexture::SANS_BODY_IDLE))
    , sansLegSprite_(assets.getTexture(Res::sansTexture::SANS_LEG))
    , soulSprite_(assets.getTexture(Res::soulTexture::SOUL_))
    , soulLightSprite_(assets.getTexture(Res::soulTexture::SOUL_LIGHT))
    , boxSprite_({0.f, 0.f})
{
    auto headBounds = sansHeadSprite_.getLocalBounds();
    sansHeadSprite_.setOrigin({headBounds.size.x / 2, headBounds.size.y / 1.5f});
    sansHeadSprite_.setScale({0.5f, 0.5f});

    auto bodyBounds = sansBodySprite_.getLocalBounds();
    sansBodySprite_.setOrigin({bodyBounds.size.x / 2, bodyBounds.size.y / 1.5f});
    sansBodySprite_.setScale({0.5f, 0.5f});

    auto soulBounds = soulSprite_.getLocalBounds();
    soulSprite_.setOrigin({soulBounds.size.x / 2, soulBounds.size.y / 2});
    soulSprite_.setScale({1.0f, 1.0f});

    auto soulLightBounds = soulLightSprite_.getLocalBounds();
    soulLightSprite_.setOrigin({soulLightBounds.size.x / 2, soulLightBounds.size.y / 2});
    soulLightSprite_.setScale({0.07f, 0.07f});

    boxSprite_.setFillColor(sf::Color(0, 0, 0, 0));
    boxSprite_.setOutlineColor(sf::Color::White);
    boxSprite_.setOutlineThickness(6.f);
}

void BattleState::enter() {
    battleClock.restart();

    if (!music_.openFromFile(Res::Audio::BATTLE_MUSIC)) {
        std::cerr << "Fail to load music\n";
    }

    setBoxPosition(30, 220, true);
    setBoxSize(580, 160, true);

    setSansPosition(320, 200, true);
    setSoulPosition(320, 250, false);
}

void BattleState::handleEvent(const sf::Event& event) {}

void BattleState::update(float dt) {
    float t = battleClock.getElapsedTime().asMilliseconds();
    updateSansAnimation(t);
    updateBox();
    updateSoul();
}

void BattleState::render(sf::RenderWindow& window) {
    drawSans(window);
    drawBox(window);
    drawSoul(window);
}

// ========== Sans ==========

void BattleState::setSansPosition(float x, float y, bool ifSmooth, float factor) {
    if (ifSmooth) {
        sans_.ifSmooth_ = true;
        sans_.smoothFactor_ = factor;
        sans_.targetX_ = x;
        sans_.targetY_ = y;
    } else {
        sans_.ifSmooth_ = false;
        sans_.baseX_ = x;
        sans_.baseY_ = y;
    }
}

void BattleState::updateSansAnimation(float t) {
    sans_.time_ = t;
    sans_.swayX_ = std::sin(sans_.time_ * sans_.swayScorpX_) * sans_.swayFreqX_;
    sans_.swayY_ = std::sin(sans_.time_ * sans_.swayScorpY_) * sans_.swayFreqY_;

    if (sans_.ifSmooth_) {
        sans_.baseX_ += (sans_.targetX_ - sans_.baseX_) * sans_.smoothFactor_;
        sans_.baseY_ += (sans_.targetY_ - sans_.baseY_) * sans_.smoothFactor_;
    }
}

void BattleState::drawSans(sf::RenderWindow& window) {
    float sx = sans_.baseX_ + sans_.swayX_;
    float sy = sans_.baseY_ + sans_.swayY_;

    sansHeadSprite_.setPosition({sx, sy});
    sansBodySprite_.setPosition({sx, sy});

    sansLegSprite_.setCorners(
        {sans_.legBaseX_() + sans_.swayX_, sans_.legBaseY_() + sans_.swayY_},
        {sans_.legBaseX_() + sans_.legWidth_ + sans_.swayX_, sans_.legBaseY_() + sans_.swayY_},
        {sans_.legBaseX_(), sans_.legBaseY_() + sans_.legHeight_},
        {sans_.legBaseX_() + sans_.legWidth_, sans_.legBaseY_() + sans_.legHeight_}
    );

    sansLegSprite_.draw(window);
    window.draw(sansBodySprite_);
    window.draw(sansHeadSprite_);
}

// ========== Box ==========

void BattleState::setBoxPosition(float x, float y, bool ifSmooth) {
    if (ifSmooth) {
        box_.ifSmooth_ = true;
        box_.targetX_ = x;
        box_.targetY_ = y;
    } else {
        box_.ifSmooth_ = false;
        box_.x_ = x;
        box_.y_ = y;
    }
}

void BattleState::setBoxSize(float width, float height, bool ifSmooth) {
    if (ifSmooth) {
        box_.ifSmooth_ = true;
        box_.targetWidth_ = width;
        box_.targetHeight_ = height;
    } else {
        box_.ifSmooth_ = false;
        box_.width_ = width;
        box_.height_ = height;
    }
}

void BattleState::updateBox() {
    if (box_.ifSmooth_) {
        box_.x_ += (box_.targetX_ - box_.x_) * box_.smoothFactor_;
        box_.y_ += (box_.targetY_ - box_.y_) * box_.smoothFactor_;
        box_.width_  += (box_.targetWidth_  - box_.width_)  * box_.smoothFactor_;
        box_.height_ += (box_.targetHeight_ - box_.height_) * box_.smoothFactor_;
    }
    else{

    }
    boxSprite_.setPosition(sf::Vector2f(box_.x_, box_.y_));
    boxSprite_.setSize(sf::Vector2f(box_.width_, box_.height_));
    // 更新边界
    box_.updateBounds(box_.x_, box_.y_, box_.width_, box_.height_, boxSprite_.getOutlineThickness());
}

void BattleState::drawBox(sf::RenderWindow& window) {
    window.draw(boxSprite_);
}

// ========== Soul ==========

void BattleState::setSoulPosition(float x, float y, bool ifSmooth) {
    soul_.x_ = x;
    soul_.y_ = y;
}

void BattleState::setSoulDir(float dir) {
    soul_.dir_ = dir;
}

void BattleState::updateSoul() {
    float dx = 0, dy = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    dy -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  dy += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  dx -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dx += 1.0f;

    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0) {
        soul_.x_ += (dx / length) * soul_.moveSpeed_;
        soul_.y_ += (dy / length) * soul_.moveSpeed_;
    }

    // 使用 BoxConfig 中预计算的边界
    auto soulBounds = soulSprite_.getLocalBounds();
    float soulHalfW = (soulBounds.size.x * soulSprite_.getScale().x) / 2.0f;
    float soulHalfH = (soulBounds.size.y * soulSprite_.getScale().y) / 2.0f;

    float minX = box_.left + soulHalfW;
    float maxX = box_.right - soulHalfW;
    float minY = box_.top + soulHalfH;
    float maxY = box_.bottom - soulHalfH;

    // 防止 Box 太小时崩溃
    if (maxX < minX) maxX = minX;
    if (maxY < minY) maxY = minY;

    soul_.x_ = std::max(minX, std::min(soul_.x_, maxX));
    soul_.y_ = std::max(minY, std::min(soul_.y_, maxY));

    soulSprite_.setPosition({soul_.x_, soul_.y_});
    soulLightSprite_.setPosition({soul_.x_, soul_.y_});

    switch (soul_.status_) {
        case 0: //Red
            soulSprite_.setColor(sf::Color(255, 0, 0, 255));
            soulLightSprite_.setColor(sf::Color(255, 0, 0, 200));
            break;
        default:
            break;
    }
}

void BattleState::drawSoul(sf::RenderWindow& window) {
    if (soul_.ifDisplay_) {
        window.draw(soulLightSprite_);
        window.draw(soulSprite_);
    }
}