#include "AssetManager.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

bool AssetManager::loadTexture(const std::string& id, const std::string& path) {
    sf::Texture tex;
    if (!tex.loadFromFile(path)) {
        std::cerr << "[AssetManager] Failed to load texture: " << path << std::endl;
        return false;
    }
    tex.setSmooth(false);  // 像素风关闭抗锯齿
    textures_[id] = std::move(tex);
    return true;
}

bool AssetManager::loadFont(const std::string& id, const std::string& path) {
    sf::Font font;
    if (!font.openFromFile(path)) {
        std::cerr << "[AssetManager] Failed to load font: " << path << std::endl;
        return false;
    }
    fonts_[id] = std::move(font);
    return true;
}

bool AssetManager::loadSound(const std::string& id, const std::string& path) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(path)) {
        std::cerr << "[AssetManager] Failed to load sound: " << path << std::endl;
        return false;
    }
    sounds_[id] = std::move(buffer);
    return true;
}

const sf::Texture& AssetManager::getTexture(const std::string& id) const {
    auto it = textures_.find(id);
    if (it != textures_.end()) return it->second;
    
    std::cerr << "[AssetManager] Texture not found: " << id << std::endl;
    
    // 返回1x1粉色错误纹理
    if (error_textures_.find("error") == error_textures_.end()) {
        sf::Image img({1, 1}, sf::Color::Magenta);
        if (!error_textures_["error"].loadFromImage(img)) {
            throw std::runtime_error("Failed to create error texture");
        }
    }
    return error_textures_["error"];
}

const sf::Font& AssetManager::getFont(const std::string& id) const {
    auto it = fonts_.find(id);
    if (it != fonts_.end()) return it->second;
    throw std::runtime_error("Font not found: " + id);
}

sf::SoundBuffer& AssetManager::getSound(const std::string& id) const {
    auto it = sounds_.find(id);
    if (it != sounds_.end()) return const_cast<sf::SoundBuffer&>(it->second);
    throw std::runtime_error("Sound not found: " + id);
}

bool AssetManager::hasTexture(const std::string& id) const {
    return textures_.find(id) != textures_.end();
}

void AssetManager::clear() {
    textures_.clear();
    fonts_.clear();
    sounds_.clear();
}