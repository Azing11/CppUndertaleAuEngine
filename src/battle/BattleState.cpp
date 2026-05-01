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
    , background_(assets_.getTexture(Res::uiTexture::BACKGROUND))
    , sansHeadSprite_(assets.getTexture(Res::sansTexture::SANS_HEAD_IDLE))
    , sansBodySprite_(assets.getTexture(Res::sansTexture::SANS_BODY_IDLE))
    , sansLegSprite_(assets.getTexture(Res::sansTexture::SANS_LEG))
    , soulSprite_(assets.getTexture(Res::soulTexture::SOUL_))
    , soulLightSprite_(assets.getTexture(Res::soulTexture::SOUL_LIGHT))
    , boxFrameSprite_(assets.getTexture(Res::uiTexture::PIXEL))   // 1x1 白色像素
    , boxBgSprite_(assets.getTexture(Res::uiTexture::BOX_BG))    
    , sfx_dong_(assets_.getSound(Res::sfx::SFX_DONG))
    , menu_font_(assets_.getFont(Res::fonts::MENU_FONT))
    , sans_font_(assets_.getFont(Res::fonts::SANS_FONT))
    , text_(assets_.getFont(Res::fonts::STATUS_BAR))
    , krSprite_(assets_.getTexture(Res::uiTexture::KR))
    , hpSprite_(assets_.getTexture(Res::uiTexture::HP))
{
    //背景
    auto bg = background_.getLocalBounds();
    background_.setOrigin({bg.size.x / 2, bg.size.y / 2});
    background_.setScale({1.0f, 0.5f});
    background_.setColor(sf::Color::White);
    background_.setPosition(sf::Vector2f(320.0f, 240.0f));

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
    soulLightSprite_.setScale({0.05f, 0.05f});

    // Box 边框 - 白色，只画线框（通过顶点控制）
    boxFrameSprite_.setColor(sf::Color::White);

    // Box 背景
    boxBgSprite_.setColor(sf::Color(255, 255, 255, 200));
    
    auto maxHpRectBounds = maxHpRect.getLocalBounds();
    maxHpRect.setOrigin({maxHpRectBounds.size.x / 2, maxHpRectBounds.size.y / 2});
    maxHpRect.setPosition({260, 397});
    maxHpRect.setFillColor(sf::Color::Red);
    maxHpRect.setSize(sf::Vector2f(1.0f, 1.0f));

    auto currentHpRectBounds = currentHpRect.getLocalBounds();
    currentHpRect.setOrigin({currentHpRectBounds.size.x / 2, currentHpRectBounds.size.y / 2});
    currentHpRect.setPosition({260, 397});
    currentHpRect.setFillColor(sf::Color::Yellow);
    currentHpRect.setSize(sf::Vector2f(1.0f, 1.0f));
}

void BattleState::enter() {

    battleClock.restart();

    initStatusBar();
    if (!music_.openFromFile(Res::audio::BATTLE_MUSIC)) { std::cerr << "Fail to load music\n"; }

    // 中心 (320, 240)
    setBoxPosition(320.0f, 320.0f, false);
    setBoxSize(283.0f, 283.0f, 65.0f, 65.0f, false); //左右上下
    
    //setBoxAngle(90.0f, true);
    //setBoxSize(65.0f, 65.0f, 65.0f, 65.0f, true);

    // Sans
    setSansPosition(320.0f, 320.0f - 65.0f - 40.0f, false);

    // Soul
    setSoulPosition(320.0f, 310.0f, false);
    setSoulDir(0.0f);
}

void BattleState::handleEvent(const sf::Event& event) {}

void BattleState::update(float dt) {
    float t = battleClock.getElapsedTime().asMilliseconds();
    updateSansAnimation(t);
    updateBox();
    updateSoul();
    updateStatusBar();
}

