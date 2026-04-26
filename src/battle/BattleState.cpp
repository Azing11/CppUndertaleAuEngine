//BattleState.cpp
#include "BattleState.hpp"
#include <iostream>

// 初始化列表中构造 sprite
BattleState::BattleState(AssetManager& assets) 
    : assets_(assets)
    , sansHeadSprite_(assets.getTexture(Res::sansTexture::SANS_HEAD_IDLE))  // 直接传 texture 构造
    , sansBodySprite_(assets.getTexture(Res::sansTexture::SANS_BODY_IDLE))
    , sansLegSprite_(assets.getTexture(Res::sansTexture::SANS_LEG))
    , box({200.f, 150.f})
{
    auto headBounds = sansHeadSprite_.getLocalBounds();
    sansHeadSprite_.setOrigin({headBounds.size.x / 2, headBounds.size.y / 1.5f});
    sansHeadSprite_.setScale({0.5f, 0.5f});

    auto bodyBounds = sansBodySprite_.getLocalBounds();
    sansBodySprite_.setOrigin({bodyBounds.size.x / 2, bodyBounds.size.y / 1.5f});
    sansBodySprite_.setScale({0.5f, 0.5f});

    //box
    box.setFillColor(sf::Color(0, 0, 255, 80));// 设置填充颜色
    box.setOutlineColor(sf::Color::White);// 设置边框颜色
    box.setOutlineThickness(4.f);// 设置边框粗细

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
}

void BattleState::handleEvent(const sf::Event& event) {

}



void BattleState::update(float dt) {
    float t = battleClock.getElapsedTime().asMilliseconds();
    updateSansAnimation(t);
    setBoxPosition(10, 10);
}

void BattleState::render(sf::RenderWindow& window) { 
    drawSans(window);
    drawBox(window);
}

//Sans摇摆
void BattleState::updateSansAnimation(float t) {

    sans_.time_ = t;
    sans_.swayX_ = std::sin(sans_.time_ * sans_.swayScorpX_) * sans_.swayFreqX_;   // 水平摆动幅度
    sans_.swayY_ = std::sin(sans_.time_ * sans_.swayScorpY_) * sans_.swayFreqY_;   // 垂直摆动幅度（频率更高）


    if(sans_.ifSmooth){
    sans_.baseX_ += (sans_.vX_ - sans_.baseX_) * sans_.SmoothFactor_;
    sans_.baseY_ += (sans_.vY_ - sans_.baseY_) * sans_.SmoothFactor_;
    }
}

//draw sans
void BattleState::drawSans(sf::RenderWindow& window){
    
    sansHeadSprite_.setPosition(sf::Vector2f(sans_.baseX_ + sans_.swayX_, sans_.baseY_ + sans_.swayY_));
    sansBodySprite_.setPosition(sf::Vector2f(sans_.baseX_ + sans_.swayX_, sans_.baseY_ + sans_.swayY_));
    sansLegSprite_.setCorners(
        sf::Vector2f((sans_.baseX_ + sans_.legOffsetX_) + sans_.swayX_, (sans_.baseY_ + sans_.legOffsetY_) + sans_.swayY_),           // 左上
        sf::Vector2f((sans_.baseX_ + sans_.legOffsetX_) + sans_.legWidth_ + sans_.swayX_, (sans_.baseY_ + sans_.legOffsetY_) + sans_.swayY_), // 右上
        sf::Vector2f((sans_.baseX_ + sans_.legOffsetX_), (sans_.baseY_ + sans_.legOffsetY_) + sans_.legHeight_),                 // 左下（固定）
        sf::Vector2f((sans_.baseX_ + sans_.legOffsetX_) + sans_.legWidth_, (sans_.baseY_ + sans_.legOffsetY_) + sans_.legHeight_)       // 右下（固定）
    );

    sansLegSprite_.draw(window);
    window.draw(sansBodySprite_);
    window.draw(sansHeadSprite_);
}

void BattleState::setBoxPosition(float x, float y){
    box.setPosition(sf::Vector2(x, y));
}
//draw box
void BattleState::drawBox(sf::RenderWindow& window){
    window.draw(box);
}