// DeformableSprite.hpp
// DeformableSprite.hpp
#pragma once
#include <SFML/Graphics.hpp>

class DeformableSprite {
public:
    DeformableSprite() = default;
    explicit DeformableSprite(const sf::Texture& texture);
    
    // 核心：4 点控制
    void setCorners(const sf::Vector2f& tl, const sf::Vector2f& tr,
                    const sf::Vector2f& bl, const sf::Vector2f& br);
    
    // 快捷变换
    void setRectangle(const sf::FloatRect& rect);
    void setPerspective(float tiltX, float tiltY);
    void setSkew(float skewX, float skewY);
    
    // 颜色控制
    void setColors(const sf::Color& tl, const sf::Color& tr,
                   const sf::Color& bl, const sf::Color& br);
    void setColor(const sf::Color& color);
    void setAlpha(uint8_t alpha);
    
    // 纹理/渲染
    void setTexture(const sf::Texture& texture, bool resetRect = true);
    void setTextureRect(const sf::IntRect& rect);
    
    // 标准接口
    void draw(sf::RenderTarget& target, sf::RenderStates states = {}) const;
    sf::FloatRect getLocalBounds() const;

private:
    void updateTexCoords();
    
    sf::Vertex vertices[4];
    const sf::Texture* texture = nullptr;
    sf::IntRect textureRect;
};