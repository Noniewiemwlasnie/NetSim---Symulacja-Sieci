#include "gtest/gtest.h"
#include "factory.hxx"

TEST(FactoryTest, IsConsistent_SimplePath) {
    Factory f;
    f.add_ramp(Ramp(1, 1));
    f.add_storehouse(Storehouse(1));
    
    auto* ramp = &(*f.find_ramp_by_id(1));
    auto* store = &(*f.find_storehouse_by_id(1));
    ramp->receiver_preferences_.add_receiver(store);

    EXPECT_TRUE(f.is_consistent());
}

TEST(FactoryTest, IsNotConsistent_DeadEnd) {
    Factory f;
    f.add_ramp(Ramp(1, 1));

    EXPECT_FALSE(f.is_consistent());
}

TEST(FactoryTest, IsConsistent_ComplexPath) {
    Factory f;
    f.add_ramp(Ramp(1, 1));
    f.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    f.add_storehouse(Storehouse(1));

    auto* ramp = &(*f.find_ramp_by_id(1));
    auto* worker = &(*f.find_worker_by_id(1));
    auto* store = &(*f.find_storehouse_by_id(1));

    ramp->receiver_preferences_.add_receiver(worker);
    worker->receiver_preferences_.add_receiver(store);

    EXPECT_TRUE(f.is_consistent());
}

TEST(FactoryTest, IsNotConsistent_CycleLoop) {
    Factory f;
    f.add_ramp(Ramp(1, 1));
    f.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));

    auto* ramp = &(*f.find_ramp_by_id(1));
    auto* worker = &(*f.find_worker_by_id(1));

    ramp->receiver_preferences_.add_receiver(worker);
    worker->receiver_preferences_.add_receiver(worker);

    EXPECT_FALSE(f.is_consistent());
}