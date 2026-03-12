#include "Upgrade.h"

#include "Player.h"

Upgrade::Upgrade(const std::string& name, int cost, const std::string& description)
    : name(name), cost(cost), description(description) {}

const std::string& Upgrade::getName() const { return name; }
int Upgrade::getCost() const { return cost; }
const std::string& Upgrade::getDescription() const { return description; }

bool Upgrade::canBuy(const Player& player) const {
    return player.getMoney() >= cost && !player.hasUpgrade(name);
}

bool Upgrade::buy(Player& player) const {
    if (!canBuy(player)) {
        return false;
    }

    player.changeMoney(-cost);
    player.addUpgrade(name);
    return true;
}
