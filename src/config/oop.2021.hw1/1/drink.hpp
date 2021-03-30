//
// Created by lyubo on 8.03.21 г..
//

#ifndef SOLUTION_DRINK_HPP
#define SOLUTION_DRINK_HPP

#include <iostream>

class Drink {
public:
    Drink();
    Drink(const char* init_name, const int init_calories, const double& init_quantity, const double& init_price);
    Drink(const Drink&);
    Drink& operator=(const Drink&);
    ~Drink();

    const char* get_name() const;
    int get_calories() const;
    double get_quantity() const;
    double get_price() const;

    void set_name(const char* new_name);

    void copy_char(const char* src);
    void copy_object(const Drink& src);
private:
    char* name;
    int calories;
    double quantity;
    double price;
};


#endif //SOLUTION_DRINK_HPP
