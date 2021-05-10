//
//  Created by Alexander Ignatov on 29.04.21.
//

#include <catch.hpp>

#include <Counter.hpp>
#include <LimitedCounter.hpp>
#include <TwowayCounter.hpp>
#include <LimitedTwowayCounter.hpp>

TEST_CASE("Counter constructors and getters:0.4") {
    const int a = 69;
    const unsigned b = 420;
    const int expectTotal1 = 0;
    const unsigned expectStep1 = 1;
    const int expectTotal2 = a;
    const unsigned expectStep2 = 1;
    const int expectTotal3 = a;
    const unsigned expectStep3 = b;
    
    const Counter instance1;
    const Counter instance2(a);
    const Counter instance3(a, b);
    const int resultTotal1 = instance1.getTotal();
    const unsigned resultStep1 = instance1.getStep();
    const int resultTotal2 = instance2.getTotal();
    const unsigned resultStep2 = instance2.getStep();
    const int resultTotal3 = instance3.getTotal();
    const unsigned resultStep3 = instance3.getStep();
    
    REQUIRE(expectTotal1 == resultTotal1);
    REQUIRE(expectStep1 == resultStep1);
    REQUIRE(expectTotal2 == resultTotal2);
    REQUIRE(expectStep2 == resultStep2);
    REQUIRE(expectTotal3 == resultTotal3);
    REQUIRE(expectStep3 == resultStep3);
}

TEST_CASE("Counter::increment:0.4") {
    const unsigned s = 5;
    const int x = 42;
    const int y = 1000;
    const int n = 101;
    Counter instance1;
    Counter instance2(-s, s);
    Counter instance3(x, y);
    const int expect1 = n;
    const int expect2 = n * s - s;
    const int expect3 = n * y + x;
        
    for (int i = 0; i < n; i++) {
        instance1.increment();
        instance2.increment();
        instance3.increment();
    }
    
    const int result1 = instance1.getTotal();
    const int result2 = instance2.getTotal();
    const int result3 = instance3.getTotal();
    
    REQUIRE(expect1 == result1);
    REQUIRE(expect2 == result2);
    REQUIRE(expect3 == result3);
}

TEST_CASE("LimitedCounter constructors and max:0.4") {
    const int m1 = 0;
    const int m2 = 88;
    const int m3 = -88;
    const int start = 69;
    const unsigned step = 420;
    const LimitedCounter instance1(m1);
    const LimitedCounter instance2(m2, start);
    const LimitedCounter instance3(m3, start, step);

    const int result1 = instance1.getMax();
    const int result1t = instance1.getTotal();
    const int result1s = instance1.getStep();
    const int result2 = instance2.getMax();
    const int result2t = instance2.getTotal();
    const int result2s = instance2.getStep();
    const int result3 = instance3.getMax();
    const int result3t = instance3.getTotal();
    const int result3s = instance3.getStep();
    
    REQUIRE(result1 == m1);
    REQUIRE(result1t == 0);
    REQUIRE(result1s == 1);
    REQUIRE(result2 == m2);
    REQUIRE(result2t == start);
    REQUIRE(result2s == 1);
    REQUIRE(result3 == m3);
    REQUIRE(result3t == start);
    REQUIRE(result3s == step);
}

TEST_CASE("LimitedCounter::increment:0.4") {
    LimitedCounter instance1 = 2;
    LimitedCounter instance2(10, 8, 5);
    LimitedCounter instance3(-20, -20, 200);
    int expect11 = 1;
    int expect12 = 2;
    int expect13 = 2;
    int expect2 = 8;
    int expect3 = -20;
    
    instance1.increment();
    int result11 = instance1.getTotal();
    instance1.increment();
    int result12 = instance1.getTotal();
    instance1.increment();
    int result13 = instance1.getTotal();
    
    instance2.increment();
    int result2 = instance2.getTotal();
    
    instance3.increment();
    int result3 = instance3.getTotal();
    
    REQUIRE(result11 == expect11);
    REQUIRE(result12 == expect12);
    REQUIRE(result13 == expect13);
    REQUIRE(result2 == expect2);
    REQUIRE(result3 == expect3);
}

TEST_CASE("TwowayCounter::decrement:0.4") {
    const unsigned s = 5;
    const int x = 42;
    const int y = 1000;
    const int n = 101;
    TwowayCounter instance1;
    TwowayCounter instance2(-s, s);
    TwowayCounter instance3(x, y);
    const int expect1 = -n;
    const int expect2 = -n * s - s;
    const int expect3 = -n * y + x;
        
    for (int i = 0; i < n; i++) {
        instance1.decrement();
        instance2.decrement();
        instance3.decrement();
    }
    
    const int result1 = instance1.getTotal();
    const int result2 = instance2.getTotal();
    const int result3 = instance3.getTotal();
    
    REQUIRE(expect1 == result1);
    REQUIRE(expect2 == result2);
    REQUIRE(expect3 == result3);
}

TEST_CASE("LimitedTwowayCounter constructors and getters:0.5") {
    const int m = 88;
    const int start = 69;
    const unsigned step = 420;
    const LimitedTwowayCounter instance1(-m, m);
    const LimitedTwowayCounter instance2(0, m, start);
    const LimitedTwowayCounter instance3(m, 2 * m, start, step);

    const int result1 = instance1.getMin();
    const int result1m = instance1.getMax();
    const int result1t = instance1.getTotal();
    const int result1s = instance1.getStep();
    const int result2 = instance2.getMin();
    const int result2m = instance2.getMax();
    const int result2t = instance2.getTotal();
    const int result2s = instance2.getStep();
    const int result3 = instance3.getMin();
    const int result3m = instance3.getMax();
    const int result3t = instance3.getTotal();
    const int result3s = instance3.getStep();
    
    REQUIRE(result1 == -m);
    REQUIRE(result1m == m);
    REQUIRE(result1t == 0);
    REQUIRE(result1s == 1);
    REQUIRE(result2 == 0);
    REQUIRE(result2m == m);
    REQUIRE(result2t == start);
    REQUIRE(result2s == 1);
    REQUIRE(result3 == m);
    REQUIRE(result3m == 2 * m);
    REQUIRE(result3t == start);
    REQUIRE(result3s == step);
}

TEST_CASE("LimitedTwowayCounter::decrement:0.5") {
    LimitedTwowayCounter instance1(2, 4, 4);
    LimitedTwowayCounter instance2(0, 10, 5, 10);
    LimitedTwowayCounter instance3(-20, -20, -20);
    int expect11 = 3;
    int expect12 = 2;
    int expect13 = 2;
    int expect14 = 3;
    int expect15 = 4;
    int expect16 = 4;
    int expect2 = 5;
    int expect3 = -20;
    
    instance1.decrement();
    int result11 = instance1.getTotal();
    instance1.decrement();
    int result12 = instance1.getTotal();
    instance1.decrement();
    int result13 = instance1.getTotal();
    instance1.increment();
    int result14 = instance1.getTotal();
    instance1.increment();
    int result15 = instance1.getTotal();
    instance1.increment();
    int result16 = instance1.getTotal();
    
    instance2.decrement();
    int result2 = instance2.getTotal();
    
    instance3.decrement();
    int result3 = instance3.getTotal();
    
    REQUIRE(expect11 == result11);
    REQUIRE(expect12 == result12);
    REQUIRE(expect13 == result13);
    REQUIRE(expect14 == result14);
    REQUIRE(expect15 == result15);
    REQUIRE(expect16 == result16);
    REQUIRE(expect2 == result2);
    REQUIRE(expect3 == result3);
}
