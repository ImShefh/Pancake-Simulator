#pragma once

#include <string>

class Player;

class DecisionCard {
public:
    DecisionCard(const std::string& title,
                 const std::string& situation,
                 const std::string& choiceA,
                 int moneyA,
                 int reputationA,
                 int hungerA,
                 const std::string& choiceB,
                 int moneyB,
                 int reputationB,
                 int hungerB);

    const std::string& getTitle() const;
    const std::string& getSituation() const;
    const std::string& getChoiceA() const;
    const std::string& getChoiceB() const;

    void applyChoice(Player& player, int choice) const;

private:
    std::string title;
    std::string situation;
    std::string choiceA;
    std::string choiceB;

    int moneyA;
    int reputationA;
    int hungerA;

    int moneyB;
    int reputationB;
    int hungerB;
};
