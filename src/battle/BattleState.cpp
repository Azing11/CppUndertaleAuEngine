//BattleState.cpp
#include "BattleState.hpp"

// 初始化列表中构造 sprite
BattleState::BattleState(AssetManager& assets) 
    : assets_(assets)
    , sansHeadSprite_(assets.getTexture(Res::sansTexture::SANS_HEAD_IDLE))  // 直接传 texture 构造
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

    //soul
    auto soulBounds = soulSprite_.getLocalBounds();
    soulSprite_.setOrigin({soulBounds.size.x / 2, soulBounds.size.y / 2});
    soulSprite_.setScale({1.0f, 1.0f});

    //soul light 
    auto soulLightBounds = soulLightSprite_.getLocalBounds();
    soulLightSprite_.setOrigin({soulLightBounds.size.x / 2, soulLightBounds.size.y / 2});
    soulLightSprite_.setScale({0.07f, 0.07f});

    //box
    boxSprite_.setFillColor(sf::Color(0, 0, 0, 0));// 设置填充颜色
    boxSprite_.setOutlineColor(sf::Color::White);// 设置边框颜色
    boxSprite_.setOutlineThickness(6.f);// 设置边框粗细

}

void BattleState::enter() {
    //std::cout << "Entering battle\n";
    battleClock.restart();

    //加载音乐
    if(!music_.openFromFile(Res::Audio::BATTLE_MUSIC))
    {
        std::cerr << "Fail to load music";
    }
    music_.play();

    setBoxPosition(30, 220);
    setBoxSize(580, 160);

    setSansPosition(320, 200, true);

    setSoulPosition(320, 250);
    
}

void BattleState::handleEvent(const sf::Event& event) {

}



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





void BattleState::setSansPosition(float x, float y, bool ifSmooth, float factor) {

    if(ifSmooth) {
        sans_.ifSmooth_ = true;
        sans_.SmoothFactor_ = factor;

        sans_.targetX_ = x;
        sans_.targetY_ = y;
    }
    else {
        ifSmooth = false;
        sans_.baseX_ = x;
        sans_.baseY_ = y;
    }
}

void BattleState::updateSansAnimation(float t) {

    sans_.time_ = t;
    sans_.swayX_ = std::sin(sans_.time_ * sans_.swayScorpX_) * sans_.swayFreqX_;   // 水平摆动幅度
    sans_.swayY_ = std::sin(sans_.time_ * sans_.swayScorpY_) * sans_.swayFreqY_;   // 垂直摆动幅度（频率更高）


    if(sans_.ifSmooth_){
    sans_.baseX_ += (sans_.targetX_ - sans_.baseX_) * sans_.SmoothFactor_;
    sans_.baseY_ += (sans_.targetY_ - sans_.baseY_) * sans_.SmoothFactor_;
    }
}

//draw sans
void BattleState::drawSans(sf::RenderWindow& window) {
    
    sansHeadSprite_.setPosition(sf::Vector2f(sans_.baseX_ + sans_.swayX_, sans_.baseY_ + sans_.swayY_));
    sansBodySprite_.setPosition(sf::Vector2f(sans_.baseX_ + sans_.swayX_, sans_.baseY_ + sans_.swayY_));
    sansLegSprite_.setCorners(
        sf::Vector2f(sans_.legBaseX_() + sans_.swayX_, sans_.legBaseY_() + sans_.swayY_),           // 左上
        sf::Vector2f(sans_.legBaseX_() + sans_.legWidth_ + sans_.swayX_, sans_.legBaseY_() + sans_.swayY_), // 右上
        sf::Vector2f(sans_.legBaseX_(), sans_.legBaseY_() + sans_.legHeight_),                 // 左下（固定）
        sf::Vector2f(sans_.legBaseX_() + sans_.legWidth_, sans_.legBaseY_() + sans_.legHeight_)       // 右下（固定）
    );

    sansLegSprite_.draw(window);
    window.draw(sansBodySprite_);
    window.draw(sansHeadSprite_);
}

void BattleState::setBoxPosition(float x, float y ,bool ifSmooth) {

    if(ifSmooth){
        box_.ifSmooth_ = true;
        box_.targetX_ = x;
        box_.targetY_ = y;

    }
    else{
        box_.ifSmooth_ = false;
        box_.x_ = x;
        box_.y_ = y;
    }
}

void BattleState::setBoxSize(float weight, float height, bool ifSmooth) {

    if(ifSmooth){
        box_.ifSmooth_ = true;
        box_.targetWeight_ = weight;
        box_.targetHeight_ = height;
    }
    else{
        box_.ifSmooth_ = false;
        box_.weight_ = weight;
        box_.height_ = height;
    }
}

void BattleState::updateBox() {
    
    if(box_.ifSmooth_){
        //Position
        box_.x_ += (box_.targetX_ - box_.x_) * box_.SmoothFactor_;
        box_.y_ += (box_.targetY_ - box_.y_) * box_.SmoothFactor_;
        boxSprite_.setPosition(sf::Vector2f(box_.x_, box_.y_));

        //Size
        box_.weight_ += (box_.targetWeight_ - box_.weight_) * box_.SmoothFactor_;
        box_.height_ += (box_.targetHeight_ - box_.height_) * box_.SmoothFactor_;
        boxSprite_.setSize(sf::Vector2f(box_.weight_, box_.height_));
    }
    else{
        boxSprite_.setSize(sf::Vector2f(box_.weight_, box_.height_));
        boxSprite_.setPosition(sf::Vector2(box_.x_, box_.y_));
    }
}
//draw box
void BattleState::drawBox(sf::RenderWindow& window) {
    window.draw(boxSprite_);
}

void BattleState::setSoulPosition(float x, float y, bool ifSmooth){
    soul_.x_ = x;
    soul_.y_ = y;
}


void BattleState::setSoulDir(float dir){

}

void BattleState::updateSoul(){

    float dx = 0, dy = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    dy -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dy += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) dx -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dx += 1.0f;

    // 归一化，保持对角线速度和单方向一致
    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0) {
        soul_.x_ += (dx / length) * soul_.moveSpeed_;
        soul_.y_ += (dy / length) * soul_.moveSpeed_;
    }


    soulSprite_.setPosition(sf::Vector2(soul_.x_, soul_.y_));
    soulLightSprite_.setPosition(sf::Vector2(soul_.x_, soul_.y_));

    switch (soul_.Status_){
    case 0:
    soulSprite_.setColor(sf::Color(255, 0, 0, 255));//Red
    soulLightSprite_.setColor(sf::Color(255, 0, 0, 200));
    break;
    
    default:
    break;
    }
}

void BattleState::drawSoul(sf::RenderWindow& window){

    if(soul_.ifDisplay_){
        window.draw(soulLightSprite_);
        window.draw(soulSprite_);
    }
}