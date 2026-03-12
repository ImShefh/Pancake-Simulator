#include "Achievement.h"

#include "Player.h"

Achievement::Achievement(const std::string& name, const std::string& description, Type type, int threshold)
    : name(name), description(description), type(type), threshold(threshold), unlocked(false) {}

bool Achievement::checkAndUnlock(const Player& player) {
    if (unlocked) {
        return false;
    }

    int value = 0;

    switch (type) {
        case Type::Money:
            value = player.getMoney();
            break;
        case Type::Reputation:
            value = player.getReputation();
            break;
        case Type::CasinoWins:
            value = player.getCasinoWins();
            break;
        case Type::Days:
            value = player.getDay();
            break;
        case Type::Pancakes:
            value = player.getPancakesSold();
            break;
    }

    if (value >= threshold) {
        unlocked = true;
        return true;
    }

    return false;
}

bool Achievement::isUnlocked() const { return unlocked; }
const std::string& Achievement::getName() const { return name; }
const std::string& Achievement::getDescription() const { return description; }

void Achievement::setUnlocked(bool value) {
    unlocked = value;
}
