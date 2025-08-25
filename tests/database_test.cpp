#include <gtest/gtest.h>
#include "../include/database.hpp"
#include "../include/tick.hpp"

class DatabaseTest {
public:
  static auto constructor_test() -> void {
    auto db = Database();

    EXPECT_TRUE(db.thread_pool_ != nullptr);
    EXPECT_TRUE(db.storage_handler_ != nullptr);
    EXPECT_TRUE(db.data_buffer_ != nullptr);
  }

  static auto insert_tick_test() -> void {
    auto db = Database();

    for (int i = 0; i < 30000000; i++) {
      db.Insert(Tick(100, 1.1, 1));
    }
  }
};

TEST(DatabaseTest, ConstructorsTest) {
  DatabaseTest::constructor_test();
}

TEST(DatabaseTest, InsertTickTest) {
  DatabaseTest::insert_tick_test();
}
