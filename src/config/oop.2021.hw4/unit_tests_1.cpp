#include <Averager.hpp>
#include <BacklogPublisher.hpp>
#include <Message.hpp>
#include <MovingAverager.hpp>
#include <PeriodicSampler.hpp>
#include <Repository.hpp>
#include <SimplePublisher.hpp>
#include <catch.hpp>

TEST_CASE("Averager:0.2") {
  Averager sub("test");
  auto result0 = sub.read();
  auto expected0 = 0;
  REQUIRE(result0 == expected0);

  sub.signal({2});
  auto result1 = sub.read();
  auto expected1 = 2;
  REQUIRE(result1 == expected1);

  sub.signal({4});
  auto result2 = sub.read();
  auto expected2 = 3;
  REQUIRE(result2 == expected2);

  Averager sub2("test2");
  int accumulator = 0;
  for (int i = 0; i < 125; i += 1) {
    sub2.signal(i);
    accumulator += i;
    auto result = sub2.read();
    auto expected = accumulator / (i + 1);
    REQUIRE(result == expected);
  }
}

TEST_CASE("Averager const correctness:0.1") {
  const Averager sub("test");
  auto result0 = sub.read();
  auto expected0 = 0;
  REQUIRE(result0 == expected0);
}

TEST_CASE("MovingAverager:0.3") {
  MovingAverager sub("test", 3);
  auto result0 = sub.read();
  auto expected0 = 0;
  REQUIRE(result0 == expected0);

  sub.signal({2});
  auto result1 = sub.read();
  auto expected1 = 2;
  REQUIRE(result1 == expected1);

  sub.signal({3});
  sub.signal({4});
  auto result2 = sub.read();
  auto expected2 = 3;
  REQUIRE(result2 == expected2);

  sub.signal({5});
  auto result3 = sub.read();
  auto expected3 = 4;
  REQUIRE(result3 == expected3);

  MovingAverager sub2("test2", 15);
  std::vector<int> data;
  for (int i = 0; i < 125; i += 1) {
    sub2.signal(i);

    data.push_back(i);
    if (data.size() > 15) {
      data.erase(data.begin());
    }
    auto accumulator = 0;
    for (auto x : data) {
      accumulator += x;
    }

    auto expected = (int)(accumulator / data.size());
    auto result = sub2.read();
    REQUIRE(result == expected);
  }
}

TEST_CASE("MovingAverager const correctness:0.1") {
  const MovingAverager sub("test", 24);
  auto result0 = sub.read();
  auto expected0 = 0;
  REQUIRE(result0 == expected0);
}

TEST_CASE("PeriodicSampler:0.2") {
  PeriodicSampler sub("test", 3);
  auto result0 = sub.read();
  auto expected0 = 0;
  REQUIRE(result0 == expected0);

  sub.signal({2});
  auto result1 = sub.read();
  auto expected1 = 2;
  REQUIRE(result1 == expected1);

  sub.signal({3});
  sub.signal({4});
  auto result2 = sub.read();
  auto expected2 = 2;
  REQUIRE(result2 == expected2);

  sub.signal({5});
  auto result3 = sub.read();
  auto expected3 = 5;
  REQUIRE(result3 == expected3);
}

TEST_CASE("PeriodicSampler const correctness:0.1") {
  const PeriodicSampler sub("test", 24);
  auto result0 = sub.read();
  auto expected0 = 0;
  REQUIRE(result0 == expected0);
}

TEST_CASE("SimplePublisher:0.5") {
  SimplePublisher sp;
  Averager sub("test");
  sp.subscribe(&sub);

  sp.signal({2});
  auto result1 = sub.read();
  auto expected1 = 2;
  REQUIRE(result1 == expected1);

  sp.signal({452});
  auto result2 = sub.read();
  auto expected2 = 227;
  REQUIRE(result2 == expected2);

  Averager sub2("test2");
  sp.subscribe(&sub2);
  Averager sub3("test3");
  sp.subscribe(&sub3);
  Averager sub4("test4");
  sp.subscribe(&sub4);

  sp.unsubscribe(&sub);

  for (int i = 141; i < 56252; i += 17) {
    sp.signal(i);
  }

  auto result4 = sub2.read();
  auto expected4 = 28191;
  REQUIRE(result4 == expected4);
  auto result5 = sub3.read();
  auto expected5 = 28191;
  REQUIRE(result5 == expected5);
  auto result6 = sub4.read();
  auto expected6 = 28191;
  REQUIRE(result6 == expected6);

  auto result7 = sub.read();
  auto expected7 = 227;
  REQUIRE(result7 == expected7);
}

