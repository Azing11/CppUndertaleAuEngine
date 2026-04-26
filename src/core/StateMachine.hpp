#pragma once
#include "State.hpp"
#include <stack>
#include <memory>
#include <functional>

class StateMachine {
public:
    using StateFactory = std::function<std::unique_ptr<State>()>;
    
    void push(std::unique_ptr<State> state);
    void pop();
    void change(std::unique_ptr<State> state);  // 替换当前
    void clear();
    void restart();
    
    // 注册状态工厂，用于延迟创建
    void registerState(const std::string& name, StateFactory factory);
    void pushRegistered(const std::string& name);
    
    // 主循环调用
    void handleEvent(const sf::Event& event);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    bool isEmpty() const;
    bool shouldQuit() const;

private:
    std::stack<std::unique_ptr<State>> states_;
    std::unordered_map<std::string, StateFactory> factories_;
    
    // 延迟操作（避免迭代时修改栈）
    enum class Action { Pop, Clear };
    std::vector<Action> pending_actions_;
    std::unique_ptr<State> pending_push_;
    
    void applyPending();
};