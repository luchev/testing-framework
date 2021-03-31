#include <bits/stdc++.h>

#include <pollyfills.hpp>


#include <Error.hpp>
#include <TestCase.hpp>
#include <TestSuite.hpp>
#include <catch.hpp>

TEST_CASE("Error::getType():0.1") {
    Error none = Error::newNone();
    REQUIRE(none.getType() == ErrorType::None);

    const char* msg = "message";

    Error build = Error::newBuildFailed(msg);
    REQUIRE(build.getType() == ErrorType::BuildFailed);

    Error assertion = Error::newFailedAssertion(msg);
    REQUIRE(assertion.getType() == ErrorType::FailedAssertion);

    Error warning = Error::newWarning(msg);
    REQUIRE(warning.getType() == ErrorType::Warning);
}

TEST_CASE("Error::hasMessage() type:0.1") {
    Error none = Error::newNone();
    REQUIRE(!none.hasMessage());

    const char* msg = "message";

    Error build = Error::newBuildFailed(msg);
    REQUIRE(build.hasMessage());

    Error assertion = Error::newFailedAssertion(msg);
    REQUIRE(assertion.hasMessage());

    Error warning = Error::newWarning(msg);
    REQUIRE(warning.hasMessage());
}

TEST_CASE("Error::getMessage() type:0.1") {
    const char* msg = "42message42";

    Error build = Error::newBuildFailed(msg);
    REQUIRE(strcmp(msg, build.getMessage()) == 0);

    Error assertion = Error::newFailedAssertion(msg);
    REQUIRE(strcmp(msg, assertion.getMessage()) == 0);

    Error warning = Error::newWarning(msg);
    REQUIRE(strcmp(msg, warning.getMessage()) == 0);
}

TEST_CASE("Error pointer redirection:0.1") {
    const char* msg = "message";

    Error build = Error::newBuildFailed(msg);
    REQUIRE(msg != build.getMessage());

    Error assertion = Error::newFailedAssertion(msg);
    REQUIRE(msg != assertion.getMessage());

    Error warning = Error::newWarning(msg);
    REQUIRE(msg != warning.getMessage());
}

TEST_CASE("Error copy constructor:0.1") {
    const char* msg = "message";

    Error build = Error::newBuildFailed(msg);
    Error buildCopy(build);
    REQUIRE(build.getMessage() != buildCopy.getMessage());
    REQUIRE(strcmp(buildCopy.getMessage(), msg) == 0);
    REQUIRE(buildCopy.getType() == ErrorType::BuildFailed);
    REQUIRE(buildCopy.hasMessage());
}

TEST_CASE("Error operator=:0.1") {
    const char* msg = "message";

    Error build = Error::newBuildFailed(msg);
    Error buildCopy = Error::newNone();
    buildCopy = build;
    REQUIRE(build.getMessage() != buildCopy.getMessage());
    REQUIRE(strcmp(buildCopy.getMessage(), msg) == 0);
    REQUIRE(buildCopy.getType() == ErrorType::BuildFailed);
    REQUIRE(buildCopy.hasMessage());
}

TEST_CASE("Error small static char[]:0.1") {
    std::string str;
    for (int i = 0; i < 424242; i++) {
        str.push_back('a');
    }

    Error build = Error::newBuildFailed(str.c_str());
    REQUIRE(strcmp(str.c_str(), build.getMessage()) == 0);
}

TEST_CASE("TestCase getName():0.1") {
    std::string name = "xXx_name69_xXx";
    TestCase x = TestCase(name, Error::newNone());
    REQUIRE(name == x.getName());
}

TEST_CASE("TestCase isPassing():0.1") {
    REQUIRE(TestCase("", Error::newNone()).isPassing());
    REQUIRE(TestCase("", Error::newWarning("")).isPassing());
    REQUIRE_FALSE(TestCase("", Error::newBuildFailed("")).isPassing());
    REQUIRE_FALSE(TestCase("", Error::newFailedAssertion("")).isPassing());
}

TEST_CASE("TestCase hasError():0.1") {
    REQUIRE_FALSE(TestCase("", Error::newNone()).hasError());
    REQUIRE(TestCase("", Error::newBuildFailed("")).hasError());
    REQUIRE(TestCase("", Error::newFailedAssertion("")).hasError());
    REQUIRE(TestCase("", Error::newWarning("")).hasError());
}

TEST_CASE("TestCase getErrorType():0.1") {
    REQUIRE(TestCase("", Error::newNone()).getErrorType() == ErrorType::None);
    REQUIRE(TestCase("", Error::newBuildFailed("")).getErrorType() == ErrorType::BuildFailed);
    REQUIRE(TestCase("", Error::newFailedAssertion("")).getErrorType() == ErrorType::FailedAssertion);
    REQUIRE(TestCase("", Error::newWarning("")).getErrorType() == ErrorType::Warning);

    REQUIRE_FALSE(TestCase("", Error::newNone()).getErrorType() == ErrorType::Warning);
}

