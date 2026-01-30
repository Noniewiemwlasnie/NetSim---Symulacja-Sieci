#include "gtest/gtest.h"
#include "package.hxx"
#include "storage_types.hxx"

TEST(PackageTest, IsIdUnique) {
    Package p1;
    Package p2;
    EXPECT_NE(p1.get_id(), p2.get_id());
}

TEST(PackageQueueTest, FifoOrder) {
    PackageQueue queue(PackageQueueType::FIFO);
    queue.push(Package(1));
    queue.push(Package(2));

    auto p1 = queue.pop();
    EXPECT_EQ(p1->get_id(), 1);

    auto p2 = queue.pop();
    EXPECT_EQ(p2->get_id(), 2);
}

TEST(PackageQueueTest, LifoOrder) {
    PackageQueue queue(PackageQueueType::LIFO);
    queue.push(Package(1));
    queue.push(Package(2));

    auto p1 = queue.pop();
    EXPECT_EQ(p1->get_id(), 2);

    auto p2 = queue.pop();
    EXPECT_EQ(p2->get_id(), 1);
}