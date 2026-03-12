#pragma once

#include <string>

class Player;

class Upgrade {
public:
    Upgrade(const std::string& name, int cost, const std::string& description);

    const std::string& getName() const;
    int getCost() const;
    const std::string& getDescription() const;

    bool canBuy(const Player& player) const;
    bool buy(Player& player) const;

private:
    std::string name;
    int cost;
    std::string description;
};