TEST_CASE("TestCase getErrorMessage():0.1") {
    std::string msg = "__message__";
    Error build = Error::newBuildFailed(msg.c_str());
    REQUIRE(TestCase("", Error::newBuildFailed(msg.c_str())).getErrorMessage() == msg);
    REQUIRE(TestCase("", Error::newNone()).getErrorMessage() == std::string(""));
}

TEST_CASE("TestSuit get/setName():0.1") {
    TestSuite suite("suite221");
    REQUIRE(suite.getName() == std::string("suite221"));

    suite.setName("Suite774447");
    REQUIRE(suite.getName() == std::string("Suite774447"));
}

TEST_CASE("TestSuit filterPassing():0.1") {
    TestSuite suite("suite221");

    suite.add(TestCase("none", Error::newNone()));
    suite.add(TestCase("none2", Error::newNone()));
    suite.add(TestCase("build", Error::newBuildFailed("")));
    suite.add(TestCase("failed11", Error::newFailedAssertion("failed11")));
    suite.add(TestCase("failed22", Error::newFailedAssertion("failed22")));
    suite.add(TestCase("failed33", Error::newFailedAssertion("failed33")));
    suite.add(TestCase("warn", Error::newWarning("")));

    REQUIRE(suite.filterPassing().size() == 3);
}

TEST_CASE("TestSuit filterFailing():0.1") {
    TestSuite suite("suite221");

    suite.add(TestCase("none", Error::newNone()));
    suite.add(TestCase("none2", Error::newNone()));
    suite.add(TestCase("build", Error::newBuildFailed("")));
    suite.add(TestCase("failed11", Error::newFailedAssertion("failed11")));
    suite.add(TestCase("failed22", Error::newFailedAssertion("failed22")));
    suite.add(TestCase("failed33", Error::newFailedAssertion("failed33")));
    suite.add(TestCase("warn", Error::newWarning("")));

    REQUIRE(suite.filterFailing().size() == 4);
}

TEST_CASE("TestSuit filterByErrorType() None:0.1") {
    TestSuite suite("suite221");

    suite.add(TestCase("none", Error::newNone()));
    suite.add(TestCase("none2", Error::newNone()));
    suite.add(TestCase("build", Error::newBuildFailed("")));
    suite.add(TestCase("failed11", Error::newFailedAssertion("failed11")));
    suite.add(TestCase("failed22", Error::newFailedAssertion("failed22")));
    suite.add(TestCase("warn", Error::newWarning("")));

    auto noneErrors = suite.filterByErrorType(ErrorType::None);
    REQUIRE(noneErrors.size() == 2);

    std::set<std::string> namesExpected;
    namesExpected.insert("none");
    namesExpected.insert("none2");

    std::set<std::string> namesActual;
    for (auto& x : noneErrors) {
        namesActual.insert(x.getName());
    }

    REQUIRE(namesExpected == namesActual);
}

TEST_CASE("TestSuit filterByErrorType() FailedAssertion:0.1") {
    TestSuite suite("suite221");

    suite.add(TestCase("none", Error::newNone()));
    suite.add(TestCase("none2", Error::newNone()));
    suite.add(TestCase("build", Error::newBuildFailed("")));
    suite.add(TestCase("failed11", Error::newFailedAssertion("failed11")));
    suite.add(TestCase("failed22", Error::newFailedAssertion("failed22")));
    suite.add(TestCase("warn", Error::newWarning("")));

    auto failedAssert = suite.filterByErrorType(ErrorType::FailedAssertion);
    REQUIRE(failedAssert.size() == 2);

    std::set<std::string> namesExpected;
    namesExpected.insert("failed11");
    namesExpected.insert("failed22");

    std::set<std::string> namesActual;
    for (auto& x : failedAssert) {
        namesActual.insert(x.getName());
    }

    REQUIRE(namesExpected == namesActual);
}

TEST_CASE("TestSuit removeByErrorType():0.1") {
    TestSuite suite("suite221");

    suite.add(TestCase("none", Error::newNone()));
    suite.add(TestCase("none2", Error::newNone()));
    suite.add(TestCase("build", Error::newBuildFailed("")));
    suite.add(TestCase("failed11", Error::newFailedAssertion("failed11")));
    suite.add(TestCase("failed22", Error::newFailedAssertion("failed22")));
    suite.add(TestCase("warn", Error::newWarning("")));

    REQUIRE(suite.filterByErrorType(ErrorType::FailedAssertion).size() == 2);
    REQUIRE(suite.filterPassing().size() == 3);
    REQUIRE(suite.filterFailing().size() == 3);

    suite.removeByErrorType(ErrorType::FailedAssertion);
    REQUIRE(suite.filterByErrorType(ErrorType::FailedAssertion).size() == 0);
    REQUIRE(suite.filterPassing().size() == 3);
    REQUIRE(suite.filterFailing().size() == 1);
}
