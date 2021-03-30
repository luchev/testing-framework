//
// Created by lyubo on 8.03.21 г..
//

#include <cstring>
#include "drink.hpp"

void Drink::copy_char(const char *src) {
    int len = strlen(src);
    this->name = new char[len + 1];

    strcpy(this->name, src);
    this->name[len] = '\0';
}

void Drink::copy_object(const Drink &src) {
    this->calories = src.calories;
    this->quantity = src.quantity;
    this->price = src.price;
    this->copy_char(src.name);
}
Drink::Drink(): calories(0), quantity(0.0), price(0.0) {
    this->name = new char[1];
    this->name[0] = '\0';
}

Drink::Drink(const char *init_name, const int init_calories, const double &init_quantity, const double &init_price) {
    this->calories = init_calories;
    this->quantity = init_quantity;
    this->price = init_price;
    this->copy_char(init_name);
    std::cout << this->name;
}
Drink::Drink(const Drink& from) {
    this->copy_object(from);
}
Drink& Drink::operator=(const Drink& from) {
    if (this != &from) {
        delete[] this->name;
        this->copy_object(from);
    }
    return *this;
}
Drink::~Drink() {
    delete[] this->name;
}

const char* Drink::get_name() const {
    return this->name;
}

int Drink::get_calories() const {
    return this->calories;
}
double Drink::get_quantity() const {
    return this->quantity;
}
double Drink::get_price() const {
    return this->price;
}

void Drink::set_name(const char *new_name) {
    delete[] this->name;
    this->copy_char(new_name);
}