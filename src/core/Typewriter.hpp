#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Typewriter {
private:
    sf::Text text;
    std::string fullText;
    std::string currentText;
    size_t currentIndex;
    float timer;
    float delay; // 每个字符的延迟时间（秒）

public:
    // 构造函数，传入字体和字符大小
    Typewriter(const sf::Font& font, unsigned int characterSize = 30);

    // 设置要打印的文本
    void setText(const std::string& text);

    // 设置文本位置
    void setPosition(float x, float y);

    // 设置每个字符的打印延迟
    void setDelay(float delay);

    // 重置打字机状态
    void reset();

    // 打印文本
    void print(const std::string& text, float x, float y, float delay = 0.05f);

    // 更新打字机状态（每帧调用）
    void update(float deltaTime);

    // 绘制文本
    void draw(sf::RenderTarget& target);

    // 检查是否打印完毕
    bool isFinished() const;

    // 获取当前显示的文本
    const std::string& getCurrentText() const;
};
