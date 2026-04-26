// DeformableSprite.cpp
#include "DeformableSprite.hpp"
#include <algorithm>

DeformableSprite::DeformableSprite(const sf::Texture& tex) {
    setTexture(tex);
}

void DeformableSprite::setTexture(const sf::Texture& tex, bool resetRect) {
    texture = &tex;
    if (resetRect) {
        textureRect = sf::IntRect({0, 0}, {(int)tex.getSize().x, (int)tex.getSize().y});
        updateTexCoords();
    }
}

void DeformableSprite::setTextureRect(const sf::IntRect& rect) {
    textureRect = rect;
    updateTexCoords();
}

void DeformableSprite::setCorners(const sf::Vector2f& tl, const sf::Vector2f& tr,
                                  const sf::Vector2f& bl, const sf::Vector2f& br) {
    vertices[0].position = tl;
    vertices[1].position = tr;
    vertices[2].position = bl;
    vertices[3].position = br;
}

void DeformableSprite::setRectangle(const sf::FloatRect& rect) {
    setCorners(
        sf::Vector2f(rect.position.x, rect.position.y),
        sf::Vector2f(rect.position.x + rect.size.x, rect.position.y),
        sf::Vector2f(rect.position.x, rect.position.y + rect.size.y),
        sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y)
    );
}

void DeformableSprite::setPerspective(float tiltX, float tiltY) {
    float w = (float)textureRect.size.x;
    float h = (float)textureRect.size.y;
    float cx = w * 0.5f;
    float cy = h * 0.5f;
    
    float dx = tiltX * w * 0.5f;
    float dy = tiltY * h * 0.5f;
    
    setCorners(
        sf::Vector2f(cx - w/2 - dx + dx*tiltY, cy - h/2 - dy),
        sf::Vector2f(cx + w/2 + dx + dx*tiltY, cy - h/2 - dy),
        sf::Vector2f(cx - w/2 - dx - dx*tiltY, cy + h/2 + dy),
        sf::Vector2f(cx + w/2 + dx - dx*tiltY, cy + h/2 + dy)
    );
}

void DeformableSprite::setSkew(float skewX, float skewY) {
    float w = (float)textureRect.size.x;
    float h = (float)textureRect.size.y;
    
    setCorners(
        sf::Vector2f(0, 0),
        sf::Vector2f(w, skewY * h),
        sf::Vector2f(skewX * w, h),
        sf::Vector2f(w + skewX * w, h + skewY * h)
    );
}

void DeformableSprite::setColors(const sf::Color& tl, const sf::Color& tr,
                                  const sf::Color& bl, const sf::Color& br) {
    vertices[0].color = tl;
    vertices[1].color = tr;
    vertices[2].color = bl;
    vertices[3].color = br;
}

void DeformableSprite::setColor(const sf::Color& color) {
    for (auto& v : vertices) v.color = color;
}

void DeformableSprite::setAlpha(uint8_t alpha) {
    for (auto& v : vertices) v.color.a = alpha;
}

void DeformableSprite::updateTexCoords() {
    float l = (float)textureRect.position.x;
    float t = (float)textureRect.position.y;
    float r = l + (float)textureRect.size.x;
    float b = t + (float)textureRect.size.y;
    
    vertices[0].texCoords = sf::Vector2f(l, t);
    vertices[1].texCoords = sf::Vector2f(r, t);
    vertices[2].texCoords = sf::Vector2f(l, b);
    vertices[3].texCoords = sf::Vector2f(r, b);
}

void DeformableSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.texture = texture;
    target.draw(vertices, 4, sf::PrimitiveType::TriangleStrip, states);
}

sf::FloatRect DeformableSprite::getLocalBounds() const {
    float x0 = vertices[0].position.x, x1 = vertices[1].position.x;
    float x2 = vertices[2].position.x, x3 = vertices[3].position.x;
    float y0 = vertices[0].position.y, y1 = vertices[1].position.y;
    float y2 = vertices[2].position.y, y3 = vertices[3].position.y;
    
    float minX = std::min({x0, x1, x2, x3});
    float maxX = std::max({x0, x1, x2, x3});
    float minY = std::min({y0, y1, y2, y3});
    float maxY = std::max({y0, y1, y2, y3});
    
    return sf::FloatRect(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));
}