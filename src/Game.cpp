#include "Game.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>

Game::Game() : rng(std::random_device{}()) {
    initEvents();
    initDecisionCards();
    initUpgrades();
    initAchievements();
}

void Game::run() {
    clearScreen();
    std::cout << "Добро пожаловать в Pancake & Risk!\n";

    if (!setupSession()) {
        std::cout << "Не удалось запустить игру.\n";
        return;
    }

    while (!isGameOver() && !exitRequested) {
        clearScreen();
        printRestaurantAscii();
        showStatus();

        // Сохраняем состояние до эффектов дня, чтобы показать игроку изменения.
        int moneyBeforeDayEffect = player.getMoney();
        int reputationBeforeDayEffect = player.getReputation();
        int hungerBeforeDayEffect = player.getHunger();
        applyDayOfWeekEffect();
        printStatChange("Изменения от эффекта дня", moneyBeforeDayEffect, reputationBeforeDayEffect, hungerBeforeDayEffect);

        std::cout << "\nВыберите действие:\n\n";
        std::cout << "1) Работать в блинной\n";
        std::cout << "2) Пойти в казино\n";
        std::cout << "3) Купить улучшение\n";
        std::cout << "4) Отдохнуть пируя (-15$, -18 голода)\n";
        std::cout << "0) Сохранить и выйти\n";

        int choice = 0;
        while (true) {
            std::cout << "> ";
            if (askChoiceInRange(choice, 0, 4)) {
                break;
            }
            std::cout << "Введите число от 0 до 4.\n";
        }

        if (choice == 0) {
            std::cout << "Игра сохранена. Выход в меню системы...\n";
            exitRequested = true;
            saveSession();
            break;
        }

        // Сохраняем состояние до выбранного действия, чтобы вывести дельту.
        int moneyBeforeAction = player.getMoney();
        int reputationBeforeAction = player.getReputation();
        int hungerBeforeAction = player.getHunger();
        bool actionCommitted = false;

        switch (choice) {
        case 1:
            foodService.runShift(player, events, rng);
            printStatChange("Изменения после работы в блинной", moneyBeforeAction, reputationBeforeAction, hungerBeforeAction);
            actionCommitted = true;
            break;
        case 2:
            printCasinoAscii();
            actionCommitted = casino.play(player, rng);
            if (actionCommitted) {
                printStatChange("Изменения после казино", moneyBeforeAction, reputationBeforeAction, hungerBeforeAction);
            } else {
                std::cout << "День не потрачен: вы вышли из казино без ставки.\n";
            }
            break;
        case 3:
            actionCommitted = handleUpgradeShop();
            if (actionCommitted) {
                printStatChange("Изменения после магазина", moneyBeforeAction, reputationBeforeAction, hungerBeforeAction);
            } else {
                std::cout << "День не потрачен: вы вышли из магазина улучшений.\n";
            }
            break;
        case 4:
            std::cout << "Вы устроили пир и хорошо отдохнули.\n";
            player.changeMoney(-15);
            player.changeHunger(-18);
            printStatChange("Изменения после отдыха пируя", moneyBeforeAction, reputationBeforeAction, hungerBeforeAction);
            break;
        }

        if (!actionCommitted) {
            saveSession();
            waitForEnter();
            continue;
        }


        int moneyBeforeDecision = player.getMoney();
        int reputationBeforeDecision = player.getReputation();
        int hungerBeforeDecision = player.getHunger();
        bool decisionTriggered = triggerDecisionCard();
        if (decisionTriggered) {
            printStatChange("Итог после карты решения", moneyBeforeDecision, reputationBeforeDecision, hungerBeforeDecision);
        }

        if (exitRequested) {
            std::cout << "Игра сохранена. Выход в меню системы...\n";
            saveSession();
            break;
        }

        checkAchievements();
        player.nextDay();
        saveSession();
        waitForEnter();
    }

    if (!exitRequested) {
        clearScreen();
        printGameOverAscii();
        std::cout << "Вы продержались " << player.getDay() - 1 << " дней.\n";
        saveSession();
    }
}


