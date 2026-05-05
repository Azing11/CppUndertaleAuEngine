//基本信息
#pragma once

#include <iostream>
#include <vector>
#include <string>

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

    struct BattleOptions {
        // ACT
        std::vector<std::string> actTargets = {"sans"};
        std::vector<std::vector<std::string>> actOptions = {
            {"Check", "Compliment", "Joke"}
        };
        std::vector<std::vector<std::string>> actResults = {
            {"* SANS 1 ATK 1 DEF\n* The easiest enemy.\n* Can be easier without killing.", "* You compliment SANS.\n* His eyes light up.\n* Seems flattered.", "* You tell SANS a joke.\n* He chuckles.\n* Seems amused."}
        };

        // ITEM
        std::vector<std::string> itemTargets = {""};
        std::vector<std::vector<std::string>> itemOptions = {
            {"Pie", "Stick", "Bandage"}
        };
        std::vector<std::vector<std::string>> itemResults = {
            {"* You ate the Butterscotch Pie.\n* Your HP was maxed out.", "* You threw the Stick.\n* SANS dodges it.\n* Nothing happened.", "* You used the Bandage.\n* You recovered 10 HP."}
        };

        // MERCY
        std::vector<std::string> mercyTargets = {"sans"};
        std::vector<std::vector<std::string>> mercyOptions = {
            {"Spare", "Flee"}
        };
        std::vector<std::vector<std::string>> mercyResults = {
            {"* You spared SANS.\n* He looks relieved.", "* You tried to flee.\n* But you couldn't escape."}
        };
    };

}