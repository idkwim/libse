#include "concurrent/block.h"
#include "concurrent/encoder_c0.h"

#include "gtest/gtest.h"

using namespace se;

class TestEvent : public Event {
public:
  TestEvent(unsigned event_id) :
    Event(event_id, 0, Zone::unique_atom(), true, &TypeInfo<int>::s_type) {}

  smt::UnsafeTerm encode_eq(const ValueEncoder& encoder, Encoders& helper) const {
    return helper.constant(*this);
  }

  smt::UnsafeTerm constant(Encoders& helper) const { return helper.constant(*this); }
};

TEST(BlockTest, InsertEvents) {
  std::unique_ptr<Block> root_ptr(Block::make_root());
  EXPECT_NE(nullptr, root_ptr);

  const std::shared_ptr<Event> a(new TestEvent(1));
  const std::shared_ptr<Event> b(new TestEvent(2));
  const std::shared_ptr<Event> c(new TestEvent(3));
  const std::shared_ptr<Event> d(new TestEvent(4));

  std::forward_list<std::shared_ptr<Event>> event_ptrs;
  event_ptrs.push_front(c);
  event_ptrs.push_front(b);

  root_ptr->append(a);
  root_ptr->append_all(event_ptrs);
  root_ptr->append(d);

  std::forward_list<std::shared_ptr<Event>> body(root_ptr->body());
  EXPECT_EQ(a, body.front());
  body.pop_front();

  EXPECT_EQ(b, body.front());
  body.pop_front();

  EXPECT_EQ(c, body.front());
  body.pop_front();

  EXPECT_EQ(d, body.front());
  body.pop_front();

  EXPECT_TRUE(body.empty());
}

TEST(BlockTest, InsertAllEmpty) {
  std::unique_ptr<Block> root_ptr(Block::make_root());
  EXPECT_NE(nullptr, root_ptr);

  std::forward_list<std::shared_ptr<Event>> event_ptrs;

  root_ptr->append_all(event_ptrs);

  EXPECT_TRUE(root_ptr->body().empty());
}
