#include <gtest/gtest.h>

#include "nodes.hxx"
#include "storage_types.hxx"
#include "package.hxx"
#include "factory.hxx"

TEST(FactoryTest, IsConsistentCorrect) {
    // R -> W -> S

    Factory factory;
    factory.add_ramp(Ramp(1, 1));
    factory.add_worker(Worker(1, 1, std::make_unique<PackageQueue>(PackageQueueType::FIFO)));
    factory.add_storehouse(Storehouse(1));

    Ramp& r = *(factory.find_ramp_by_id(1));
    r.receiver_preferences_.add_receiver(&(*factory.find_worker_by_id(1)));

    Worker& w = *(factory.find_worker_by_id(1));
    w.receiver_preferences_.add_receiver(&(*factory.find_storehouse_by_id(1)));

    EXPECT_TRUE(factory.is_consistent());
}

TEST(FactoryIOTest, ParseRamp) {
    std::istringstream iss("LOADING_RAMP id=1 delivery-interval=3");
    auto factory = load_factory_structure(iss);

    ASSERT_EQ(std::next(factory.ramp_cbegin(), 1), factory.ramp_cend());
    const auto& r = *(factory.ramp_cbegin());
    EXPECT_EQ(1, r.get_id());
    EXPECT_EQ(3, r.get_delivery_interval());
}

TEST(WorkerTest, HasBuffer) {
    // Test scenariusza opisanego na stronie:
    // http://home.agh.edu.pl/~mdig/dokuwiki/doku.php?id=teaching:programming:soft-dev:topics:net-simulation:part_nodes#bufor_aktualnie_przetwarzanego_polproduktu

    Worker w(1, 2, std::make_unique<PackageQueue>(PackageQueueType::FIFO));
    Time t = 1;

    w.receive_package(Package(1));
    w.do_work(t);
    ++t;
    w.receive_package(Package(2));
    w.do_work(t);
    auto& buffer = w.get_sending_buffer();

    ASSERT_TRUE(buffer.has_value());
    EXPECT_EQ(buffer.value().get_id(), 1);
}

