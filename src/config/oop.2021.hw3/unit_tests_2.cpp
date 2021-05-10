//
//  Created by Zhechko Popov on 01.05.21.
//

#include <catch.hpp>
#include <vector>

#include <Request.hpp>
#include <LeavingRequest.hpp>
#include <PromotionRequest.hpp>
#include <Developer.hpp>
#include <TeamLead.hpp>

using namespace std;

TEST_CASE("Request_hierarchy_object_count_and_ID :0.25") {
    const int expectedCount = 3;
    const int expectedIDFirst = 1;
    const int expectedIDThird = 3;

    Request request("request", "sender");
    LeavingRequest leavingRequest("sender");
    PromotionRequest promotionRequest("seneder", 0);

    int resultCountLastCreated = promotionRequest.getCount();
    int resultCountFirstCreated = request.getCount();
    int resultIDFirst = request.getID();
    int resultIDThird = promotionRequest.getID();

    REQUIRE(expectedCount == resultCountLastCreated);
    REQUIRE(expectedCount == resultCountFirstCreated);
    REQUIRE(expectedIDFirst == resultIDFirst);
    REQUIRE(expectedIDThird == resultIDThird);
}

TEST_CASE("Request_hierarchy_send_functions_count_and_id:0.25") {
    Developer * developer = new Developer("developer");
    TeamLead * teamLead = new TeamLead("teamLead", 10000);
    teamLead->addDeveloperToTeam(developer, 2000);
    developer->sendPromotionRequest(200);
    developer->sendLeavingRequest();
    
    Developer * developerWithoutTeamLead = new Developer("developerWithoutTeamLead");
    developerWithoutTeamLead->sendPromotionRequest(200);
    developerWithoutTeamLead->sendLeavingRequest();

    Request request("message", "sender");
    
    const int expectedCount = 6;
    int resultCount = request.getCount();

    delete developer;
    delete teamLead;
    delete developerWithoutTeamLead;

    REQUIRE(expectedCount == resultCount);
}

TEST_CASE("Request_hierarchy_inheritance:0.5") {
    LeavingRequest leavingRequest("sender");
    PromotionRequest promotionRequest("seneder", 0);

    Request * rp_leavingRequest = dynamic_cast<Request *>(&leavingRequest);
    Request * rp_promotionRequest = dynamic_cast<Request *>(&promotionRequest);

    REQUIRE(rp_leavingRequest != nullptr);
    REQUIRE(rp_promotionRequest != nullptr);
}

TEST_CASE("Developer_hierarchy_inheritance:0.5") {
    Developer * developer = new Developer("developer");
    TeamLead * teamLead = new TeamLead("teamLead", 10000);

    Developer * dp_teamLead = dynamic_cast<TeamLead *>(teamLead);

    REQUIRE(dp_teamLead != nullptr);

    delete developer;
    delete teamLead;
}

TEST_CASE("Developer_salary:0.5") {
    const int expectedInitialSalary = 0;
    const int expectedInitialSalaryAfterChange = 2000;
    const int expectedSalaryAfterChange = 2000;
    
    Developer developer("developer");
    int resultInitialSalary = developer.getSalary();

    developer.setInitialSalary(2000);
    int resultInitialSalaryAfterChange = developer.getSalary();

    developer.setInitialSalary(4000);
    int resultSalaryAfterChange = developer.getSalary();

    REQUIRE(expectedInitialSalary == resultInitialSalary);
    REQUIRE(expectedInitialSalaryAfterChange == resultInitialSalaryAfterChange);
    REQUIRE(expectedSalaryAfterChange == resultSalaryAfterChange);
}

TEST_CASE("Developer_teamLead_initialization:0.5") {
    TeamLead * teamLead = new TeamLead("teamLead", 10000);
    Developer * developer = new Developer("developer");

    const TeamLead * resultGetTeamLeadNotAdded = developer->getTeamLead();

    REQUIRE(resultGetTeamLeadNotAdded == nullptr);

    teamLead->addDeveloperToTeam(developer, 2000);

    const TeamLead * resultGetTeamLeadAdded = developer->getTeamLead();

    REQUIRE(resultGetTeamLeadAdded == teamLead);

    teamLead->removeDeveloperFromTeam(developer->getName());

    const TeamLead * resultGetTeamLeadRemoved = developer->getTeamLead();

    REQUIRE(resultGetTeamLeadRemoved == nullptr);

    delete teamLead;
    delete developer;
}

TEST_CASE ("TeamLead_add_and_remove_developers_to_team:0.5") {
    TeamLead * teamLead = new TeamLead("teamLead", 10000);
    Developer * developer1 = new Developer("developer1");
    Developer * developer2 = new Developer("developer2");

    teamLead->addDeveloperToTeam(developer1, 3000);
    teamLead->addDeveloperToTeam(developer2, 5000);

    vector<Developer *> expectedTeam;
    expectedTeam.push_back(developer1);
    expectedTeam.push_back(developer2);

    REQUIRE(expectedTeam == teamLead->getTeam());

    teamLead->removeDeveloperFromTeam("developer2");
    
    expectedTeam.erase(expectedTeam.begin() + 1);

    REQUIRE(expectedTeam == teamLead->getTeam());

    delete teamLead;
    delete developer1;
    delete developer2;
}

TEST_CASE("TeamLead_increase_and_reduce_salaries:0.5") {
    TeamLead * teamLead = new TeamLead("teamLead", 10000);
    Developer * developer1 = new Developer("developer1");
    Developer * developer2 = new Developer("developer2");

    teamLead->addDeveloperToTeam(developer1, 3000);
    teamLead->addDeveloperToTeam(developer2, 5000);

    teamLead->increaseTeamSalariesBy(500);

    const double developer1IncreasedSalary = 3500;
    const double developer2IncreasedSalary = 5500;

    REQUIRE(developer1IncreasedSalary == developer1->getSalary());
    REQUIRE(developer2IncreasedSalary == developer2->getSalary());

    teamLead->decreaseTeamSalariesBy(600);

    const double developer1ReducedSalary = 2900;
    const double developer2ReducedSalary = 4900;

    REQUIRE(developer1ReducedSalary == developer1->getSalary());
    REQUIRE(developer2ReducedSalary == developer2->getSalary());

    delete teamLead;
    delete developer1;
    delete developer2;
}

TEST_CASE("TeamLead_fullill_leaving_requests:0.5") {
    TeamLead * teamLead = new TeamLead("teamLead", 10000);
    Developer * developer1 = new Developer("developer1");
    Developer * developer2 = new Developer("developer2");

    teamLead->addDeveloperToTeam(developer1, 3000);
    teamLead->addDeveloperToTeam(developer2, 5000);

    developer1->sendLeavingRequest();

    teamLead->fulfillLeavingRequests();

    vector<Developer *> expectedTeam;
    expectedTeam.push_back(developer2);

    REQUIRE(expectedTeam == teamLead->getTeam());

    delete teamLead;
    delete developer1;
    delete developer2;
}

TEST_CASE("TeamLead_fullill_promotion_requests:0.5") {
    TeamLead * teamLead = new TeamLead("teamLead", 10000);
    Developer * developer1 = new Developer("developer1");
    Developer * developer2 = new Developer("developer2");

    teamLead->addDeveloperToTeam(developer1, 3000);
    teamLead->addDeveloperToTeam(developer2, 5000);

    developer1->sendPromotionRequest(500);

    const double developer1IncreasedSalary = 3500;

    teamLead->fulfillPromotionRequests();

    REQUIRE(developer1IncreasedSalary == developer1->getSalary());

    delete teamLead;
    delete developer1;
    delete developer2;
}
