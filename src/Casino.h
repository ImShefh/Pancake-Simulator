#pragma once

#include <random>
#include <string>
#include <vector>

class Player;

class Casino {
public:
    bool play(Player& player, std::mt19937& rng) const;

private:
    std::vector<std::string> symbols{"PAN", "7", "CHR", "GOL", "BAR", "RSK"};
};
