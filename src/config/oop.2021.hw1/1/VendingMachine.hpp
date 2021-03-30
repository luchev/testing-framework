//
// Created by lyubo on 9.03.21 г..
//

#ifndef SOLUTION_VENDING_MACHINE_HPP
#define SOLUTION_VENDING_MACHINE_HPP

#include "drink.hpp"
#include <cstring>

class VendingMachine {
public:
    VendingMachine();
    VendingMachine(const VendingMachine& from);
    VendingMachine& operator=(const VendingMachine& from);
    ~VendingMachine();

    bool add_drink(const Drink& to_add);
    int buy_drink(const char* drink_name, const double money);

    double get_income() const;
private:
    void copy_drinks(const Drink* src);
    void copy_vending_machine(const VendingMachine& src);
    void remove_from_machine(int index);
    int is_drink_in_machine(const char* name);

    Drink* drinks;
    int count;
    int capacity;
    double money;
};


#endif //SOLUTION_VENDING_MACHINE_HPP
