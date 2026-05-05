#pragma once
#include "BattleUtils.hpp"
#include "SansComponent.hpp"
#include "BoxComponent.hpp"
#include "SoulComponent.hpp"
#include "StatusBarComponent.hpp"
#include "ButtonComponent.hpp"
#include "core/baseData.hpp"
#include "core/Typewriter.hpp"

#include <vector>

class TurnController {
public:
    explicit TurnController(AssetManager& assets);
    void init();
    void handleEvent(const sf::Event& event);
    void update(float dt);
    void render(sf::RenderWindow& window);

    void startTurn(baseData::Turn::Owner owner) {
        if (turn_.currentOwner != owner) { // 防重复
            turn_.currentOwner = owner;
        }
        onTurnChanged();  // 回合切换时的设置
    }
    
    void setPlayerPhase(baseData::Turn::PlayerPhase phase) {
        turn_.currentPlayerPhase = phase;
        onPhaseChanged();  // 玩家阶段切换时的设置
    }

    void nextTurn() {
        turn_.currentOwner = (turn_.currentOwner == baseData::Turn::Owner::Player)
                             ? baseData::Turn::Owner::Enemy
                             : baseData::Turn::Owner::Player;
        ++turn_.currentRound;
        onTurnChanged();
    }
    
    void setTurnCount(int n) { turn_.currentRound = n; }
    int getTurnCount() const { return turn_.currentRound; }

private:
    void onTurnChanged();  // 回合切换回调
    void onPhaseChanged(); // 玩家阶段切换回调
    void end();

    void startActTargetSelection();
    void startActOptionSelection();
    void updateActMenuText();
    void finishActResult();

    AssetManager& assets_;

    sf::Sprite background_;
    sf::Music music_;
    sf::Clock battleClock_;

    std::string turnText_ = "<col>white *the<col>red<shake>0.5 enemy";

    SansComponent sans_;
    BoxComponent box_;
    SoulComponent soul_;
    StatusBarComponent statusBar_;
    ButtonComponent button_;
    Typewriter typer_;
    
    bool typingCompleted_ = false;

    enum class ActBranchStep : uint8_t { None, TargetSelect, OptionSelect, Result };
    ActBranchStep actBranchStep_ = ActBranchStep::None;
    std::vector<std::string> actTargets_;
    std::vector<std::vector<std::string>> actOptions_;
    int actTargetIndex_ = 0;
    int actOptionIndex_ = 0;

    baseData::Turn turn_;

    using Owner = baseData::Turn::Owner;
    using PlayerPhase = baseData::Turn::PlayerPhase;
    const float SOUL_OFFSET_X = -40;
};

TurnController::TurnController(AssetManager& assets)
    : assets_(assets)
    , background_(assets.getTexture(Res::uiTexture::BACKGROUND))
    , sans_(assets)
    , box_(assets)
    , soul_(assets)
    , statusBar_(assets)
    , button_(assets)
    , typer_(24)
{
    typer_.setFont(assets.getFont(Res::fonts::MENU_FONT));
    typer_.setSoundBuffer(assets.getSound(Res::sfx::SFX_VOICE_TYPER));

    actTargets_ = {"enemy", "friend", "ally"};
    actOptions_ = {
        {"Check", "Talk", "Threaten"},
        {"Compliment", "Joke", "Encourage"},
        {"Heal", "Support", "Wait"}
    };

    auto bg = background_.getLocalBounds();
    background_.setOrigin({bg.size.x / 2, bg.size.y / 2});
    background_.setScale({1.0f, 0.5f});
    background_.setPosition(sf::Vector2f(320.0f, 240.0f));
}

void TurnController::init() {
    battleClock_.restart();
    statusBar_.init();

    if (!music_.openFromFile(Res::audio::BATTLE_MUSIC)) { std::cerr << "Fail to load music\n"; }

    setTurnCount(0);
    startTurn(Owner::Player);

    sans_.setPosition(320.0f, 320.0f - 65.0f - 40.0f, false);
}

void TurnController::onTurnChanged() {
    actBranchStep_ = ActBranchStep::None;
    if (turn_.currentOwner == Owner::Player) {
        soul_.setMode(SoulComponent::Mode::Menu);
        button_.setSelectable(true);
        box_.setPosition(320.0f, 320.0f, true);
        box_.setSize(283.0f, 283.0f, 65.0f, 65.0f, true);
        box_.setAngle(0.0f, true);
        setPlayerPhase(PlayerPhase::Selecting);
    } else {
        soul_.setDisplayable(true);
        button_.setSelectable(false);
    }
}

void TurnController::onPhaseChanged() {
    if (turn_.currentOwner == Owner::Player) {
        if (turn_.currentPlayerPhase == PlayerPhase::Selecting) {
            actBranchStep_ = ActBranchStep::None;
            button_.setSelectable(true);
            typer_.print(turnText_, 320.0f - 283.0f + 15.0f, 320.0f - 50.0f, 0.05f);
        }
        else if(turn_.currentPlayerPhase == PlayerPhase::OnBranch) {
            button_.setSelected(false);
            switch (button_.getSelection()) {
                case 0: // FIGHT
                    break;
                case 1: // ACT
                    startActTargetSelection();
                    break;
                case 2: // ITEM
                    break; 
                case 3: // MERCY
                    break;
                
                default:
                    break;
            }
        } 
    }
}

void TurnController::startActTargetSelection() {
    actBranchStep_ = ActBranchStep::TargetSelect;
    actTargetIndex_ = 0;
    actOptionIndex_ = 0;
    button_.setSelectable(false);
    updateActMenuText();
}

