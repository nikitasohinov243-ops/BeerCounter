#include "BeerCounter.h"
#include <string>
#include <fstream>
#include <stdexcept>

class BeerFileManager {
    public:
        BeerFileManager() {};

        void saved(const BeerTracker& tracker, const std::string& filename) {
            std::ofstream file(filename);
            if (!file.is_open()) throw std::runtime_error("Не удалось открыть файл для сохранения");
            
            for(const auto& [name, liter] : tracker.getMap()) {
                file << name << "," << liter << " \n";
            }
            file.close();
        }

        void load(BeerTracker& tracker, const std::string& filename) {
            std::ifstream file(filename);
            std::string line;
            
            if (!file.is_open()) throw std::runtime_error("Не удалось открыть файл для загрузки");
            
            while (std::getline(file, line)) {
                auto commaPos = line.find(",");
                if(commaPos != std::string::npos) {
                    std::string name = line.substr(0, commaPos);
                    int liters = std::stoi(line.substr(commaPos + 1));

                    tracker.setLiters(name, liters);
                }
            }
            file.close();
        }
};