void Game::printRestaurantAscii() const {
    std::cout << "\n========================================\n";
    std::cout << "      PANCAKE & RISK - РЕСТОРАН         \n";
    std::cout << "========================================\n";
    std::cout << "      (\\_/)" << "\n";
    std::cout << "     ( •_•)  Свежая смена начинается!\n";
    std::cout << "    / >🥞  Сохрани свою блинную.\n";
}

void Game::printCasinoAscii() const {
    std::cout << "\n########################################\n";
    std::cout << "                 КАЗИНО                 \n";
    std::cout << "########################################\n";
    std::cout << "| [PAN] [777] [BAR] |\n";
    std::cout << "|     КРУТИ И РИСКУЙ |\n";
}

void Game::printGameOverAscii() const {
    std::cout << "\n========================================\n";
    std::cout << "              ИГРА ОКОНЧЕНА             \n";
    std::cout << "========================================\n";
    std::cout << "   Мечта о блинной рухнула...\n";
}

void Game::printAchievementAscii(const Achievement& achievement) const {
    std::cout << "\n******** ДОСТИЖЕНИЕ ОТКРЫТО ********\n";
    std::cout << achievement.getName() << "\n";
    std::cout << achievement.getDescription() << "\n";
    std::cout << "************************************\n";
}

void Game::initEvents() {
    events = {
        {"Офисный заказ", "Местный офис заказал большую партию блинов.", 40, 3, 4},
        {"Фуд-блогер", "Блогер похвалил вашу фирменную подачу.", 25, 5, 2},
        {"Сгоревшая партия", "Часть блинов подгорела, пришлось вернуть деньги.", -30, -4, 6},
        {"Поломка холодильника", "Ингредиенты испортились из-за сбоя охлаждения.", -25, -2, 3},
        {"Уличные музыканты", "Живая музыка привлекла гостей в кафе.", 20, 3, 1},
        {"Дождливый день", "Из-за плохой погоды стало меньше клиентов.", -15, -2, 1},
        {"Задержка поставки", "Поставку привезли поздно и дороже обычного.", -18, -1, 2},
        {"Партнёрство с кофейней", "Соседняя кофейня порекомендовала вашу блинную.", 22, 4, 0},
        {"Жалоба клиента", "Недовольный гость оставил плохой отзыв.", -10, -5, 3},
        {"Детский праздник", "Праздник занял почти все столики.", 35, 4, 5},
        {"Вирусное фото", "Фото вашей башни из блинов стало вирусным.", 45, 6, 2},
        {"Скидка у поставщика", "Удалось купить муку по выгодной цене.", 15, 1, 0},
        {"Отключение электричества", "Полсмены прошло без света.", -28, -3, 4},
        {"Кулинарный конкурс", "Вы заняли второе место в городском конкурсе.", 18, 5, 6},
        {"Поток туристов", "Район заполнили туристы.", 30, 3, 2},
        {"Рост цен на масло", "Масло резко подорожало.", -20, -1, 1},
        {"Бесплатная дегустация", "Дегустация улучшила узнаваемость.", -8, 4, 5},
        {"Успешная ночная смена", "Поздние гости принесли хороший доход.", 24, 2, 7},
        {"Кухонный инцидент", "Небольшая авария замедлила работу кухни.", -22, -3, 8},
        {"Щедрые чаевые", "Постоянный клиент оставил хорошие чаевые.", 16, 2, 0},
        {"Попадание в приложение", "Сервис доставки выделил вашу блинную.", 27, 5, 3},
        {"Проверка санитарии", "Инспектор нашёл несколько нарушений.", -14, -2, 2},
        {"Идеальный отзыв", "Критик поставил максимальную оценку.", 12, 6, 1},
        {"Хаос в час пик", "Наплыв заказов перегрузил команду.", 18, -1, 9},
        {"Кража запасов", "Кто-то украл сироп со склада.", -26, -2, 1},
        {"Благотворительная акция", "Вы помогли району и получили уважение.", -12, 7, 4},
        {"Упоминание на ТВ", "Утреннее шоу похвалило ваше меню.", 32, 5, 1},
        {"Агрессивный конкурент", "Конкурент резко снизил цены.", -17, -3, 0},
        {"Городской фестиваль", "На фестивале вы продали много блинов.", 38, 4, 6},
        {"Болезнь сотрудника", "Пришлось вести смену почти в одиночку.", -11, -1, 10}
    };
}

