//
// Created by lyubo on 9.03.21 г..
//

#include "VendingMachine.hpp"

void VendingMachine::copy_drinks(const Drink *src) {
    this->drinks = new Drink[this->capacity];

    for (int i = 0; i < this->count; i++) {
        this->drinks[i] = src[i];
    }
}
void VendingMachine::copy_vending_machine(const VendingMachine &src) {
    this->count = src.count;
    this->capacity = src.capacity;
    this->money = src.money;

    this->copy_drinks(src.drinks);
}

void VendingMachine::remove_from_machine(int index) {
    Drink* new_drinks = new Drink[this->capacity - 1];

    for(int i = 0; i < index; i++) {
        new_drinks[i] = this->drinks[i];
    }

    for(int i = index + 1; i < this->count; i++) {
        new_drinks[i - 1] = this->drinks[i];
    }

    delete[] this->drinks;
    this->drinks = new_drinks;
    this->count -= 1;
    this->capacity -= 1;
}

int VendingMachine::is_drink_in_machine(const char *name) {
    for (int i = 0; i < this->count; i++) {
        if (strcmp(name, this->drinks[i].get_name()) == 0) {
            return i;
        }
    }
    return -1;
}
VendingMachine::VendingMachine(): capacity(1), count(0), money(0.0) {
    this->drinks = new Drink[this->capacity];
}
VendingMachine::VendingMachine(const VendingMachine& from) {
    this->copy_vending_machine(from);
}
VendingMachine& VendingMachine::operator=(const VendingMachine& from) {
    if (this != &from) {
        delete[] this->drinks;
        this->copy_vending_machine(from);
    }
    return *this;
}
VendingMachine::~VendingMachine() {
    delete[] this->drinks;
}

bool VendingMachine::add_drink(const Drink& to_add) {
    if (this->is_drink_in_machine(to_add.get_name()) == -1) {
        if(this->count == this->capacity) {
            this->capacity *= 2;
            Drink* old_drinks = this->drinks;
            this->copy_drinks(this->drinks);
            delete[] old_drinks;
        }

        this->drinks[this->count] = to_add;
        this->count += 1;

        return true;
    }
    return false;
}
int VendingMachine::buy_drink(const char* drink_name, const double money) {
    int index = this->is_drink_in_machine(drink_name);

    if (index != -1) {
        if(this->drinks[index].get_price() <= money) {
            this->remove_from_machine(index);
            this->money += money;

            return 0;
        }
        this->money += money;
        return 1;
    }
    else {
        return 2;
    }
}

double VendingMachine::get_income() const {
    return this->money;
}
