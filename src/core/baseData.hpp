//基本信息
#include <iostream>

namespace baseData
{
    struct player
    {
        std::string name = "";
        
        int lv = 1;
        int currentHp = 0;
        
        void setName(std::string name_){ name = name_; }
        void setLv(int lv_){ lv = lv_; }
        void setCurrentHp(int hp_){ currentHp = hp_; }
        
        std::string getName(){ return name; }
        int getLv(){ return lv; }
        int getMaxHp(){ return 16 + (4 * lv); }
        int getCurrentHp(){ return currentHp; }

    };
}
