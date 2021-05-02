//
//  tests.cpp
//  Vector4D
//
//  Created by Alexander Ignatov on 8.04.21.
//

#include <Vector4D.hpp>
#include <catch.hpp>
#include <iostream>
#include <pollyfills.hpp>
#include <string>

bool areEqual(double d1, double d2)
{
    const double diff = d1 - d2;
    const double tolerance = 0.0001;
    return -tolerance <= diff && diff <= tolerance;
}

TEST_CASE("Operator [] set and get:0.1")
{
    double x = 2;
    double y = 4;
    double z = 8;
    double t = 16;
    Vector4D v(0, 0, 0, 0);

    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = t;

    REQUIRE(areEqual(v[0], x));
    REQUIRE(areEqual(v[1], y));
    REQUIRE(areEqual(v[2], z));
    REQUIRE(areEqual(v[3], t));
}

TEST_CASE("Operator +:0.1")
{
    double x = 1;
    double y = 2;
    double z = 3;
    double t = 4;
    const Vector4D v1(x, y, z, t);
    const Vector4D v2(t, z, y, x);
    double expect = 5;

    Vector4D result = v1 + v2;

    REQUIRE(areEqual(result[0], expect));
    REQUIRE(areEqual(result[1], expect));
    REQUIRE(areEqual(result[2], expect));
    REQUIRE(areEqual(result[3], expect));
}

TEST_CASE("Binary operator -:0.1")
{
    double x = 1;
    double y = 2;
    double z = 3;
    double t = 4;
    const Vector4D v1(x, y, z, t);
    const Vector4D v2(x, y, z, t);
    double expect = 0;

    Vector4D result = v1 - v2;

    REQUIRE(areEqual(result[0], expect));
    REQUIRE(areEqual(result[1], expect));
    REQUIRE(areEqual(result[2], expect));
    REQUIRE(areEqual(result[3], expect));
}

TEST_CASE("Unary operator -:0.1")
{
    double x = 1;
    double y = 2;
    double z = 3;
    double t = 4;
    const Vector4D v(x, y, z, t);

    Vector4D result = -v;

    REQUIRE(areEqual(result[0], -x));
    REQUIRE(areEqual(result[1], -y));
    REQUIRE(areEqual(result[2], -z));
    REQUIRE(areEqual(result[3], -t));
}

TEST_CASE("Operator * dot product:0.1")
{
    double x = 2;
    double y = 4;
    double z = 8;
    double t = 16;
    const Vector4D v1(x, y, z, t);
    const Vector4D v2(t, z, y, x);
    double expect = 32;

    Vector4D result = v1 * v2;

    REQUIRE(areEqual(result[0], expect));
    REQUIRE(areEqual(result[1], expect));
    REQUIRE(areEqual(result[2], expect));
    REQUIRE(areEqual(result[3], expect));
}

TEST_CASE("Operator * scalar:0.1")
{
    double x = 2;
    double y = 4;
    double z = 8;
    double t = 16;
    double pr = 10;
    const Vector4D v1(x, y, z, t);

    Vector4D result = v1 * pr;

    REQUIRE(areEqual(result[0], x * pr));
    REQUIRE(areEqual(result[1], y * pr));
    REQUIRE(areEqual(result[2], z * pr));
    REQUIRE(areEqual(result[3], t * pr));
}

TEST_CASE("Operator /:0.1")
{
    double x = 2;
    double y = 4;
    double z = 8;
    double t = 16;
    const Vector4D v1(x, y, z, t);
    const Vector4D v2(x, y, z, t);
    double expect = 1;

    Vector4D result = v1 / v2;

    REQUIRE(areEqual(result[0], expect));
    REQUIRE(areEqual(result[1], expect));
    REQUIRE(areEqual(result[2], expect));
    REQUIRE(areEqual(result[3], expect));
}

TEST_CASE("Operator +=:0.1")
{
    double x = 1;
    double y = 2;
    double z = 3;
    double t = 4;
    Vector4D v1(x, y, z, t);
    const Vector4D v2(t, z, y, x);
    double expect = 5;

    Vector4D &result = v1 += v2;

    REQUIRE(areEqual(v1[0], expect));
    REQUIRE(areEqual(v1[1], expect));
    REQUIRE(areEqual(v1[2], expect));
    REQUIRE(areEqual(v1[3], expect));
    REQUIRE(&result == &v1);
}

TEST_CASE("Operator -=:0.1")
{
    double x = 1;
    double y = 2;
    double z = 3;
    double t = 4;
    Vector4D v1(x, y, z, t);
    const Vector4D v2(x, y, z, t);
    double expect = 0;

    Vector4D &result = v1 -= v2;

    REQUIRE(areEqual(v1[0], expect));
    REQUIRE(areEqual(v1[1], expect));
    REQUIRE(areEqual(v1[2], expect));
    REQUIRE(areEqual(v1[3], expect));
    REQUIRE(&result == &v1);
}

