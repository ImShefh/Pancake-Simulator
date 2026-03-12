#pragma once

#include <random>
#include <string>
#include <vector>

#include "Achievement.h"
#include "Casino.h"
#include "DecisionCard.h"
#include "Event.h"
#include "FoodService.h"
#include "Player.h"
#include "Upgrade.h"

class Game {
public:
    Game();
    void run();

private:
    Player player;
    FoodService foodService;
    Casino casino;

    std::vector<Event> events;
    std::vector<DecisionCard> decisionCards;
    std::vector<Upgrade> upgrades;
    std::vector<Achievement> achievements;

    std::mt19937 rng;

    void printRestaurantAscii() const;
    void printCasinoAscii() const;
    void printGameOverAscii() const;
    void printAchievementAscii(const Achievement& achievement) const;

    void initEvents();
    void initDecisionCards();
    void initUpgrades();
    void initAchievements();

    void clearScreen() const;
    void waitForEnter() const;
    bool readMenuChoice(int& value) const;
    bool askChoiceInRange(int& value, int minValue, int maxValue) const;

    bool setupSession();
    bool saveSession() const;
    bool loadSession();

    void showStatus() const;
    void printStatChange(const std::string& title, int moneyBefore, int reputationBefore, int hungerBefore) const;
    void applyDayOfWeekEffect();
    bool triggerDecisionCard();

    bool handleUpgradeShop();
    void checkAchievements();
    bool isGameOver() const;

    std::string getDayName() const;

    bool exitRequested = false;
};
