#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>

namespace BattleUtils {

inline sf::Vector2f rotatePoint(float px, float py, float cx, float cy, float angleDeg) {
    float rad = angleDeg * 3.14159265f / 180.0f;
    float c = std::cos(rad);
    float s = std::sin(rad);
    float dx = px - cx;
    float dy = py - cy;
    return { cx + dx * c - dy * s, cy + dx * s + dy * c };
}

inline float angleLerp(float a, float b, float t) {
    float diff = b - a;
    while (diff > 180.0f) diff -= 360.0f;
    while (diff < -180.0f) diff += 360.0f;
    return a + diff * t;
}

inline float distToSegment(float px, float py, float x1, float y1, float x2, float y2) {
    float dx = x2 - x1, dy = y2 - y1;
    float lenSq = dx * dx + dy * dy;
    if (lenSq == 0.0f) {
        dx = px - x1; dy = py - y1;
        return std::sqrt(dx * dx + dy * dy);
    }
    float t = std::max(0.0f, std::min(1.0f, ((px - x1) * dx + (py - y1) * dy) / lenSq));
    float cx = x1 + t * dx, cy = y1 + t * dy;
    dx = px - cx; dy = py - cy;
    return std::sqrt(dx * dx + dy * dy);
}

inline sf::Vector2f closestOnSegment(float px, float py, float x1, float y1, float x2, float y2) {
    float dx = x2 - x1, dy = y2 - y1;
    float lenSq = dx * dx + dy * dy;
    if (lenSq == 0.0f) return {x1, y1};
    float t = std::max(0.0f, std::min(1.0f, ((px - x1) * dx + (py - y1) * dy) / lenSq));
    return {x1 + t * dx, y1 + t * dy};
}

} // namespace BattleUtils
