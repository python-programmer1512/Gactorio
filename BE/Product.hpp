#pragma once

#include <string>
#include <vector>

class Product {
private:
    std::string name;
    int caffeineMg;
    int sugarGram;
    int volumeMl;
    bool carbonated;
    std::vector<std::string> ingredients;

public:
    Product();

    Product(const std::string& name,
            int caffeineMg,
            int sugarGram,
            int volumeMl,
            bool carbonated);

    void addIngredient(const std::string& ingredientName);

    std::string getName() const;
    int getCaffeineMg() const;
    int getSugarGram() const;
    int getVolumeMl() const;
    bool isCarbonated() const;
    std::vector<std::string> getIngredients() const;

    void setName(const std::string& name);
    void setCaffeineMg(int caffeineMg);
    void setSugarGram(int sugarGram);
    void setVolumeMl(int volumeMl);
    void setCarbonated(bool carbonated);

    std::string getInfo() const;
};