void BattleState::render(sf::RenderWindow& window) {
    window.draw(background_);
    drawSans(window);
    drawBox(window);
    drawSoul(window);
    drawStatusBar(window);
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

        box_.targetX = cx;
        box_.targetY = cy;
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

        box_.targetLeft = left;
        box_.targetRight = right;
        box_.targetUp = up;
        box_.targetDown = down;
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
    float ft = -box_.frameThickness;
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

void BattleState::setSoulDir(float dir, bool ifSmooth, float factor) {
    if (ifSmooth){
        soul_.rotationIfSmooth = true;
        soul_.rotationSmoothFactor = factor;
        soul_.targetDit_ = dir;
    }
    else{
        soul_.rotationIfSmooth = false;
        soul_.dir_ = dir;
    }
}

void BattleState::updateSoul() {
   
    // 碰撞
    auto soulBounds = soulSprite_.getLocalBounds();
    float soulRadius = std::max(
        (soulBounds.size.x * soulSprite_.getScale().x) / 2.0f,
        (soulBounds.size.y * soulSprite_.getScale().y) / 2.0f
    );

    // 移动处理
    {
        float dx = 0, dy = 0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    dy -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  dy += 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  dx -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dx += 1.0f;

        // 预检测边界，清零被阻挡方向的输入
        sf::Vector2f tl, tr, bl, br;
        getInnerEdges(tl, tr, bl, br);

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
            float midX = (a.x + b.x) / 2, midY = (a.y + b.y) / 2;
            float toCenterX = box_.x - midX, toCenterY = box_.y - midY;
            if (nx * toCenterX + ny * toCenterY < 0) { nx = -nx; ny = -ny; }
            return std::make_pair(nx, ny);
        };

        auto [nTopX, nTopY]       = getNormal(tl, tr);
        auto [nBottomX, nBottomY] = getNormal(br, bl);
        auto [nLeftX, nLeftY]     = getNormal(bl, tl);
        auto [nRightX, nRightY]   = getNormal(tr, br);

        // ========== 地面检测（基于重力方向） ==========
        float gravRad = soul_.dir_ * PI / 180.0f;
        float gravX = std::sin(gravRad);
        float gravY = std::cos(gravRad);
        float groundNX = -gravX;
        float groundNY = -gravY;

        struct EdgeInfo {
            float dist;
            float nX, nY;
        };
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

        // ========== 预阻挡 ==========
        switch (soul_.status_)
        {
        case 0:
            if (dTop < margin && dy < 0) {
                if (nTopY < -0.1f) dy = 0;
            }
            if (dBottom < margin && dy > 0) {
                if (nBottomY > 0.1f) dy = 0;
            }
            if (dLeft < margin && dx < 0) {
                if (nLeftX < -0.1f) dx = 0;
            }
            if (dRight < margin && dx > 0) {
                if (nRightX > 0.1f) dx = 0;
            }
            break;

        case 1:
            // case 1 的键盘预阻挡：只阻挡与重力方向垂直的移动（左右）
            // 上下键用于跳跃/下落控制，不预阻挡
            if (dLeft < margin && dx < 0) {
                if (nLeftX < -0.1f) dx = 0;
            }
            if (dRight < margin && dx > 0) {
                if (nRightX > 0.1f) dx = 0;
            }
            break;

        default:
            break;
        }

        // ========== 移动 ==========
        float length = std::sqrt(dx * dx + dy * dy);
        
        switch (soul_.status_)
        {
        case 0:// 红心模式

            if (length > 0) {
                soul_.x_ += (dx / length) * soul_.moveSpeed_;
                soul_.y_ += (dy / length) * soul_.moveSpeed_;
            }
            break;

        case 1: {// ========== 重力模式 ==========
            
            
            // ---- 水平移动（垂直于重力方向）----
            float perpX = -gravY;
            float perpY = gravX;
            
            float moveInput = dx * perpX + dy * perpY;
            
            float targetHorizSpeed = moveInput * soul_.moveSpeed_;
            float horizAccel = onGround ? 0.3f : 0.15f;
            soul_.horizSpeed_ += (targetHorizSpeed - soul_.horizSpeed_) * horizAccel;
            
            soul_.x_ += soul_.horizSpeed_ * perpX;
            soul_.y_ += soul_.horizSpeed_ * perpY;

            // ---- 跳跃触发 ----
            float jumpInput = -(dx * gravX + dy * gravY);  // 沿重力反方向的输入
            
            // 检测跳跃键是否刚刚按下
            bool jumpPressed = jumpInput > 0.5f;
            bool jumpJustPressed = jumpPressed && !soul_.wasJumpPressed_;
            
            if (onGround && jumpJustPressed) {
                // 起跳，给向上的初速度
                soul_.velocity_ = -soul_.jumpSpeed_;
                soul_.isJumping_ = true;  // 标记正在跳跃中
            }
            
            soul_.wasJumpPressed_ = jumpPressed;  // 记录本次状态

            // ---- 松开跳跃键 ----
            if (soul_.isJumping_ && !jumpPressed && soul_.velocity_ < 0) {
                // velocity_ < 0 表示仍在上升（反重力方向）
                // 截断上升速度，让角色开始下落
                soul_.velocity_ *= 0.3f;  // 大幅衰减，也可直接 = 0
            }

            // ---- 重力更新 ----
            if (onGround && soul_.velocity_ >= 0) {
                // 落地
                soul_.velocity_ = 0;
                if(soul_.ifPlaySfx ){ sfx_dong_.play(); soul_.ifPlaySfx = false; }
                soul_.isJumping_ = false;  // 跳跃结束
                
            } else {
                // 空中
                soul_.velocity_ += ACC_GRAVITY;
            }

            // ---- 垂直位移 ----
            soul_.x_ += soul_.velocity_ * gravX;
            soul_.y_ += soul_.velocity_ * gravY;
            
            break;
        }

        default:
            break;
        }

        // 计算距离（移动后）
        dTop    = distToSegment(soul_.x_, soul_.y_, tl.x, tl.y, tr.x, tr.y);
        dBottom = distToSegment(soul_.x_, soul_.y_, br.x, br.y, bl.x, bl.y);
        dLeft   = distToSegment(soul_.x_, soul_.y_, bl.x, bl.y, tl.x, tl.y);
        dRight  = distToSegment(soul_.x_, soul_.y_, tr.x, tr.y, br.x, br.y);

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

        // 收集所有需要推回的边
        float pushX = 0, pushY = 0;
        int pushCount = 0;

        auto addPush = [&](float dist, const sf::Vector2f& a, const sf::Vector2f& b, float nx, float ny) {
            if (dist < soulRadius) {
                sf::Vector2f cp = closestOnSegment(soul_.x_, soul_.y_, a.x, a.y, b.x, b.y);
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
            soul_.x_ += pushX;
            soul_.y_ += pushY;
        }

        // 推回后再次确认地面状态
        if (onGround && soul_.status_ == 1 && soul_.velocity_ > 0) {
            soul_.velocity_ = 0;
        }
    }
    

    // 绘制颜色
    switch (soul_.status_) {
        case 0:
            soulSprite_.setColor(sf::Color(255, 0, 0, 255));
            soulLightSprite_.setColor(sf::Color(255, 0, 0, 200));
            break;
        case 1:
            soulSprite_.setColor(sf::Color(0, 0, 255, 255));
            soulLightSprite_.setColor(sf::Color(0, 0, 255, 200));
            break;

        default:
            break;
    }

    if (soul_.rotationIfSmooth)
    {
        soul_.dir_ += (soul_.targetDit_ - soul_.dir_) * soul_.rotationSmoothFactor;
        soulSprite_.setRotation(soul_.angle_(soul_.dir_));
    }
    else{
        soulSprite_.setRotation(soul_.angle_(soul_.dir_));
    }

    soulSprite_.setPosition({soul_.x_, soul_.y_});
    soulLightSprite_.setPosition({soul_.x_, soul_.y_});
}

void BattleState::drawSoul(sf::RenderWindow& window) {
    if (soul_.ifDisplay_) {
        window.draw(soulLightSprite_);
        window.draw(soulSprite_);
    }
}

// ======== StatusBar ========

void BattleState::initStatusBar(){
    frisk_.setName("FRISK");
    frisk_.setLv(1);

    text_.setCharacterSize(26);
    text_.setFillColor(sf::Color::White);
    text_.setPosition({35, 390});
    text_.setLineSpacing(1.0f);
    auto pos = text_.getPosition();
    text_.setPosition({std::round(pos.x), std::round(pos.y)});

    hpSprite_.setPosition({220, 402});
    hpSprite_.setColor(sf::Color::White);
    hpSprite_.setScale(sf::Vector2f(1.2f, 1.2f));

    
    krSprite_.setColor(sf::Color::White);
    krSprite_.setScale(sf::Vector2f(1.2f, 1.2f));

    frisk_.setCurrentHp(frisk_.getMaxHp());
    maxHpRect.setScale(sf::Vector2f(1.2f * frisk_.getMaxHp(), 20.0f));
}

void BattleState::updateStatusBar(){
    status_bar_vHp += (frisk_.getCurrentHp() - status_bar_vHp) * status_bar_smooth_factor;
    currentHpRect.setScale(sf::Vector2f(1.2f * status_bar_vHp, 20.0f));

    krSprite_.setPosition({270 + frisk_.getMaxHp() * 1.2f, 402});
    std::string displayText = frisk_.getName() + "  LV " + std::to_string(frisk_.getLv()) + spaces(frisk_.getLv()/3 + 15) + std::to_string(frisk_.getCurrentHp()) + " / " +std::to_string(frisk_.getMaxHp());
    text_.setString(displayText);
}

void BattleState::drawStatusBar(sf::RenderWindow& window){
    window.draw(text_);
    window.draw(hpSprite_);
    window.draw(maxHpRect);
    window.draw(currentHpRect);
    window.draw(krSprite_);
}