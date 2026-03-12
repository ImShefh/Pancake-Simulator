#include "Casino.h"

#include <iostream>
#include <limits>

#include "Player.h"

bool Casino::play(Player& player, std::mt19937& rng) const {
    std::cout << "\n=== КАЗИНО ===\n";

    int bet = 0;
    while (true) {
        std::cout << "Введите сумму ставки (0 чтобы выйти): ";
        std::cin >> bet;

        if (!std::cin.good()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Введите число.\n";
            continue;
        }

        if (bet < 0) {
            std::cout << "Ставка не может быть отрицательной.\n";
            continue;
        }

        if (bet > player.getMoney()) {
            std::cout << "Недостаточно денег для такой ставки. Доступно: $" << player.getMoney() << "\n";
            continue;
        }

        break;
    }

    if (bet <= 0) {
        std::cout << "Вы отошли от слот-машины.\n";
        return false;
    }

    player.changeMoney(-bet);

    std::uniform_int_distribution<int> symbolRoll(0, static_cast<int>(symbols.size()) - 1);
    std::uniform_int_distribution<int> chance(1, 100);

    int first = symbolRoll(rng);
    int second = symbolRoll(rng);
    int third = symbolRoll(rng);

    int winChanceBoost = player.getCasinoLuckBonus();
    bool jackpot = (chance(rng) <= 8 + winChanceBoost / 3);
    bool doubleMatch = (chance(rng) <= 30 + winChanceBoost / 2);

    if (jackpot) {
        first = second = third;
    } else if (doubleMatch) {
        second = first;
    }

    std::cout << "+-------------------------+\n";
    std::cout << "|   " << symbols[first] << "   |   " << symbols[second] << "   |   " << symbols[third] << "   |\n";
    std::cout << "+-------------------------+\n";

    if (first == second && second == third) {
        int reward = bet * 4;
        std::cout << "ДЖЕКПОТ! Вы выиграли $" << reward << "!\n";
        player.changeMoney(reward);
        player.changeReputation(2);
        player.addCasinoWin();
    } else if (first == second || second == third || first == third) {
        int reward = bet * 2;
        std::cout << "Отлично! Два символа совпали. Выигрыш: $" << reward << ".\n";
        player.changeMoney(reward);
        player.changeReputation(1);
        player.addCasinoWin();
    } else {
        std::cout << "Совпадений нет. Ставка проиграна.\n";
        player.changeReputation(-1);
        player.changeHunger(2);
    }

    return true;
}
