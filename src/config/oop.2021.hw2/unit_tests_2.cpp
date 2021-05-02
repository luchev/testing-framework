//
//  tests.cpp
//  Homework 2
//
//  Created by Alexander Ignatov on 4.04.21.
//

#include <catch.hpp>
#include <iostream>
#include <pollyfills.hpp>
#include <sstream>
#include <string>

#include <ElectionResultsDatabase.hpp>
#include <SectionVotes.hpp>

const char* filename1 = "bulgaria.txt";
const char* filename2 = "abroad.txt";

TEST_CASE("SectionVotes smoke test:0.3") {
    int p1Expect = 42;
    int p2Expect = 69;
    int p3Expect = 88;

    SectionVotes instance(p1Expect, p2Expect, p3Expect);
    int p1Actual = instance.votesForParty(PARTY1);
    int p2Actual = instance.votesForParty(PARTY2);
    int p3Actual = instance.votesForParty(PARTY3);

    REQUIRE(p1Expect == p1Actual);
    REQUIRE(p2Expect == p2Actual);
    REQUIRE(p3Expect == p3Actual);
}

TEST_CASE("SectionVotes istream >> test:0.3") {
    std::stringstream stream;
    int p1Expect1 = 3;
    int p2Expect1 = 0;
    int p3Expect1 = 8;
    int p1Expect2 = 25;
    int p2Expect2 = 34;
    int p3Expect2 = 23;
    stream << p1Expect1 << ' ' << p2Expect1 << ' ' << p3Expect1
        << std::endl << p1Expect2 << ' ' << p2Expect2 << ' ' << p3Expect2;

    SectionVotes instance1, instance2;
    stream >> instance1 >> instance2;

    REQUIRE(instance1.votesForParty(PARTY1) == p1Expect1);
    REQUIRE(instance1.votesForParty(PARTY2) == p2Expect1);
    REQUIRE(instance1.votesForParty(PARTY3) == p3Expect1);
    REQUIRE(instance2.votesForParty(PARTY1) == p1Expect2);
    REQUIRE(instance2.votesForParty(PARTY2) == p2Expect2);
    REQUIRE(instance2.votesForParty(PARTY3) == p3Expect2);
}

TEST_CASE("SectionVotes ostream << test:0.3") {
    int p1Expect = 2;
    int p2Expect = 0;
    int p3Expect = 333;
    SectionVotes instance(p1Expect, p2Expect, p3Expect);
    std::stringstream stream;

    stream << instance << std::endl << instance;

    int p1Actual1, p2Actual1, p3Actual1;
    int p1Actual2, p2Actual2, p3Actual2;
    stream >> p1Actual1 >> p2Actual1 >> p3Actual1
           >> p1Actual2 >> p2Actual2 >> p3Actual2;

    REQUIRE(p1Actual1 == p1Expect);
    REQUIRE(p2Actual1 == p2Expect);
    REQUIRE(p3Actual1 == p3Expect);
    REQUIRE(p1Actual2 == p1Expect);
    REQUIRE(p2Actual2 == p2Expect);
    REQUIRE(p3Actual2 == p3Expect);
}

TEST_CASE("Database add from 1 file test:0.3") {
    ElectionResultsDatabase instance;
    const char* filename = filename1;
    int numberOfLines = 100;

    instance.addResultsFromFile(filename);

    REQUIRE(instance.numberOfSections() == numberOfLines);
}

TEST_CASE("Database add from 2 files test:0.3") {
    ElectionResultsDatabase instance;
    int numberOfLinesTotal = 128;

    instance.addResultsFromFile(filename1);
    instance.addResultsFromFile(filename2);

    REQUIRE(instance.numberOfSections() == numberOfLinesTotal);
}

TEST_CASE("Database add from inexisting file test:0.3") {
    ElectionResultsDatabase instance;

    instance.addResultsFromFile("inexistingfile.kon");

    REQUIRE(instance.numberOfSections() == 0);
}

TEST_CASE("Database sum votes test:0.3") {
    ElectionResultsDatabase instance;
    const char* filename = filename2;
    int p1Expect = 781;
    int p2Expect = 1224;
    int p3Expect = 1604;

    instance.addResultsFromFile(filename);

    REQUIRE(instance.votesForParty(PARTY1) == p1Expect);
    REQUIRE(instance.votesForParty(PARTY2) == p2Expect);
    REQUIRE(instance.votesForParty(PARTY3) == p3Expect);
}

TEST_CASE("Database winning party test:0.3") {
    ElectionResultsDatabase instance1;
    ElectionResultsDatabase instance2;
    Party expect1 = PARTY1;
    Party expect2 = PARTY3;
    Party expect3 = PARTY3;

    instance1.addResultsFromFile(filename1);
    Party result1 = instance1.winningParty();
    instance2.addResultsFromFile(filename2);
    Party result2 = instance2.winningParty();
    instance1.addResultsFromFile(filename2);
    Party result3 = instance1.winningParty();

    REQUIRE(result1 == expect1);
    REQUIRE(result2 == expect2);
    REQUIRE(result3 == expect3);
}

TEST_CASE("Database istream >> test:0.3")
{
    ElectionResultsDatabase instance;
    std::stringstream stream1("1 1 1\n2 2 2\n");
    std::stringstream stream2("3 3 3\n4 4 4\n5 5 5\n");

    stream1 >> instance;
    int result1 = instance.numberOfSections();
    stream2 >> instance;
    int result2 = instance.numberOfSections();

    REQUIRE(result1 == 2);
    REQUIRE(result2 == 5);
}

TEST_CASE("Database ostream << test:0.3") {
    ElectionResultsDatabase instance;
    std::stringstream instream("1 1 1\n2 2 2\n3 3 3\n");
    std::stringstream outstream;

    instream >> instance;
    outstream << instance;

    SectionVotes s1, s2, s3;
    outstream >> s1 >> s2 >> s3;

    REQUIRE(s1.votesForParty(PARTY1) == 1);
    REQUIRE(s1.votesForParty(PARTY2) == 1);
    REQUIRE(s1.votesForParty(PARTY3) == 1);
    REQUIRE(s2.votesForParty(PARTY1) == 2);
    REQUIRE(s2.votesForParty(PARTY2) == 2);
    REQUIRE(s2.votesForParty(PARTY3) == 2);
    REQUIRE(s3.votesForParty(PARTY1) == 3);
    REQUIRE(s3.votesForParty(PARTY2) == 3);
    REQUIRE(s3.votesForParty(PARTY3) == 3);
}
