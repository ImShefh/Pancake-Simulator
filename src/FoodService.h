#pragma once

#include <random>
#include <vector>

class Event;
class Player;

class FoodService {
public:
    void runShift(Player& player, const std::vector<Event>& events, std::mt19937& rng) const;
};
