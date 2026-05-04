#include "Typewriter.hpp"

Typewriter::Typewriter(const sf::Font& font, unsigned int characterSize)
    : text(font, "", characterSize) {
    text.setFillColor(sf::Color::White);
    currentIndex = 0;
    timer = 0.0f;
    delay = 0.05f; // 默认每字符0.05秒
}

void Typewriter::setText(const std::string& text) {
    fullText = text;
    currentText = "";
    currentIndex = 0;
    timer = 0.0f;
    this->text.setString(currentText);
}

void Typewriter::setPosition(float x, float y) {
    text.setPosition(sf::Vector2f(x, y));
}

void Typewriter::setDelay(float delay) {
    this->delay = delay;
}

void Typewriter::reset() {
    currentText = "";
    currentIndex = 0;
    timer = 0.0f;
    text.setString(currentText);
}

void Typewriter::print(const std::string& text, float x, float y, float delay) {
    setText(text);
    setPosition(x, y);
    setDelay(delay);
}

void Typewriter::update(float deltaTime) {
    if (currentIndex < fullText.size()) {
        timer += deltaTime;
        if (timer >= delay) {
            currentText += fullText[currentIndex];
            currentIndex++;
            timer = 0.0f;
            text.setString(currentText);
        }
    }
}

void Typewriter::draw(sf::RenderTarget& target) {
    target.draw(text);
}

bool Typewriter::isFinished() const {
    return currentIndex >= fullText.size();
}

const std::string& Typewriter::getCurrentText() const {
    return currentText;
}