#pragma once

#include <string>

class Player;

class Event {
public:
    Event(const std::string& title, const std::string& description, int moneyDelta, int reputationDelta, int hungerDelta);

    const std::string& getTitle() const;
    const std::string& getDescription() const;

    void apply(Player& player) const;

private:
    std::string title;
    std::string description;
    int moneyDelta;
    int reputationDelta;
    int hungerDelta;
};