TEST_CASE("BacklogPublisher:0.7") {
  BacklogPublisher bp;
  Averager sub("test");
  bp.subscribe(&sub);

  bp.signal({5641});
  auto result1 = sub.read();
  auto expected1 = 5641;
  REQUIRE(result1 == expected1);

  bp.signal({452});
  auto result2 = sub.read();
  auto expected2 = 3046;
  REQUIRE(result2 == expected2);

  Averager sub2("test2");
  bp.subscribe(&sub2);

  auto result3 = sub2.read();
  auto expected3 = 3046;
  REQUIRE(result3 == expected3);

  bp.unsubscribe(&sub);

  for (int i = 1418; i < 98127; i += 93) {
    bp.signal(i);
  }

  Averager sub3("test3");
  bp.subscribe(&sub3);

  auto result4 = sub3.read();
  auto expected4 = 49641;
  REQUIRE(result4 == expected4);

  auto result5 = sub.read();
  auto expected5 = 3046;
  REQUIRE(result5 == expected5);
}

TEST_CASE("Repository add/get:0.4") {
  Repository repo;
  auto result0 = repo.get("tt");
  auto expected0 = nullptr;
  REQUIRE(result0 == expected0);

  Averager sub("tt");
  repo.add(&sub);

  auto result1 = repo.get("tt");
  auto expected1 = &sub;
  REQUIRE(result1 != expected1);

  auto result2 = repo.get("tt");
  auto expected2 = nullptr;
  REQUIRE(result2 != expected2);

  MovingAverager msub("tt42", 12);
  repo.add(&msub);

  auto result3 = repo.get("tt42");
  auto expected3 = &msub;
  REQUIRE(result3 != expected3);

  auto result4 = repo.get("tt42");
  auto expected4 = nullptr;
  REQUIRE(result4 != expected4);

  PeriodicSampler sampler("ttsampler", 12);
  repo.add(&sampler);

  auto result5 = repo.get("ttsampler");
  auto expected5 = &msub;
  REQUIRE(result5 != expected5);

  auto result6 = repo.get("ttsampler");
  auto expected6 = nullptr;
  REQUIRE(result6 != expected6);

  auto avg = repo.get("tt");
  avg->signal({42});
  auto result7 = repo.get("tt")->read();
  auto expected7 = 42;
  REQUIRE(result7 == expected7);
}

TEST_CASE("Repository add/get polymorphism:0.2") {
  Repository repo;
  Averager sub("tt");
  repo.add(&sub);
  auto result1 = dynamic_cast<Averager *>(repo.get("tt"));
  auto expected1 = nullptr;
  REQUIRE(result1 != expected1);

  MovingAverager msub("tt42", 12);
  repo.add(&msub);
  auto result2 = dynamic_cast<MovingAverager *>(repo.get("tt42"));
  auto expected2 = nullptr;
  REQUIRE(result2 != expected2);

  PeriodicSampler sampler("ttsampler", 12);
  repo.add(&sampler);
  auto result3 = dynamic_cast<PeriodicSampler *>(repo.get("ttsampler"));
  auto expected3 = nullptr;
  REQUIRE(result3 != expected3);
}

TEST_CASE("Repository copy:0.2") {
  Repository repo;
  Averager sub("tt");
  repo.add(&sub);

  Repository repoCopy(repo);
  auto result1 = repoCopy.get("tt");
  auto expected1 = repo.get("tt");
  auto expectednu = nullptr;
  REQUIRE(result1 != expected1);
  REQUIRE(result1 != expectednu);
  REQUIRE(expected1 != expectednu);
}

TEST_CASE("Repository operator=:0.2") {
  Repository repo;
  Averager sub("tt");
  repo.add(&sub);

  Averager tmp("tmp");
  Repository repoCopy;
  repoCopy.add(&tmp);
  repoCopy = repo;

  auto result1 = repoCopy.get("tt");
  auto expected1 = repo.get("tt");
  auto expectednu = nullptr;
  REQUIRE(result1 != expected1);
  REQUIRE(result1 != expectednu);
  REQUIRE(expected1 != expectednu);

  auto result2 = repoCopy.get("tmp");
  auto expected2 = nullptr;
  REQUIRE(result2 == expected2);
}
