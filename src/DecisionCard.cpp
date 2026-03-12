#include "DecisionCard.h"

#include "Player.h"

DecisionCard::DecisionCard(const std::string& title,
                           const std::string& situation,
                           const std::string& choiceA,
                           int moneyA,
                           int reputationA,
                           int hungerA,
                           const std::string& choiceB,
                           int moneyB,
                           int reputationB,
                           int hungerB)
    : title(title), situation(situation), choiceA(choiceA), choiceB(choiceB), moneyA(moneyA), reputationA(reputationA), hungerA(hungerA),
      moneyB(moneyB), reputationB(reputationB), hungerB(hungerB) {}

const std::string& DecisionCard::getTitle() const { return title; }
const std::string& DecisionCard::getSituation() const { return situation; }
const std::string& DecisionCard::getChoiceA() const { return choiceA; }
const std::string& DecisionCard::getChoiceB() const { return choiceB; }

void DecisionCard::applyChoice(Player& player, int choice) const {
    if (choice == 1) {
        player.changeMoney(moneyA);
        player.changeReputation(reputationA);
        player.changeHunger(hungerA);
    } else {
        player.changeMoney(moneyB);
        player.changeReputation(reputationB);
        player.changeHunger(hungerB);
    }
}
