#include <gtest/gtest.h>
#include <string>
#include "../include/forward_list.h"
#include "../include/forward_list_memory_resource.h"

struct TestStruct {
    int a;
    double b;
    std::string c;
    
    TestStruct(int a, double b, const std::string& c) : a(a), b(b), c(c) {}
    
    bool operator==(const TestStruct& other) const {
        return a == other.a && b == other.b && c == other.c;
    }
};

TEST(ForwardListTest, EmptyList) {
    forward_list_memory_resource mr(forward_list<int>::node_size(), 5);
    forward_list<int> list(&mr);
    
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST(ForwardListTest, PushFront) {
    forward_list_memory_resource mr(forward_list<int>::node_size(), 5);
    forward_list<int> list(&mr);
    
    list.push_front(42);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.front(), 42);
    
    list.push_front(100);
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), 100);
}

TEST(ForwardListTest, PopFront) {
    forward_list_memory_resource mr(forward_list<int>::node_size(), 5);
    forward_list<int> list(&mr);
    
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    
    EXPECT_EQ(list.front(), 3);
    list.pop_front();
    EXPECT_EQ(list.front(), 2);
    list.pop_front();
    EXPECT_EQ(list.front(), 1);
    list.pop_front();
    EXPECT_TRUE(list.empty());
}

TEST(ForwardListTest, Iterator) {
    forward_list_memory_resource mr(forward_list<int>::node_size(), 5);
    forward_list<int> list(&mr);
    
    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    
    auto it = list.begin();
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(it, list.end());
}

TEST(ForwardListTest, RangeBasedFor) {
    forward_list_memory_resource mr(forward_list<int>::node_size(), 5);
    forward_list<int> list(&mr);
    
    list.push_front(3);
    list.push_front(2);
    list.push_front(1);
    
    std::vector<int> result;
    for (int value : list) {
        result.push_back(value);
    }
    
    std::vector<int> expected = {1, 2, 3};
    EXPECT_EQ(result, expected);
}

TEST(ForwardListTest, ComplexStruct) {
    forward_list_memory_resource mr(forward_list<TestStruct>::node_size(), 5);
    forward_list<TestStruct> list(&mr);
    
    TestStruct s1(1, 2.5, "hello");
    TestStruct s2(3, 4.5, "world");
    
    list.push_front(s1);
    list.push_front(s2);
    
    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.front(), s2);
    
    auto it = list.begin();
    EXPECT_EQ(*it, s2);
    ++it;
    EXPECT_EQ(*it, s1);
}

TEST(MemoryResourceTest, ReuseMemory) {
    forward_list_memory_resource mr(sizeof(int), 3);
    
    void* block1 = mr.allocate(sizeof(int), alignof(int));
    mr.deallocate(block1, sizeof(int), alignof(int));
    
    void* block2 = mr.allocate(sizeof(int), alignof(int));
    
    // Память должна переиспользоваться
    EXPECT_EQ(block1, block2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}