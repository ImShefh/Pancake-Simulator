#include "Event.h"

#include "Player.h"

Event::Event(const std::string& title, const std::string& description, int moneyDelta, int reputationDelta, int hungerDelta)
    : title(title), description(description), moneyDelta(moneyDelta), reputationDelta(reputationDelta), hungerDelta(hungerDelta) {}

const std::string& Event::getTitle() const { return title; }
const std::string& Event::getDescription() const { return description; }

void Event::apply(Player& player) const {
    player.changeMoney(moneyDelta);
    player.changeReputation(reputationDelta);
    player.changeHunger(hungerDelta);
}
