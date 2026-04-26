#pragma once
#include <SFML/Graphics.hpp>

class State {
public:
    virtual ~State() = default;
    
    // 生命周期
    virtual void enter() {}   // 进入状态
    virtual void exit() {}    // 退出状态
    virtual void pause() {}   // 被覆盖时暂停
    virtual void resume() {}  // 重新置顶时恢复
    
    // 每帧调用
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    // 状态切换请求（可选实现）
    virtual std::unique_ptr<State> getNextState() { return nullptr; }
    virtual bool shouldPop() const { return false; }
    virtual bool shouldQuit() const { return false; }
};