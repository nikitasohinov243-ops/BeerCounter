#include <iostream>
#include <string>
#include <tgbot/tgbot.h>
#include "BeerCounter.h" 
#include "BeerFileManager.h"

//clang++ -std=c++17 BeerCounter.cpp -I. -I/opt/homebrew/include -L/opt/homebrew/lib -I/usr/local/include -L/usr/local/lib -lTgBot -lboost_system -lssl -lcrypto -lz -o beer_bot

int main() {
    BeerTracker tracker;
    BeerFileManager fileManager;
    std::string filename = "beer.txt";
    fileManager.load(tracker, filename);

    TgBot::Bot bot("8963471483:AAFd0bpIA24nH_OSMXB4XxtUd5TK_W8595A");

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет! Я бот для учета пива на смене. Напиши /invent чтобы увидеть остатки.");
    });

    bot.getEvents().onCommand("invent", [&bot, &tracker](TgBot::Message::Ptr message) {
        std::string response = "🍺 Текущие остатки на кранах:\n\n";
        for (const auto& [name, liters] : tracker.getMap()) {
            response += name + ": " + std::to_string(liters) + " л\n";
        }

        bot.getApi().sendMessage(message->chat->id, response);
    });

    #include <sstream> // Не забудь подключить этот заголовок в начале файла для работы со стримами

// ... внутри int main() ...

    // 1. Команда добавления
    bot.getEvents().onCommand("add", [&bot, &tracker](TgBot::Message::Ptr message) {
        std::stringstream ss(message->text);
        std::string cmd, name;
        double liters = 0.0;

        ss >> cmd >> name >> liters; // Считывает по порядку: "/add", "Лагер", "10"

        if (name.empty() || liters <= 0) {
            bot.getApi().sendMessage(message->chat->id, "Формат ошибки. Используй: /add [сорт] [литры]\nПример: /add Лагер 10");
            return;
        }

        tracker.add(name, liters);
        bot.getApi().sendMessage(message->chat->id, "✅ Добавлено " + std::to_string(liters) + " л. Сорт: " + name);
    });

    // 2. Команда списания
    bot.getEvents().onCommand("subtract", [&bot, &tracker](TgBot::Message::Ptr message) {
        std::stringstream ss(message->text);
        std::string cmd, name;
        double liters = 0.0;

        ss >> cmd >> name >> liters;

        if (name.empty() || liters <= 0) {
            bot.getApi().sendMessage(message->chat->id, "Формат ошибки. Используй: /subtract [сорт] [литры]\nПример: /subtract Лагер 3");
            return;
        }

        try {
            tracker.subtract(name, liters);
            bot.getApi().sendMessage(message->chat->id, "🔻 Списано " + std::to_string(liters) + " л. Сорт: " + name);
        } catch (const std::exception& e) {
            bot.getApi().sendMessage(message->chat->id, e.what());
        }
    });

    // 3. Команда сброса остатка в ноль: /reset Лагер
    bot.getEvents().onCommand("reset", [&bot, &tracker](TgBot::Message::Ptr message) {
        std::stringstream ss(message->text);
        std::string cmd, name;
        ss >> cmd >> name;

        if (name.empty()) {
            bot.getApi().sendMessage(message->chat->id, "Формат: /reset [сорт]\nПример: /reset Лагер");
            return;
        }

        try {
            tracker.reset(name);
            bot.getApi().sendMessage(message->chat->id, "🔄 Остаток сорта '" + name + "' сброшен до 0 л.");
        } catch (const std::exception& e) {
            bot.getApi().sendMessage(message->chat->id, e.what());
        }
    });

    // 4. Команда полного удаления сорта из базы: /remove Лагер
    bot.getEvents().onCommand("remove", [&bot, &tracker](TgBot::Message::Ptr message) {
        std::stringstream ss(message->text);
        std::string cmd, name;
        ss >> cmd >> name;

        if (name.empty()) {
            bot.getApi().sendMessage(message->chat->id, "Формат: /remove [сорт]\nПример: /remove Лагер");
            return;
        }

        tracker.remove(name);
        bot.getApi().sendMessage(message->chat->id, "🗑 Сорт '" + name + "' полностью удален из базы.");
    });

    std::cout << "[Система] Бот успешно запущен. Жду сообщений..." << std::endl;
    while (true) {
        try {
            TgBot::TgLongPoll longPoll(bot);
            longPoll.start();
        } catch (const std::exception& e) {
            // Если сеть моргнула, бот напишет об этом, подождет и попробует снова
            std::cout << "[Сетевая ошибка] " << e.what() << "\n";
            std::cout << "[Система] Переподключение через 3 секунды...\n";
            sleep(3); // Пауза в 3 секунды перед следующей попыткой
        }
    }
    
    return 0;
}