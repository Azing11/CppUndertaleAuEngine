//基本信息
#pragma once

#include <iostream>

namespace baseData {

    struct Player {
        std::string name;
        int lv = 1;
        int currentHp = 0;
        
        int maxHp() const { return 16 + 4 * lv; }
        bool isAlive() const { return currentHp > 0; }
    };

    struct Turn {
        enum class Owner : uint8_t { Player, Enemy };
        static constexpr int maxRounds = 2;
        
        Owner currentOwner;
        int currentRound = 0;
        
        enum class PlayerPhase : uint8_t {None, Selecting, OnBranch, Result};
        PlayerPhase	 currentPlayerPhase;
        
        bool isEnded() const { return currentRound >= maxRounds; }
    };

}