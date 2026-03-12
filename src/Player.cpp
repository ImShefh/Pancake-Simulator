#include "Player.h"

#include <algorithm>

Player::Player()
    : money(120), hunger(20), reputation(50), day(1), casinoWins(0), pancakesSold(0),
      serviceBonus(0), reputationBonus(0), casinoLuckBonus(0), securityBonus(0) {}

int Player::getMoney() const { return money; }
int Player::getHunger() const { return hunger; }
int Player::getReputation() const { return reputation; }
int Player::getDay() const { return day; }
int Player::getCasinoWins() const { return casinoWins; }
int Player::getPancakesSold() const { return pancakesSold; }

int Player::getServiceBonus() const { return serviceBonus; }
int Player::getReputationBonus() const { return reputationBonus; }
int Player::getCasinoLuckBonus() const { return casinoLuckBonus; }
int Player::getSecurityBonus() const { return securityBonus; }

bool Player::hasUpgrade(const std::string& upgradeName) const {
    return std::find(ownedUpgrades.begin(), ownedUpgrades.end(), upgradeName) != ownedUpgrades.end();
}

void Player::changeMoney(int amount) {
    money += amount;
}

void Player::changeHunger(int amount) {
    hunger += amount;
    hunger = std::clamp(hunger, 0, 100);
}

void Player::changeReputation(int amount) {
    reputation += amount;
    reputation = std::clamp(reputation, 0, 100);
}

void Player::nextDay() {
    ++day;
}

void Player::addCasinoWin() {
    ++casinoWins;
}

void Player::addPancakesSold(int amount) {
    pancakesSold += amount;
}

void Player::addUpgrade(const std::string& upgradeName) {
    if (hasUpgrade(upgradeName)) {
        return;
    }

    ownedUpgrades.push_back(upgradeName);

    if (upgradeName == "Better pan") {
        serviceBonus += 4;
    } else if (upgradeName == "Better ingredients") {
        serviceBonus += 6;
        reputationBonus += 3;
    } else if (upgradeName == "Advertising") {
        reputationBonus += 6;
    } else if (upgradeName == "VIP table") {
        serviceBonus += 8;
        reputationBonus += 4;
    } else if (upgradeName == "Security") {
        securityBonus += 12;
        casinoLuckBonus += 5;
    }
}

void Player::loadState(int moneyValue, int hungerValue, int reputationValue, int dayValue, int casinoWinsValue, int pancakesSoldValue) {
    money = moneyValue;
    hunger = std::clamp(hungerValue, 0, 100);
    reputation = std::clamp(reputationValue, 0, 100);
    day = std::max(1, dayValue);
    casinoWins = std::max(0, casinoWinsValue);
    pancakesSold = std::max(0, pancakesSoldValue);
}

void Player::resetProgress() {
    serviceBonus = 0;
    reputationBonus = 0;
    casinoLuckBonus = 0;
    securityBonus = 0;
    ownedUpgrades.clear();
}

const std::vector<std::string>& Player::getOwnedUpgrades() const {
    return ownedUpgrades;
}
