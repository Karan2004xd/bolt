#include <gtest/gtest.h>
#include "../include/database.hpp"
#include "../include/tick.hpp"
#include "../include/buffer_manager.hpp"
#include "../include/buffer.hpp"
#include "../include/state.hpp"

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

    db.Insert(Tick(100, 1.1, 1));
    db.Insert(Tick(100, 1.1, 1));
    db.Flush();

    EXPECT_EQ(db.storage_handler_->GetState()->GetActiveBuffer()->Size(), 2);
  }
};

TEST(DatabaseTest, ConstructorsTest) {
  DatabaseTest::constructor_test();
}

TEST(DatabaseTest, InsertTickTest) {
  DatabaseTest::insert_tick_test();
}