TEST_CASE("Operator *= dot product:0.1")
{
    double x = 2;
    double y = 4;
    double z = 8;
    double t = 16;
    Vector4D v1(x, y, z, t);
    const Vector4D v2(t, z, y, x);
    double expect = 32;

    Vector4D &result = v1 *= v2;

    REQUIRE(areEqual(v1[0], expect));
    REQUIRE(areEqual(v1[1], expect));
    REQUIRE(areEqual(v1[2], expect));
    REQUIRE(areEqual(v1[3], expect));
    REQUIRE(&result == &v1);
}

TEST_CASE("Operator *= scalar:0.1")
{
    double x = 2;
    double y = 4;
    double z = 8;
    double t = 16;
    double pr = 10;
    Vector4D v1(x, y, z, t);

    Vector4D &result = v1 *= pr;

    REQUIRE(areEqual(v1[0], x * pr));
    REQUIRE(areEqual(v1[1], y * pr));
    REQUIRE(areEqual(v1[2], z * pr));
    REQUIRE(areEqual(v1[3], t * pr));
    REQUIRE(&result == &v1);
}

TEST_CASE("Operator /=:0.1")
{
    double x = 2;
    double y = 4;
    double z = 8;
    double t = 16;
    Vector4D v1(x, y, z, t);
    const Vector4D v2(x, y, z, t);
    double expect = 1;

    Vector4D &result = v1 /= v2;

    REQUIRE(areEqual(v1[0], expect));
    REQUIRE(areEqual(v1[1], expect));
    REQUIRE(areEqual(v1[2], expect));
    REQUIRE(areEqual(v1[3], expect));
    REQUIRE(&result == &v1);
}

TEST_CASE("Operator ==:0.1")
{
    double x = 2;
    double y = 4;
    double z = 8;
    double t = 16;
    const Vector4D v1(x, y, z, t);
    const Vector4D v2(x, y, z, t);
    const Vector4D v3(x, x, x, x);

    bool result1 = v1 == v2;
    bool result2 = v1 == v3;

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
}

TEST_CASE("Operator !=:0.1")
{
    double x = 2;
    double y = 4;
    double z = 8;
    double t = 16;
    const Vector4D v1(x, y, z, t);
    const Vector4D v2(x, y, z, t);
    const Vector4D v3(x, x, x, x);

    bool result1 = v1 != v2;
    bool result2 = v1 != v3;

    REQUIRE(result1 == false);
    REQUIRE(result2 == true);
}

TEST_CASE("Operator <:0.1")
{
    const Vector4D v0(0, 0, 0, 0);
    const Vector4D v1(1, 2, 3, 4);
    const Vector4D v2(1, 2, 4, 8);

    bool result1 = v0 < v1;
    bool result2 = v1 < v2;
    bool result3 = v1 < v0;
    bool result4 = v2 < v1;
    bool result5 = v1 < v1;

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);
    REQUIRE(result4 == false);
    REQUIRE(result5 == false);
}

TEST_CASE("Operator >:0.1")
{
    const Vector4D v0(0, 0, 0, 0);
    const Vector4D v1(1, 2, 3, 4);
    const Vector4D v2(1, 2, 4, 8);

    bool result1 = v0 > v1;
    bool result2 = v1 > v2;
    bool result3 = v1 > v0;
    bool result4 = v2 > v1;
    bool result5 = v1 > v1;

    REQUIRE(result1 == false);
    REQUIRE(result2 == false);
    REQUIRE(result3 == true);
    REQUIRE(result4 == true);
    REQUIRE(result5 == false);
}

TEST_CASE("Operator <=:0.1")
{
    const Vector4D v0(0, 0, 0, 0);
    const Vector4D v1(1, 2, 3, 4);
    const Vector4D v2(1, 2, 4, 8);

    bool result1 = v0 <= v1;
    bool result2 = v1 <= v2;
    bool result3 = v1 <= v0;
    bool result4 = v2 <= v1;
    bool result5 = v1 <= v1;

    REQUIRE(result1 == true);
    REQUIRE(result2 == true);
    REQUIRE(result3 == false);
    REQUIRE(result4 == false);
    REQUIRE(result5 == true);
}

TEST_CASE("Operator >=:0.1")
{
    const Vector4D v0(0, 0, 0, 0);
    const Vector4D v1(1, 2, 3, 4);
    const Vector4D v2(1, 2, 4, 8);

    bool result1 = v0 >= v1;
    bool result2 = v1 >= v2;
    bool result3 = v1 >= v0;
    bool result4 = v2 >= v1;
    bool result5 = v1 >= v1;

    REQUIRE(result1 == false);
    REQUIRE(result2 == false);
    REQUIRE(result3 == true);
    REQUIRE(result4 == true);
    REQUIRE(result5 == true);
}

TEST_CASE("Operator !:0.1")
{
    const Vector4D v0(0, 0, 0, 0);
    const Vector4D v1(1, 2, 3, 4);

    bool result1 = !v0;
    bool result2 = !v1;

    REQUIRE(result1 == true);
    REQUIRE(result2 == false);
}
