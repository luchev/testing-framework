#include <bits/stdc++.h>

#include <VendingMachine.hpp>
#include <catch.hpp>
#include <drink.hpp>
#include <pollyfills.hpp>

TEST_CASE("Drink test:0.2", "[drink]") {
    Drink instance("some drink", 100, 0.5, 1.60);

    REQUIRE(strcmp(instance.get_name(), "some drink") == 0);

    instance.set_name("just a drink");
    REQUIRE(strcmp(instance.get_name(), "just a drink") == 0);

    REQUIRE(instance.get_calories() == 100);
    REQUIRE(instance.get_quantity() == 0.5);
    REQUIRE(instance.get_price() == 1.60);
}

TEST_CASE("Drink copy/operator= test:0.2", "[drink]") {
    Drink first("some drink", 100, 0.5, 1.60);
    Drink second(first);
    Drink third("some drink", 100, 0.5, 1.60);
    third = first;

    first.set_name("the og drink");

    REQUIRE(strcmp(first.get_name(), "the og drink") == 0);
    REQUIRE(strcmp(second.get_name(), "some drink") == 0);
    REQUIRE(strcmp(third.get_name(), "some drink") == 0);

    second.set_name("the new drink");
    third.set_name("the zoomer drink");

    REQUIRE(strcmp(first.get_name(), "the og drink") == 0);
    REQUIRE(strcmp(second.get_name(), "the new drink") == 0);
    REQUIRE(strcmp(third.get_name(), "the zoomer drink") == 0);
}

TEST_CASE("Vending machine - add drink:0.2", "[vending machine]") {
    VendingMachine instance;

    REQUIRE(instance.add_drink(Drink("cola", 250, 0.5, 1.80)));
    REQUIRE(instance.add_drink(Drink("water", 10, 0.5, 0.80)));
    REQUIRE(instance.add_drink(Drink("cola", 750, 1.5, 1.80)) == false);
}

TEST_CASE("Vending machine - buy drink:0.2", "[vending machine]") {
    VendingMachine instance;

    instance.add_drink(Drink("cola", 250, 0.5, 1.80));
    instance.add_drink(Drink("water", 10, 0.5, 0.80));
    instance.add_drink(Drink("Beer Kozel", 250, 0.5, 4.50));
    instance.add_drink(Drink("Beer Ariana", 250, 0.5, 1.00));

    REQUIRE(instance.buy_drink("cola", 1.80) == 0);
    REQUIRE(instance.buy_drink("cola", 1.80) == 2);

    REQUIRE(instance.buy_drink("water", 0.30) == 1);
    REQUIRE(instance.buy_drink("water", 0.80) == 0);
    REQUIRE(instance.buy_drink("water", 0.30) == 2);
}

TEST_CASE("Vending machine - get income:0.2", "[vending machine]") {
    VendingMachine instance;

    instance.add_drink(Drink("cola", 250, 0.5, 1.80));
    instance.add_drink(Drink("water", 10, 0.5, 0.80));
    instance.add_drink(Drink("Beer Kozel", 250, 0.5, 4.50));
    instance.add_drink(Drink("Beer Ariana", 250, 0.5, 1.00));

    instance.buy_drink("cola", 1.80);
    REQUIRE(instance.get_income() == 1.80);

    instance.buy_drink("cola", 1.80);
    REQUIRE(instance.get_income() == 1.80);

    instance.buy_drink("water", 0.30);
    REQUIRE(instance.get_income() == 2.10);

    instance.buy_drink("water", 0.80);
    REQUIRE(instance.get_income() - 2.90 < 0.001);  // Double comparison error

    instance.buy_drink("water", 0.30);
    REQUIRE(instance.get_income() - 2.90 < 0.001);  // Double comparison error
}

TEST_CASE("Vending machine - copy/operator= tests:0.2", "[vending machine]") {
    VendingMachine first;
    first.add_drink(Drink("cola", 250, 0.5, 1.80));
    first.add_drink(Drink("water", 10, 0.5, 0.80));
    first.add_drink(Drink("Beer Kozel", 250, 0.5, 4.50));

    VendingMachine second(first);
    VendingMachine third;

    second.buy_drink("water", 0.80);

    third.add_drink(Drink("foo", 250, 0.5, 1.00));
    third = first;
    third.buy_drink("foo", 1.00);

    first.buy_drink("cola", 2.00);

    REQUIRE(first.get_income() - 2.00 < 0.001);
    REQUIRE(second.get_income() - 0.80 < 0.001);
    REQUIRE(third.get_income() - 1.00 < 0.001);
}
