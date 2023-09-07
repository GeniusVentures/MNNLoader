#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "IPFStream.hpp"

using namespace sgns::io;
using sgns::ipfs_lite::ipfs::IpfsBlockService;
using sgns::ipfs_lite::ipfs::CID;
using testing::Return;

// Mock class for IpfsBlockService
class MockIpfsBlockService : public IpfsBlockService {
public:
    MOCK_METHOD(outcome::result<CID>, set, (const CID&, const common::Buffer&), (override));
    MOCK_METHOD(outcome::result<common::Buffer>, get, (const CID&), (override));
};

// Test to check reading from an empty queue
// This test confirms that attempting to read from an empty queue returns 0 bytes read.
TEST(IPFSStreamDevice, ReadEmptyQueue) {
    auto mock_service = std::make_shared<MockIpfsBlockService>();
    IpfsStreamDevice device(mock_service);

    char buffer[128];
    auto size = device.read(buffer, 128);

    EXPECT_EQ(size, 0);
}

// Test to check reading from a non-empty queue
// This test confirms that reading from a non-empty queue works as expected.
TEST(IPFSStreamDevice, ReadFromQueue) {
    auto mock_service = std::make_shared<MockIpfsBlockService>();
    IpfsStreamDevice device(mock_service);

    common::Buffer readData({'H', 'e', 'l', 'l', 'o'});
    CID mockCID;

    EXPECT_CALL(*mock_service, get(mockCID)).WillOnce(Return(readData));

    device.cid_queue_.push(mockCID);

    char buffer[128];
    auto size = device.read(buffer, 128);

    EXPECT_EQ(size, readData.size());
    EXPECT_EQ(std::memcmp(buffer, readData.data(), readData.size()), 0);
}

// Test to check the write functionality
// This test confirms that the write functionality works as expected.
TEST(IPFSStreamDevice, WriteToQueue) {
    auto mock_service = std::make_shared<MockIpfsBlockService>();
    IpfsStreamDevice device(mock_service);

    CID mockCID;

    EXPECT_CALL(*mock_service, set(mockCID, testing::_)).WillOnce(Return(outcome::success()));

    char buffer[] = "World";
    auto size = device.write(buffer, 5);

    EXPECT_EQ(size, 5);
}

// Test to check if the read operation returns an error correctly
// This test confirms that the read function returns an error when the block service encounters an issue.
TEST(IPFSStreamDevice, ReadErrorHandling) {
    auto mock_service = std::make_shared<MockIpfsBlockService>();
    IpfsStreamDevice device(mock_service);

    CID mockCID;

    EXPECT_CALL(*mock_service, get(mockCID)).WillOnce(Return(outcome::failure()));

    device.cid_queue_.push(mockCID);

    char buffer[128];
    auto size = device.read(buffer, 128);

    EXPECT_EQ(size, -1);
}

// Test to check if the write operation returns an error correctly
// This test confirms that the write function returns an error (0) when the block service encounters an issue.
TEST(IPFSStreamDevice, WriteErrorHandling) {
    auto mock_service = std::make_shared<MockIpfsBlockService>();
    IpfsStreamDevice device(mock_service);

    CID mockCID;

    EXPECT_CALL(*mock_service, set(mockCID, testing::_)).WillOnce(Return(outcome::failure()));

    char buffer[] = "World";
    auto size = device.write(buffer, 5);

    EXPECT_EQ(size, 0);
}
