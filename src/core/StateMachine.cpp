#include "StateMachine.hpp"

// 状态机实现
void StateMachine::push(std::unique_ptr<State> state) {
    if (!states_.empty()) {
        states_.top()->pause();
    }
    state->enter();
    states_.push(std::move(state));
}

// 出栈当前状态，并恢复上一个状态（如果有）
void StateMachine::pop() {
    if (states_.empty()) return;
    states_.top()->exit();
    states_.pop();
    if (!states_.empty()) {
        states_.top()->resume();
    }
}

// 替换当前状态（先出栈再入栈）
void StateMachine::change(std::unique_ptr<State> state) {
    pop();
    push(std::move(state));
}

// 清空所有状态
void StateMachine::clear() {
    while (!states_.empty()) {
        pop();
    }
}

void StateMachine::restart()  {
    states_.pop();

}
// 注册状态工厂函数
void StateMachine::registerState(const std::string& name, StateFactory factory) {
    factories_[name] = factory;
}

// 通过注册的工厂函数入栈状态
void StateMachine::pushRegistered(const std::string& name) {
    auto it = factories_.find(name);
    if (it != factories_.end()) {
        push(it->second());
    }
}

// 主循环调用
void StateMachine::handleEvent(const sf::Event& event) {
    if (!states_.empty()) {
        states_.top()->handleEvent(event);
    }
}

// 更新当前状态，并处理状态切换请求
void StateMachine::update(float deltaTime) {
    applyPending();
    
    if (states_.empty()) return;
    
    auto& current = states_.top();
    current->update(deltaTime);
    
    // 检查状态切换请求
    if (auto next = current->getNextState()) {
        push(std::move(next));
    }
    if (current->shouldPop()) {
        pending_actions_.push_back(Action::Pop);
    }
    if (current->shouldQuit()) {
        pending_actions_.push_back(Action::Clear);
    }
}

// 渲染当前状态
void StateMachine::render(sf::RenderWindow& window) {
    if (!states_.empty()) {
        states_.top()->render(window);
    }
}

// 应用所有待处理的状态切换操作（在更新循环结束时调用）
void StateMachine::applyPending() {
    for (auto action : pending_actions_) {
        switch (action) {
            case Action::Pop: pop(); break;
            case Action::Clear: clear(); break;
        }
    }
    pending_actions_.clear();
    
    if (pending_push_) {
        push(std::move(pending_push_));
    }
}

// 辅助函数
bool StateMachine::isEmpty() const {
    return states_.empty();
}

// 如果没有状态了，说明游戏应该退出了
bool StateMachine::shouldQuit() const {
    return states_.empty();
}