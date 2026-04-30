#include "BattleState.hpp"

// ========== 工具函数 ==========

sf::Vector2f BattleState::rotatePoint(float px, float py, float cx, float cy, float angleDeg) {
    float rad = angleDeg * 3.14159265f / 180.0f;
    float c = std::cos(rad);
    float s = std::sin(rad);
    float dx = px - cx;
    float dy = py - cy;
    return { cx + dx * c - dy * s, cy + dx * s + dy * c };
}

float BattleState::angleLerp(float a, float b, float t) {
    float diff = b - a;
    while (diff > 180.0f) diff -= 360.0f;
    while (diff < -180.0f) diff += 360.0f;
    return a + diff * t;
}

float BattleState::distToSegment(float px, float py, float x1, float y1, float x2, float y2) {
    float dx = x2 - x1, dy = y2 - y1;
    float lenSq = dx * dx + dy * dy;
    if (lenSq == 0.0f) {
        dx = px - x1; dy = py - y1;
        return std::sqrt(dx * dx + dy * dy);
    }
    float t = std::max(0.0f, std::min(1.0f, ((px - x1) * dx + (py - y1) * dy) / lenSq));
    float cx = x1 + t * dx, cy = y1 + t * dy;
    dx = px - cx; dy = py - cy;
    return std::sqrt(dx * dx + dy * dy);
}

sf::Vector2f BattleState::closestOnSegment(float px, float py, float x1, float y1, float x2, float y2) {
    float dx = x2 - x1, dy = y2 - y1;
    float lenSq = dx * dx + dy * dy;
    if (lenSq == 0.0f) return {x1, y1};
    float t = std::max(0.0f, std::min(1.0f, ((px - x1) * dx + (py - y1) * dy) / lenSq));
    return {x1 + t * dx, y1 + t * dy};
}

// ========== 构造函数 ==========

BattleState::BattleState(AssetManager& assets) 
    : assets_(assets)
    , sansHeadSprite_(assets.getTexture(Res::sansTexture::SANS_HEAD_IDLE))
    , sansBodySprite_(assets.getTexture(Res::sansTexture::SANS_BODY_IDLE))
    , sansLegSprite_(assets.getTexture(Res::sansTexture::SANS_LEG))
    , soulSprite_(assets.getTexture(Res::soulTexture::SOUL_))
    , soulLightSprite_(assets.getTexture(Res::soulTexture::SOUL_LIGHT))
    , boxFrameSprite_(assets.getTexture(Res::uiTexture::PIXEL))   // 1x1 白色像素
    , boxBgSprite_(assets.getTexture(Res::uiTexture::BOX_BG))    
{
    // Sans
    auto headBounds = sansHeadSprite_.getLocalBounds();
    sansHeadSprite_.setOrigin({headBounds.size.x / 2, headBounds.size.y / 1.5f});
    sansHeadSprite_.setScale({0.5f, 0.5f});

    auto bodyBounds = sansBodySprite_.getLocalBounds();
    sansBodySprite_.setOrigin({bodyBounds.size.x / 2, bodyBounds.size.y / 1.5f});
    sansBodySprite_.setScale({0.5f, 0.5f});

    // Soul
    auto soulBounds = soulSprite_.getLocalBounds();
    soulSprite_.setOrigin({soulBounds.size.x / 2, soulBounds.size.y / 2});
    soulSprite_.setScale({1.0f, 1.0f});

    auto soulLightBounds = soulLightSprite_.getLocalBounds();
    soulLightSprite_.setOrigin({soulLightBounds.size.x / 2, soulLightBounds.size.y / 2});
    soulLightSprite_.setScale({0.07f, 0.07f});

    // Box 边框 - 白色，只画线框（通过顶点控制）
    boxFrameSprite_.setColor(sf::Color::White);

    // Box 背景
    boxBgSprite_.setColor(sf::Color(255, 255, 255, 200));
}

// ========== enter ==========

