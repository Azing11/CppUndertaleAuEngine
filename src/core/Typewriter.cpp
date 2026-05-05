#include "Typewriter.hpp"

#include <cmath>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <ctime>

static bool startsWith(const std::string& text, size_t pos, const char* prefix) {
    size_t len = std::strlen(prefix);
    return pos + len <= text.size() && text.compare(pos, len, prefix) == 0;
}

static float easeOutCubic(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    t -= 1.0f;
    return 1.0f + t * t * t;
}

Typewriter::Typewriter(unsigned int characterSize)
    : font_(nullptr), characterSize_(characterSize), currentIndex(0),
      timer(0.0f), delay(0.05f), shakeTime(0.0f), entryDuration_(0.18f), position_(0.0f, 0.0f) 
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Typewriter::~Typewriter() {
    delete typerSound_;
}

void Typewriter::setFont(const sf::Font& font) {
    font_ = &font;
}

void Typewriter::parseText(const std::string& text) {
    styledChars.clear();
    sf::Color activeColor = sf::Color::White;
    float activeShake = 0.0f;
    size_t pos = 0;

    while (pos < text.size()) {
        if (text[pos] == '<') {
            if (startsWith(text, pos, "<col>")) {
                pos += 5;
                std::string colorName = parseToken(text, pos);
                if (!colorName.empty()) {
                    activeColor = parseColor(colorName);
                }
                continue;
            }
            if (startsWith(text, pos, "<shake>")) {
                pos += 7;
                std::string shakeValue = parseToken(text, pos);
                try {
                    activeShake = std::stof(shakeValue);
                } catch (...) {
                    activeShake = 0.0f;
                }
                continue;
            }
        }

        styledChars.push_back({text[pos], activeColor, activeShake, 0.0f});
        pos++;
    }
}

sf::Color Typewriter::parseColor(const std::string& colorName) const {
    std::string lower;
    lower.reserve(colorName.size());
    for (char c : colorName) {
        lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    if (lower == "white") return sf::Color::White;
    if (lower == "black") return sf::Color::Black;
    if (lower == "red") return sf::Color::Red;
    if (lower == "green") return sf::Color::Green;
    if (lower == "blue") return sf::Color::Blue;
    if (lower == "yellow") return sf::Color::Yellow;
    if (lower == "cyan") return sf::Color::Cyan;
    if (lower == "magenta") return sf::Color::Magenta;
    if (lower == "transparent") return sf::Color::Transparent;
    if (lower == "gray" || lower == "grey") return sf::Color(128, 128, 128);
    return sf::Color::White;
}

std::string Typewriter::parseToken(const std::string& raw, size_t& pos) const {
    size_t start = pos;
    while (pos < raw.size() && !std::isspace(static_cast<unsigned char>(raw[pos])) && raw[pos] != '<') {
        pos++;
    }
    return raw.substr(start, pos - start);
}

void Typewriter::setText(const std::string& text) {
    fullText = text;
    currentText.clear();
    currentIndex = 0;
    timer = 0.0f;
    shakeTime = 0.0f;
    parseText(fullText);
}

void Typewriter::setPosition(float x, float y) {
    position_ = sf::Vector2f(x, y);
}

void Typewriter::setDelay(float delay) {
    this->delay = delay;
}

void Typewriter::reset() {
    fullText.clear();
    styledChars.clear();
    currentText.clear();
    currentIndex = 0;
    timer = 0.0f;
    shakeTime = 0.0f;
}

void Typewriter::print(const std::string& text, float x, float y, float delay, bool autoEnter) {
    setText(text);
    setPosition(x, y);
    setDelay(delay);
    autoEnter_ = autoEnter;
}

void Typewriter::printInLine(const std::vector<std::string>& lines, float x, float y, float delay, bool autoEnter) {
    std::string text;
    for (size_t i = 0; i < lines.size(); ++i) {
        text += lines[i];
        if (i + 1 < lines.size()) text += '\n';
    }
    print(text, x, y, delay, autoEnter);
}

void Typewriter::setEnableAnimation(bool enable) {
    enableAnimation_ = enable;
}

void Typewriter::setSoundBuffer(const sf::SoundBuffer& buffer) {
    if (typerSound_) {
        typerSound_->setBuffer(buffer);
    } else {
        typerSound_ = new sf::Sound(buffer);
    }
}

bool Typewriter::update(float deltaTime) {
    shakeTime += deltaTime;
    if (currentIndex < styledChars.size()) {
        if (!enableAnimation_) {
            // 立即显示所有字符
            while (currentIndex < styledChars.size()) {
                currentText.push_back(styledChars[currentIndex].character);
                currentIndex++;
            }
            timer = 0.0f;
            // 设置所有字符的age为entryDuration_，使它们立即可见
            for (auto& sc : styledChars) {
                sc.age = entryDuration_;
            }
        } else {
            // 检查跳过键
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift)) {
                // 跳过动画：快速添加所有剩余字符
                while (currentIndex < styledChars.size()) {
                    currentText.push_back(styledChars[currentIndex].character);
                    currentIndex++;
                }
                timer = 0.0f;
                // 设置所有字符的age为entryDuration_，使它们立即可见
                for (auto& sc : styledChars) {
                    sc.age = entryDuration_;
                }
            } else {
                timer += deltaTime;
                if (timer >= delay) {
                    currentText.push_back(styledChars[currentIndex].character);
                    currentIndex++;
                    timer = 0.0f;
                    if (typerSound_) typerSound_->play(); // 播放打字音效
                }
            }
        }
    } else {
        // 打字完成，检查完成键
        if(autoEnter_) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
                reset();
                return true;
            }
        }
    }

    for (size_t i = 0; i < currentIndex; ++i) {
        styledChars[i].age += deltaTime;
    }
    return false;
}

void Typewriter::draw(sf::RenderTarget& target) {
    if (!font_) {
        return;
    }

    float x = position_.x;
    float y = position_.y;
    float lineSpacing = characterSize_ * 1.2f;

    for (size_t i = 0; i < currentIndex; ++i) {
        const auto& styledChar = styledChars[i];
        if (styledChar.character == '\n') {
            x = position_.x;
            y += lineSpacing;
            continue;
        }

        std::string charStr(1, styledChar.character);
        sf::Text charText(*font_, charStr, characterSize_);

        float progress = 1.0f;
        if (styledChar.age < entryDuration_) {
            progress = easeOutCubic(styledChar.age / entryDuration_);
        }
        unsigned char alpha = static_cast<unsigned char>(255.0f * progress);
        sf::Color renderColor = styledChar.color;
        renderColor.a = alpha;
        charText.setFillColor(renderColor);

        float entryOffsetY = (1.0f - progress) * (characterSize_ * 0.5f);
        float offsetX = 0.0f;
        float offsetY = entryOffsetY;
        if (styledChar.shake != 0.0f) {
            // 离散振动：每个字符单独在X和Y轴上随机偏移
            offsetX += (std::rand() % 3 - 1) * styledChar.shake * 5.0f;
            offsetY += (std::rand() % 3 - 1) * styledChar.shake * 5.0f;
        }

        charText.setPosition(sf::Vector2f(x + offsetX, y + offsetY));
        target.draw(charText);

        sf::FloatRect bounds = charText.getLocalBounds();
        x += bounds.position.x + bounds.size.x;
    }
}

bool Typewriter::isFinished() const {
    return currentIndex >= styledChars.size();
}

const std::string& Typewriter::getCurrentText() const {
    return currentText;
}
