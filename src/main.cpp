#include "core/Game.hpp"

int main() {
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        // 日志记录
        return -1;
    }
    return 0;
}