void BattleState::enter() {
    battleClock.restart();

    if (!music_.openFromFile(Res::Audio::BATTLE_MUSIC)) {
        std::cerr << "Fail to load music\n";
    }

    // 中心 (320, 240)
    setBoxPosition(320.0f, 320.0f, false);
    setBoxSize(283.0f, 283.0f, 65.0f, 65.0f, false); //左右上下
    //setBoxAngle(45.0f, true);

    // Sans 在 Box 上方
    setSansPosition(320.0f, 320.0f - 65.0f - 40.0f, false);

    // Soul 在 Box 中心
    setSoulPosition(320.0f, 310.0f, false);
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

void BattleState::setBoxPosition(float cx, float cy, bool ifSmooth) {
    if (ifSmooth) {
        box_.ifSmooth_ = true;
        box_.targetX = cx;
        box_.targetY = cy;
    } else {
        box_.ifSmooth_ = false;
        box_.x = cx;
        box_.y = cy;
    }
}

void BattleState::setBoxSize(float left, float right, float up, float down, bool ifSmooth) {
    if (ifSmooth) {
        box_.ifSmooth_ = true;
        box_.targetLeft = left;
        box_.targetRight = right;
        box_.targetUp = up;
        box_.targetDown = down;
    } else {
        box_.ifSmooth_ = false;
        box_.left = left;
        box_.right = right;
        box_.up = up;
        box_.down = down;
    }
}

void BattleState::setBoxAngle(float angle, bool ifSmooth) {
    if (ifSmooth) {
        box_.ifSmooth_ = true;
        box_.targetAngle = angle;
    } else {
        box_.ifSmooth_ = false;
        box_.angle = angle;
    }
}

void BattleState::updateBox() {
    if (box_.ifSmooth_) {
        box_.x += (box_.targetX - box_.x) * box_.smoothFactor_;
        box_.y += (box_.targetY - box_.y) * box_.smoothFactor_;
        box_.left   += (box_.targetLeft   - box_.left)   * box_.smoothFactor_;
        box_.right  += (box_.targetRight  - box_.right)  * box_.smoothFactor_;
        box_.up     += (box_.targetUp     - box_.up)     * box_.smoothFactor_;
        box_.down   += (box_.targetDown   - box_.down)   * box_.smoothFactor_;
        box_.angle  = angleLerp(box_.angle, box_.targetAngle, box_.smoothFactor_);
    }

    // 外框顶点
    sf::Vector2f otl = rotatePoint(box_.x + box_.tl().x, box_.y + box_.tl().y, box_.x, box_.y, box_.angle);
    sf::Vector2f otr = rotatePoint(box_.x + box_.tr().x, box_.y + box_.tr().y, box_.x, box_.y, box_.angle);
    sf::Vector2f obr = rotatePoint(box_.x + box_.br().x, box_.y + box_.br().y, box_.x, box_.y, box_.angle);
    sf::Vector2f obl = rotatePoint(box_.x + box_.bl().x, box_.y + box_.bl().y, box_.x, box_.y, box_.angle);

    // 内框顶点（减去边框厚度）
    float ft = box_.frameThickness;
    sf::Vector2f itl = rotatePoint(box_.x + box_.tl().x + ft, box_.y + box_.tl().y + ft, box_.x, box_.y, box_.angle);
    sf::Vector2f itr = rotatePoint(box_.x + box_.tr().x - ft, box_.y + box_.tr().y + ft, box_.x, box_.y, box_.angle);
    sf::Vector2f ibr = rotatePoint(box_.x + box_.br().x - ft, box_.y + box_.br().y - ft, box_.x, box_.y, box_.angle);
    sf::Vector2f ibl = rotatePoint(box_.x + box_.bl().x + ft, box_.y + box_.bl().y - ft, box_.x, box_.y, box_.angle);

    // setCorners 顺序：左上, 右上, 左下, 右下（TriangleStrip 顺序）
    boxFrameSprite_.setCorners(otl, otr, obl, obr);
    boxBgSprite_.setCorners(itl, itr, ibl, ibr);
}

void BattleState::drawBox(sf::RenderWindow& window) {
    // 先画背景
    boxBgSprite_.draw(window);

    // 获取外框顶点
    sf::Vector2f otl = rotatePoint(box_.x + box_.tl().x, box_.y + box_.tl().y, box_.x, box_.y, box_.angle);
    sf::Vector2f otr = rotatePoint(box_.x + box_.tr().x, box_.y + box_.tr().y, box_.x, box_.y, box_.angle);
    sf::Vector2f obr = rotatePoint(box_.x + box_.br().x, box_.y + box_.br().y, box_.x, box_.y, box_.angle);
    sf::Vector2f obl = rotatePoint(box_.x + box_.bl().x, box_.y + box_.bl().y, box_.x, box_.y, box_.angle);

    float thick = box_.frameThickness;

    // 辅助函数：画一条边框线段，两端各延长 thick/2 以覆盖角
    auto drawBorderLine = [&](const sf::Vector2f& a, const sf::Vector2f& b) {
        sf::RectangleShape line;
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float len = std::sqrt(dx * dx + dy * dy);
        float angle = std::atan2(dy, dx) * 180.0f / 3.14159265f;
        
        // 长度 + thick，让两端覆盖角
        line.setSize({len + thick, thick});
        line.setOrigin({thick / 2, thick / 2});  // 中心对齐，从 -thick/2 开始
        line.setPosition(a);
        line.setRotation(sf::degrees(angle));
        line.setFillColor(sf::Color::White);
        
        window.draw(line);
    };

    drawBorderLine(otl, otr);  // 上
    drawBorderLine(otr, obr);  // 右
    drawBorderLine(obr, obl);  // 下
    drawBorderLine(obl, otl);  // 左
}

void BattleState::getInnerEdges(sf::Vector2f& tl, sf::Vector2f& tr, 
                                sf::Vector2f& bl, sf::Vector2f& br) const {
    float ft = box_.frameThickness;
    tl = rotatePoint(box_.x + box_.tl().x + ft, box_.y + box_.tl().y + ft, box_.x, box_.y, box_.angle);
    tr = rotatePoint(box_.x + box_.tr().x - ft, box_.y + box_.tr().y + ft, box_.x, box_.y, box_.angle);
    br = rotatePoint(box_.x + box_.br().x - ft, box_.y + box_.br().y - ft, box_.x, box_.y, box_.angle);
    bl = rotatePoint(box_.x + box_.bl().x + ft, box_.y + box_.bl().y - ft, box_.x, box_.y, box_.angle);
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
    //移动
    {
        float dx = 0, dy = 0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    dy -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  dy += 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  dx -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dx += 1.0f;

        // 预检测边界，清零被阻挡方向的输入
        sf::Vector2f tl, tr, bl, br;
        getInnerEdges(tl, tr, bl, br);

        auto soulBounds = soulSprite_.getLocalBounds();
        float soulRadius = std::max(
            (soulBounds.size.x * soulSprite_.getScale().x) / 2.0f,
            (soulBounds.size.y * soulSprite_.getScale().y) / 2.0f
        );

        float dTop    = distToSegment(soul_.x_, soul_.y_, tl.x, tl.y, tr.x, tr.y);
        float dBottom = distToSegment(soul_.x_, soul_.y_, br.x, br.y, bl.x, bl.y);
        float dLeft   = distToSegment(soul_.x_, soul_.y_, bl.x, bl.y, tl.x, tl.y);
        float dRight  = distToSegment(soul_.x_, soul_.y_, tr.x, tr.y, br.x, br.y);

        float margin = soulRadius + 0.5f;

        // 计算每条边的法线方向（朝内）
        auto getNormal = [&](const sf::Vector2f& a, const sf::Vector2f& b) {
            float nx = -(b.y - a.y);
            float ny = (b.x - a.x);
            float len = std::sqrt(nx * nx + ny * ny);
            if (len > 0) { nx /= len; ny /= len; }
            // 确保朝内
            float midX = (a.x + b.x) / 2, midY = (a.y + b.y) / 2;
            float toCenterX = box_.x - midX, toCenterY = box_.y - midY;
            if (nx * toCenterX + ny * toCenterY < 0) { nx = -nx; ny = -ny; }
            return std::make_pair(nx, ny);
        };

        auto [nTopX, nTopY]     = getNormal(tl, tr);
        auto [nBottomX, nBottomY] = getNormal(br, bl);  // 注意顶点顺序
        auto [nLeftX, nLeftY]   = getNormal(bl, tl);
        auto [nRightX, nRightY] = getNormal(tr, br);

        // 预阻挡：如果移动方向有向外的分量，清零
        if (dTop < margin && dy < 0) {
            // 检查 ↑ 是否真的有向外的分量
            if (nTopY < -0.5f) dy = 0;  // 上边法线朝上，↑ 是向外
        }
        if (dBottom < margin && dy > 0) {
            if (nBottomY > 0.5f) dy = 0;
        }
        if (dLeft < margin && dx < 0) {
            if (nLeftX < -0.5f) dx = 0;
        }
        if (dRight < margin && dx > 0) {
            if (nRightX > 0.5f) dx = 0;
        }

        // 移动
        float length = std::sqrt(dx * dx + dy * dy);
        if (length > 0) {
            soul_.x_ += (dx / length) * soul_.moveSpeed_;
            soul_.y_ += (dy / length) * soul_.moveSpeed_;
        }

        // ========== 后处理：同时推回所有过近的边 ==========
        // 重新计算距离
        dTop    = distToSegment(soul_.x_, soul_.y_, tl.x, tl.y, tr.x, tr.y);
        dBottom = distToSegment(soul_.x_, soul_.y_, br.x, br.y, bl.x, bl.y);
        dLeft   = distToSegment(soul_.x_, soul_.y_, bl.x, bl.y, tl.x, tl.y);
        dRight  = distToSegment(soul_.x_, soul_.y_, tr.x, tr.y, br.x, br.y);

        // 收集所有需要推回的边
        float pushX = 0, pushY = 0;
        int pushCount = 0;

        auto addPush = [&](float dist, const sf::Vector2f& a, const sf::Vector2f& b, float nx, float ny) {
            if (dist < soulRadius) {
                sf::Vector2f cp = closestOnSegment(soul_.x_, soul_.y_, a.x, a.y, b.x, b.y);
                float overlap = soulRadius - dist;
                // 沿法线方向推回
                pushX += nx * overlap;
                pushY += ny * overlap;
                pushCount++;
            }
        };

        addPush(dTop, tl, tr, nTopX, nTopY);
        addPush(dBottom, br, bl, nBottomX, nBottomY);  // 注意顺序
        addPush(dLeft, bl, tl, nLeftX, nLeftY);
        addPush(dRight, tr, br, nRightX, nRightY);

        if (pushCount > 0) {
            // 平均推回量（防止多条边叠加过度）
            soul_.x_ += pushX / pushCount;
            soul_.y_ += pushY / pushCount;
        }

        soulSprite_.setPosition({soul_.x_, soul_.y_});
        soulLightSprite_.setPosition({soul_.x_, soul_.y_});
    }
    
    switch (soul_.status_) {
        case 0:
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