void TurnController::startActOptionSelection() {
    actBranchStep_ = ActBranchStep::OptionSelect;
    actOptionIndex_ = 0;
    button_.setSelectable(false);
    //soul_.setMode(SoulComponent::Mode::Menu);
    updateActMenuText();
}

void TurnController::finishActResult() {
    soul_.setDisplayable(false);
    actBranchStep_ = ActBranchStep::Result;
    std::string resultText = "*" + actOptions_[actTargetIndex_][actOptionIndex_] + " " + actTargets_[actTargetIndex_] + "\n";
    typer_.setEnableAnimation(true);
    typer_.print(resultText, 320.0f - 283.0f + 15.0f, 320.0f - 50.0f, 0.05f, true);
    setPlayerPhase(PlayerPhase::Result);
}

void TurnController::updateActMenuText() {
    const float startX = 320.0f - 283.0f + 50.0f;
    const float startY = 320.0f - 50.0f + 10.0f;
    const float lineHeight = 30.0f;

    typer_.setEnableAnimation(false);  // 禁用打字动画

    if (actBranchStep_ == ActBranchStep::TargetSelect) {
        std::vector<std::string> lines;
        for (const auto& target : actTargets_) {
            lines.push_back(target);
        }
        typer_.printInLine(lines, startX, startY, 0.05f);
        soul_.setPosition(startX - 20.0f, startY + lineHeight + actTargetIndex_ * lineHeight - 15, true);
    } else if (actBranchStep_ == ActBranchStep::OptionSelect) {
        std::vector<std::string> lines;
        for (const auto& option : actOptions_[actTargetIndex_]) {
            lines.push_back(option);
        }
        typer_.printInLine(lines, startX, startY, 0.05f);
        soul_.setPosition(startX - 20.0f, startY + lineHeight + actOptionIndex_ * lineHeight -15, true);
    }
    
}

inline void TurnController::handleEvent(const sf::Event& event) {
    const auto* keyEvent = event.getIf<sf::Event::KeyReleased>();
    if (turn_.currentOwner == Owner::Player && turn_.currentPlayerPhase == PlayerPhase::OnBranch && actBranchStep_ != ActBranchStep::None && keyEvent) {
        if (actBranchStep_ == ActBranchStep::TargetSelect) {
            if (keyEvent->code == sf::Keyboard::Key::Up) {
                actTargetIndex_ = (actTargetIndex_ - 1 + static_cast<int>(actTargets_.size())) % static_cast<int>(actTargets_.size());
                updateActMenuText();
            } else if (keyEvent->code == sf::Keyboard::Key::Down) {
                actTargetIndex_ = (actTargetIndex_ + 1) % static_cast<int>(actTargets_.size());
                updateActMenuText();
            } else if (keyEvent->code == sf::Keyboard::Key::Enter || keyEvent->code == sf::Keyboard::Key::Z) {
                startActOptionSelection();
            }
        } else if (actBranchStep_ == ActBranchStep::OptionSelect) {
            const auto& options = actOptions_[actTargetIndex_];
            if (keyEvent->code == sf::Keyboard::Key::Up) {
                actOptionIndex_ = (actOptionIndex_ - 1 + static_cast<int>(options.size())) % static_cast<int>(options.size());
                updateActMenuText();
            } else if (keyEvent->code == sf::Keyboard::Key::Down) {
                actOptionIndex_ = (actOptionIndex_ + 1) % static_cast<int>(options.size());
                updateActMenuText();
            } else if (keyEvent->code == sf::Keyboard::Key::Enter || keyEvent->code == sf::Keyboard::Key::Z) {
                finishActResult();
            }
        } else {
            // 其他阶段的按键处理
        }
    }

    button_.handleEvent(event);
}

inline void TurnController::update(float dt) {
    float t = battleClock_.getElapsedTime().asMilliseconds();
    sans_.update(t);
    box_.update();
    button_.update(dt);
    soul_.update(box_);
    statusBar_.update(dt);

    if (typer_.update(dt)) {
        typingCompleted_ = true;
    } else {
        typingCompleted_ = false;
    }

    if (turn_.isEnded()) { end(); }

    if (turn_.currentOwner == Owner::Player)
    {
        //std::cout << "Current Player Phase: " << static_cast<int>(turn_.currentPlayerPhase) << std::endl;
        if(turn_.currentPlayerPhase == PlayerPhase::Selecting){
            button_.setSelectable(true);
            auto Selection = button_.getSelection();
            soul_.setDir(90.0f);
            soul_.setDisplayable(true);
            soul_.setPosition(button_.getSelectionXPosition() + SOUL_OFFSET_X, button_.getSelectionYPosition());
            if(button_.getSelected()){
                setPlayerPhase(PlayerPhase::OnBranch);
            }
        }
        else if(turn_.currentPlayerPhase == PlayerPhase::OnBranch){
            soul_.setDisplayable(true);
            button_.setSelectable(false);
        }
        else if(turn_.currentPlayerPhase == PlayerPhase::Result && typingCompleted_){
            actBranchStep_ = ActBranchStep::None;
            soul_.setDir(0.0f);
            soul_.setMode(SoulComponent::Mode::Normal);
            nextTurn();
        }
    }

}

inline void TurnController::render(sf::RenderWindow& window) {
    window.draw(background_);
    sans_.draw(window);
    box_.draw(window);
    statusBar_.draw(window);
    button_.draw(window);
    soul_.draw(window);
    typer_.draw(window);
}

void TurnController::end() {

}