void Game::initDecisionCards() {
    decisionCards = {
        {"Секретный рецепт", "Шеф предлагает премиальный рецепт за плату.", "Купить рецепт", -25, 6, 2, "Отказаться", 0, -1, 0},
        {"Ночная доставка", "Поступил срочный ночной заказ.", "Принять", 20, 1, 8, "Отказать", 0, -2, -2},
        {"Интервью на ТВ", "Местный канал зовёт вас в эфир.", "Согласиться", 5, 7, 4, "Остаться на кухне", 0, 1, 0},
        {"Сомнительные ингредиенты", "Поставщик предлагает подозрительно дешёвый товар.", "Взять", 15, -6, 1, "Отказаться", -5, 2, 0},
        {"Музыкант в зале", "Уличный музыкант просит выступать у вас.", "Нанять", -10, 4, 0, "Отказать", 0, -1, 0},
        {"Просьба о бесплатной еде", "Студент просит бесплатный обед.", "Помочь", -6, 5, 1, "Отказать", 0, -3, 0},
        {"Кулинарный тренинг", "Можно пройти однодневное обучение.", "Пойти", -8, 4, 3, "Пропустить", 0, 0, 0},
        {"Рискованная реклама", "Маркетолог предлагает агрессивную кампанию.", "Запустить", -20, 9, 2, "Пропустить", 0, 0, 0},
        {"Сбор на безопасность", "Районный фонд просит пожертвование.", "Пожертвовать", -12, 3, 0, "Игнорировать", 0, -2, 0},
        {"Таинственный клиент", "Необычный гость просит особое блюдо.", "Экспериментировать", 10, 4, 5, "Сыграть безопасно", 4, 0, 0}
    };
}

void Game::initUpgrades() {
    upgrades = {
        {"Better pan", 60, "Выше эффективность кухни и скорость готовки."},
        {"Better ingredients", 85, "Лучший вкус: рост дохода и репутации."},
        {"Advertising", 100, "О блинной узнает больше людей."},
        {"VIP table", 140, "Привлекает состоятельных клиентов."},
        {"Security", 120, "Снижает риск неприятностей и повышает удачу."}
    };
}

void Game::initAchievements() {
    achievements = {
        {"Первые накопления", "Накопите 200$.", Achievement::Type::Money, 200},
        {"Блинный капитал", "Накопите 500$.", Achievement::Type::Money, 500},
        {"Звезда района", "Достигните 70 репутации.", Achievement::Type::Reputation, 70},
        {"Любимец города", "Достигните 90 репутации.", Achievement::Type::Reputation, 90},
        {"Везучая рука", "Выиграйте в казино 3 раза.", Achievement::Type::CasinoWins, 3},
        {"Шёпот казино", "Выиграйте в казино 10 раз.", Achievement::Type::CasinoWins, 10},
        {"Герой недели", "Продержитесь 7 дней.", Achievement::Type::Days, 7},
        {"Железная лопатка", "Продержитесь 20 дней.", Achievement::Type::Days, 20},
        {"Пекарь-новичок", "Продайте 100 блинов.", Achievement::Type::Pancakes, 100},
        {"Блинный магнат", "Продайте 500 блинов.", Achievement::Type::Pancakes, 500}
    };
}

void Game::showStatus() const {
    std::cout << "День " << player.getDay() << " (" << getDayName() << ")\n";
    std::cout << "Деньги: $" << player.getMoney() << " | Репутация: " << player.getReputation();
    std::cout << " | Голод: " << player.getHunger() << "/100\n";
}

void Game::printStatChange(const std::string& title, int moneyBefore, int reputationBefore, int hungerBefore) const {
    int moneyDelta = player.getMoney() - moneyBefore;
    int reputationDelta = player.getReputation() - reputationBefore;
    int hungerDelta = player.getHunger() - hungerBefore;

    std::cout << "\n[" << title << "] ";
    std::cout << "Деньги " << (moneyDelta >= 0 ? "+" : "") << moneyDelta << ", ";
    std::cout << "Репутация " << (reputationDelta >= 0 ? "+" : "") << reputationDelta << ", ";
    std::cout << "Голод " << (hungerDelta >= 0 ? "+" : "") << hungerDelta << "\n";
}

