#pragma once

#include <string>

class Player;

class Achievement {
public:
    enum class Type {
        Money,
        Reputation,
        CasinoWins,
        Days,
        Pancakes
    };

    Achievement(const std::string& name, const std::string& description, Type type, int threshold);

    bool checkAndUnlock(const Player& player);
    bool isUnlocked() const;
    void setUnlocked(bool value);

    const std::string& getName() const;
    const std::string& getDescription() const;

private:
    std::string name;
    std::string description;
    Type type;
    int threshold;
    bool unlocked;
};
