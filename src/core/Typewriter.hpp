#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

class Typewriter {
private:
    struct StyledChar {
        char character;
        sf::Color color;
        float shake;
        float age;
    };

    const sf::Font* font_;
    unsigned int characterSize_;
    std::string fullText;
    std::string currentText;
    std::vector<StyledChar> styledChars;
    size_t currentIndex;
    float timer;
    float delay; // 每个字符的延迟时间（秒）
    float shakeTime;
    float entryDuration_; // 每个字符进入动画时长
    sf::Vector2f position_;
    sf::Sound* typerSound_ = nullptr;

    void parseText(const std::string& text);
    sf::Color parseColor(const std::string& colorName) const;
    std::string parseToken(const std::string& raw, size_t& pos) const;

    bool autoEnter_ = false;
    bool enableAnimation_ = true;

public:
    // 构造函数，传入字符大小
    Typewriter(unsigned int characterSize = 30);

    // 析构函数
    ~Typewriter();

    // 设置字体
    void setFont(const sf::Font& font);

    // 设置要打印的文本
    void setText(const std::string& text);

    // 设置文本位置
    void setPosition(float x, float y);

    // 设置字符打印延迟
    void setDelay(float delay);

    // 重置打字机状态
    void reset();

    // 打印文本
    void print(const std::string& text, float x, float y, float delay = 0.05f, bool autoEnter = false);

    // 打印带指针的选项列表
    void printInLine(const std::vector<std::string>& lines, float x, float y, float delay = 0.05f, bool autoEnter = false);

    // 设置是否启用打字动画
    void setEnableAnimation(bool enable);

    // 设置打字音效缓冲区
    void setSoundBuffer(const sf::SoundBuffer& buffer);

    // 更新打字机状态（每帧调用），返回是否完成
    bool update(float deltaTime);

    // 绘制文本
    void draw(sf::RenderTarget& target);

    // 检查是否打印完毕
    bool isFinished() const;

    // 获取当前显示的文本
    const std::string& getCurrentText() const;
};