void Game::applyDayOfWeekEffect() {
    int dayIndex = (player.getDay() - 1) % 7;

    std::cout << "\n[Эффект дня] ";
    switch (dayIndex) {
    case 0:
        std::cout << "Понедельник: подготовка к неделе стоит денег. (-8$)\n";
        player.changeMoney(-8);
        break;
    case 1:
        std::cout << "Вторник: стабильная работа. (+5$)\n";
        player.changeMoney(5 + player.getServiceBonus() / 3);
        break;
    case 2:
        std::cout << "Среда: социальная активность повышает репутацию. (+2)\n";
        player.changeReputation(2 + player.getReputationBonus() / 4);
        break;
    case 3:
        std::cout << "Четверг: усталость накапливается. (+6 к голоду)\n";
        player.changeHunger(6);
        break;
    case 4:
        std::cout << "Пятница: вечерний наплыв гостей. (+14$)\n";
        player.changeMoney(14 + player.getServiceBonus() / 2);
        break;
    case 5:
        std::cout << "Суббота: премиальная атмосфера добавляет репутацию. (+4)\n";
        player.changeReputation(4 + player.getReputationBonus() / 3);
        break;
    case 6:
        std::cout << "Воскресенье: можно перевести дух. (-7 к голоду)\n";
        player.changeHunger(-7);
        break;
    }
}

bool Game::triggerDecisionCard() {
    std::uniform_int_distribution<int> chance(1, 100);
    if (chance(rng) > 40 || decisionCards.empty()) {
        return false;
    }

    std::uniform_int_distribution<std::size_t> pick(0, decisionCards.size() - 1);
    const DecisionCard& card = decisionCards[pick(rng)];

    std::cout << "\n=== КАРТА РЕШЕНИЯ ===\n";
    std::cout << card.getTitle() << "\n";
    std::cout << card.getSituation() << "\n";
    std::cout << "1) " << card.getChoiceA() << "\n";
    std::cout << "2) " << card.getChoiceB() << "\n";

    int selected = 0;
    while (true) {
        std::cout << "> ";
        if (askChoiceInRange(selected, 1, 2)) {
            break;
        }
        std::cout << "Введите только 1 или 2.\n";
    }

    card.applyChoice(player, selected);
    return true;
}


bool Game::handleUpgradeShop() {
    while (true) {
        std::cout << "\n=== МАГАЗИН УЛУЧШЕНИЙ ===\n";
        for (std::size_t index = 0; index < upgrades.size(); ++index) {
            const Upgrade& upgrade = upgrades[index];
            std::cout << index + 1 << ") " << upgrade.getName() << " - $" << upgrade.getCost();
            std::cout << " | " << upgrade.getDescription();
            if (player.hasUpgrade(upgrade.getName())) {
                std::cout << " [КУПЛЕНО]";
            }
            std::cout << "\n";
        }
        std::cout << "0) Выйти в главное меню\n";
        std::cout << "> ";

        int selection = 0;
        if (!readMenuChoice(selection)) {
            std::cout << "Некорректный ввод.\n";
            continue;
        }


        if (selection == 0) {
            std::cout << "Вы вернулись в главное меню.\n";
            return false;
        }

        if (selection < 0 || selection > static_cast<int>(upgrades.size())) {
            std::cout << "Введите номер из списка.\n";
            continue;
        }


    Upgrade& target = upgrades[selection - 1];
    if (player.hasUpgrade(target.getName())) {
        std::cout << "Это улучшение уже куплено. Выберите другое.\n";
        continue;
    }

    if (target.buy(player)) {
        std::cout << "Улучшение куплено: " << target.getName() << "\n";
        return true;
    }

    std::cout << "Покупка не удалась. Не хватает денег.\n";
    }
}

void Game::checkAchievements() {
    for (Achievement& achievement : achievements) {
        if (achievement.checkAndUnlock(player)) {
            printAchievementAscii(achievement);
        }
    }
}

bool Game::isGameOver() const {
    return player.getMoney() < 0 || player.getReputation() <= 0 || player.getHunger() >= 100;
}

