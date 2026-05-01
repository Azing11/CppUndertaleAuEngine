#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

class AssetManager {
public:
    // 加载资源
    bool loadTexture(const std::string& id, const std::string& path);
    bool loadFont(const std::string& id, const std::string& path);
    bool loadSound(const std::string& id, const std::string& path);
    
    // 获取资源（引用，失败抛异常）
    const sf::Texture& getTexture(const std::string& id) const;
    const sf::Font& getFont(const std::string& id) const;
    const sf::SoundBuffer& getSound(const std::string& id) const;
    
    // 检查是否存在
    bool hasTexture(const std::string& id) const;
    bool hasFont(const std::string& id) const;
    
    // 批量加载目录
    void loadTexturesFromDir(const std::string& dir, const std::string& prefix = "");
    
    // 清理
    void clear();

private:
    std::unordered_map<std::string, sf::Texture> textures_;
    std::unordered_map<std::string, sf::Font> fonts_;
    std::unordered_map<std::string, sf::SoundBuffer> sounds_;
    
    mutable std::unordered_map<std::string, sf::Texture> error_textures_;
};