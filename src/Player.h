#pragma once

#include <string>
#include <vector>

class Player {
public:
    Player();

    int getMoney() const;
    int getHunger() const;
    int getReputation() const;
    int getDay() const;
    int getCasinoWins() const;
    int getPancakesSold() const;

    int getServiceBonus() const;
    int getReputationBonus() const;
    int getCasinoLuckBonus() const;
    int getSecurityBonus() const;

    bool hasUpgrade(const std::string& upgradeName) const;

    void changeMoney(int amount);
    void changeHunger(int amount);
    void changeReputation(int amount);
    void nextDay();

    void addCasinoWin();
    void addPancakesSold(int amount);
    void addUpgrade(const std::string& upgradeName);
    void loadState(int moneyValue, int hungerValue, int reputationValue, int dayValue, int casinoWinsValue, int pancakesSoldValue);
    void resetProgress();

    const std::vector<std::string>& getOwnedUpgrades() const;

private:
    int money;
    int hunger;
    int reputation;
    int day;

    int casinoWins;
    int pancakesSold;

    int serviceBonus;
    int reputationBonus;
    int casinoLuckBonus;
    int securityBonus;

    std::vector<std::string> ownedUpgrades;
};
