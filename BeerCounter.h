#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <string> // Обязательно подключаем string

class BeerTracker{
    std::unordered_map<std::string, double> BeerMap;
    public:
        BeerTracker(){
            BeerMap["Лагер"] = 0.0;
            BeerMap["Пилснер"] = 0.0;
            BeerMap["Драфт"] = 0.0;
        }
        
        void add(const std::string& P, double L){
            BeerMap[P] += L;
        }
        
        void reset(const std::string& P){
            BeerMap[P] = 0.0;
        }
        
        void remove(const std::string& P){
            BeerMap.erase(P);
        }
        
        void subtract(const std::string& P, double L){
            // Используем std::max
            BeerMap[P] = std::max(BeerMap[P] - L, 0.0);
        }
        
        double getLiters(const std::string& P) const{
            auto it = BeerMap.find(P);
            if (it != BeerMap.end()){
                return it->second;
            }
            // Используем std::out_of_range
            throw std::out_of_range("Ошибка: Сорт пива '" + P + "' не найден в базе.");
        }
        
        void PrintInvent() const{
            for(const auto& [name, liters]: BeerMap){
                // Используем std::cout и std::endl
                std::cout << name << ": " << liters << " л" << std::endl;
            }
        }
        
        const auto& getMap() const{
            return BeerMap;
        }
        
        void setLiters(const std::string& P, double L){
            BeerMap[P] = L;
        }
};