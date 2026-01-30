#include "gtest/gtest.h"
#include "nodes.hxx"

TEST(WorkerTest, ProcessingLogic) {
    TimeOffset processing_duration = 2;
    Worker w(1, processing_duration, std::make_unique<PackageQueue>(PackageQueueType::FIFO));
    
    w.receive_package(Package(1));
    
    w.do_work(1);
    ASSERT_TRUE(w.get_processing_buffer().has_value());
    EXPECT_EQ(w.get_processing_buffer()->get_id(), 1);
    EXPECT_FALSE(w.get_sending_buffer().has_value());

    w.do_work(2);
    ASSERT_TRUE(w.get_processing_buffer().has_value());
    EXPECT_FALSE(w.get_sending_buffer().has_value());

    w.do_work(3);
    EXPECT_FALSE(w.get_processing_buffer().has_value());
    ASSERT_TRUE(w.get_sending_buffer().has_value());
    EXPECT_EQ(w.get_sending_buffer()->get_id(), 1);
}