std::string Game::getDayName() const {
    static const std::vector<std::string> dayNames = {
        "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота", "Воскресенье"
    };

    return dayNames[(player.getDay() - 1) % 7];
}


void Game::clearScreen() const {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear >/dev/null 2>&1");
#endif
}

void Game::waitForEnter() const {
    std::cout << "\nНажмите Enter, чтобы продолжить...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string line;
    std::getline(std::cin, line);
}


bool Game::readMenuChoice(int& value) const {
    std::cin >> value;
    if (std::cin.good()) {
        return true;
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return false;
}

bool Game::askChoiceInRange(int& value, int minValue, int maxValue) const {
    if (!readMenuChoice(value)) {
        return false;
    }

    return value >= minValue && value <= maxValue;
}

bool Game::setupSession() {
    std::filesystem::create_directories("saves");

    if (!std::filesystem::exists("saves/savegame.sav")) {
        std::cout << "\nСохранение не найдено. Запускаем новую игру.\n";
        player = Player();
        for (Achievement& achievement : achievements) {
            achievement.setUnlocked(false);
        }
        return saveSession();
    }

    while (true) {
        std::cout << "\n1) Загрузить игру\n2) Новая игра\n> ";

        int mode = 0;
        if (!askChoiceInRange(mode, 1, 2)) {
            std::cout << "Некорректный выбор режима. Повторите.\n";
            continue;
        }

        if (mode == 1) {
            if (loadSession()) {
                return true;
            }
            std::cout << "Сохранение не найдено или повреждено. Запускаем новую игру.\n";
        }

        player = Player();
        for (Achievement& achievement : achievements) {
            achievement.setUnlocked(false);
        }
        return saveSession();
    }
}


bool Game::saveSession() const {
    std::filesystem::create_directories("saves");
    std::ofstream out("saves/savegame.sav");
    if (!out.is_open()) {
        return false;
    }

    out << player.getMoney() << ' ' << player.getHunger() << ' ' << player.getReputation() << ' ';
    out << player.getDay() << ' ' << player.getCasinoWins() << ' ' << player.getPancakesSold() << '\n';

    const std::vector<std::string>& owned = player.getOwnedUpgrades();
    out << owned.size() << '\n';
    for (const std::string& upgradeName : owned) {
        out << upgradeName << '\n';
    }

    out << achievements.size() << '\n';
    for (const Achievement& achievement : achievements) {
        out << (achievement.isUnlocked() ? 1 : 0) << '\n';
    }

    return true;
}

bool Game::loadSession() {
    std::ifstream in("saves/savegame.sav");
    if (!in.is_open()) {
        std::cout << "Сохранение не найдено.\n";
        return false;
    }

    int money = 0;
    int hunger = 0;
    int reputation = 0;
    int day = 0;
    int casinoWins = 0;
    int pancakesSold = 0;

    if (!(in >> money >> hunger >> reputation >> day >> casinoWins >> pancakesSold)) {
        std::cout << "Файл сохранения повреждён.\n";
        return false;
    }

    player = Player();
    player.loadState(money, hunger, reputation, day, casinoWins, pancakesSold);
    player.resetProgress();

    std::size_t upgradesCount = 0;
    if (!(in >> upgradesCount)) {
        std::cout << "Файл сохранения повреждён.\n";
        return false;
    }

    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (std::size_t index = 0; index < upgradesCount; ++index) {
        std::string upgradeName;
        std::getline(in, upgradeName);
        if (!upgradeName.empty()) {
            player.addUpgrade(upgradeName);
        }
    }

    for (Achievement& achievement : achievements) {
        achievement.setUnlocked(false);
    }

    std::size_t achievementsCount = 0;
    if (!(in >> achievementsCount)) {
        std::cout << "Файл сохранения повреждён.\n";
        return false;
    }

    for (std::size_t index = 0; index < achievements.size() && index < achievementsCount; ++index) {
        int flag = 0;
        if (!(in >> flag)) {
            std::cout << "Файл сохранения повреждён.\n";
            return false;
        }
        achievements[index].setUnlocked(flag != 0);
    }

    std::cout << "Сохранение успешно загружено.\n";
    return true;
}
