//
// Created by lyubo on 8.04.21 г..
//

#include <Error.hpp>
#include <Optional.hpp>
#include <Result.hpp>
#include <catch.hpp>
#include <iostream>
#include <pollyfills.hpp>
#include <string>

TEST_CASE("Error default constructor:0.34", "[Error]") {
    Error instance;
    REQUIRE(instance.get_message().empty());
}

TEST_CASE("Error constructor with parameter:0.34", "[Error]") {
    Error instance("some error message");
    REQUIRE(instance.get_message() == "some error message");
}

TEST_CASE("Optional is_none():0.33", "[Optional]") {
    Optional<int> instance;
    REQUIRE(instance.is_none());

    Optional<double> second(5.2);
    REQUIRE_FALSE(second.is_none());
}

TEST_CASE("Optional get_value():0.33", "[Optional]") {
    Optional<std::string> first("Some text");

    REQUIRE("Some text" == first.get_value());

    Optional<int> second;  //By default int() returns 0
    REQUIRE(0 == second.get_value());
}

TEST_CASE("Result operator==, when Result is Error:0.33", "[Result]") {
    Result<int> first("Error message");
    Error temp;
    REQUIRE((first == temp) == true);

    Result<int> second(3);
    REQUIRE_FALSE((second == temp) == true);
}

TEST_CASE("Result operator==, when Result is not Error:0.33", "[Result]") {
    int temp = 3;
    Result<int> first("Error message");
    REQUIRE_FALSE((first == temp) == true);

    Result<int> second(3);
    REQUIRE((second == temp) == true);
}

TEST_CASE("Result get_result:0.33", "[Result]") {
    Result<int> first(3);
    Result<double> second(3.14);

    REQUIRE(3 == first.get_result().get_value());
    REQUIRE(3.14 - second.get_result().get_value() < 0.01);
}

TEST_CASE("Result get_error:0.33", "[Result]") {
    Result<int> first("Error message");
    REQUIRE("Error message" == first.get_error().get_value().get_message());
}

TEST_CASE("Optional default constructor + templates test:0.34", "[Optional]") {
    Optional<int> first;
    Optional<double> second;

    class Foo {
       public:
        Foo() = default;
    };

    Optional<Foo> third;
    REQUIRE(1 == 1);
}
