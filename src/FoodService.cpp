#include "FoodService.h"

#include <iostream>

#include "Event.h"
#include "Player.h"

void FoodService::runShift(Player& player, const std::vector<Event>& events, std::mt19937& rng) const {
    std::uniform_int_distribution<int> salesDistribution(6, 14);
    std::uniform_int_distribution<int> eventChance(1, 100);

    int sold = salesDistribution(rng) + player.getReputation() / 12 + player.getServiceBonus() / 2;
    int income = sold * (3 + player.getServiceBonus() / 6) + 10;

    player.addPancakesSold(sold);
    player.changeMoney(income);
    player.changeHunger(7);

    std::cout << "\n=== РАБОТА В БЛИННОЙ ===\n";
    std::cout << "Вы продали " << sold << " блинов и заработали $" << income << ".\n";

    int trigger = eventChance(rng);
    int baseThreshold = 35 - player.getSecurityBonus() / 2;
    if (trigger <= baseThreshold && !events.empty()) {
        std::uniform_int_distribution<std::size_t> eventPick(0, events.size() - 1);
        const Event& event = events[eventPick(rng)];

        std::cout << "\n[Случайное событие] " << event.getTitle() << "\n";
        std::cout << event.getDescription() << "\n";
        event.apply(player);
    } else {
        std::cout << "Смена прошла спокойно, без внештатных ситуаций.\n";
    }
}
