#include <gtest/gtest.h>
#include "../CumBuffer.h"

//-----------------------------------------------------------------------------
TEST(Basic, AppendAndGet) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering; 
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.Init()); //default buffer length
    EXPECT_EQ  (buffering.GetCurHeadPos(), 0);
    EXPECT_EQ  (buffering.GetCurTailPos(), 0);
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_NO_DATA == buffering.GetData(3, dataOut));

    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"aaa", 3);
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.Append(3, data));
    EXPECT_EQ  (buffering.GetCurTailPos(), 3);

    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"bbb", 3);
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK ==buffering.Append(3, data));
    EXPECT_EQ  (buffering.GetCurTailPos(), 6);
    EXPECT_EQ  (buffering.GetCumulatedLen(), 6);

    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_INVALID_LEN == buffering.GetData(7, dataOut));
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK == buffering.GetData(6, dataOut));
    EXPECT_STREQ("aaabbb", dataOut);
    EXPECT_EQ   (buffering.GetCurHeadPos(), 6);
    EXPECT_EQ   (buffering.GetCurTailPos(), 6);
}

//-----------------------------------------------------------------------------
TEST(Basic, AppendAndGetLoop) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.Init(9)); 
    EXPECT_EQ( buffering.GetCurHeadPos(), 0);
    EXPECT_EQ( buffering.GetCurTailPos(), 0);

    for(int i =0; i < 30; i++)
    {
        memset(data, 0x00, sizeof(data));
        memcpy(data, (void*)"aaa", 3);
        ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.Append(3, data));

        memset(data, 0x00, sizeof(data));
        memcpy(data, (void*)"abbb", 4);
        ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.Append(4, data));
        EXPECT_EQ  (buffering.GetCumulatedLen(), 7);

        memset(dataOut, 0x00, sizeof(dataOut));
        ASSERT_TRUE (cumbuffer_defines::OP_RSLT_INVALID_LEN == buffering.GetData(8, dataOut));
        ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK == buffering.GetData(4, dataOut));
        EXPECT_STREQ("aaaa", dataOut);

        memset(dataOut, 0x00, sizeof(dataOut));
        ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK == buffering.GetData(3, dataOut));
        EXPECT_STREQ("bbb", dataOut);
    }
}

//-----------------------------------------------------------------------------
TEST(Exceptional, BufferFull) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.Init(10)); 

    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_INVALID_LEN == buffering.Append(11, data));
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_INVALID_LEN == buffering.Append(20, data));

    memcpy(data,(void*)"aaaaa", 5);
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.Append(5, data));

    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.PeekData(5, dataOut));
    EXPECT_EQ(strncmp("aaaaa", dataOut, 5 ), 0 ); 

    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_INVALID_LEN == buffering.GetData(10, dataOut));

    //-------- 
    memset(data, 0x00, sizeof(data));
    memcpy(data, (void*)"bbbbb", 5);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK == buffering.Append(5, data));
    
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.PeekData(10, dataOut));
    EXPECT_EQ(strncmp("aaaaabbbbb", dataOut, 10 ), 0 ); 
    
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);
    EXPECT_EQ   (buffering.GetCurTailPos(), 10);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_INVALID_LEN ==buffering.GetData(11, dataOut));
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_INVALID_LEN ==buffering.GetData(20, dataOut));

    //-------- append when full 
    memset(data, 0x00, sizeof(data));
    memcpy(data, (void*)"ccc", 3);
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_BUFFER_FULL == buffering.Append(3, data));
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);

    //-------- 
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK ==buffering.GetData(3, dataOut));
    EXPECT_STREQ("aaa", dataOut);
    EXPECT_EQ   (buffering.GetCurHeadPos(), 3);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 7);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_BUFFER_FULL == buffering.Append(4, data));
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_INVALID_LEN ==buffering.GetData(8, dataOut));

    //-------- 
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"ccc", 3);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK ==buffering.Append(3, data));

    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.PeekData(10, dataOut));
    EXPECT_EQ(strncmp("aabbbbbccc", dataOut, 10 ), 0 ); 

    EXPECT_EQ   (buffering.GetCurTailPos(), 3);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_INVALID_LEN ==buffering.GetData(11, dataOut));

    //-------- append when full 
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"ddd", 3);
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_BUFFER_FULL == buffering.Append(3, data));
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);

    //-------- 
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK ==buffering.GetData(3, dataOut));
    EXPECT_STREQ("aab", dataOut);
    EXPECT_EQ   (buffering.GetCurHeadPos(), 6);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 7);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_INVALID_LEN ==buffering.GetData(8, dataOut));

    //-------- 
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"ddd", 3);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK ==buffering.Append(3, data));
    EXPECT_EQ   (buffering.GetCurTailPos(), 6);
    EXPECT_EQ   (buffering.GetCurHeadPos(), 6);

    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.PeekData(10, dataOut));
    EXPECT_EQ(strncmp("bbbbcccddd", dataOut, 10 ), 0 ); 

    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_INVALID_LEN ==buffering.GetData(11, dataOut));

    //-------- append when full 
    memset(data, 0x00, sizeof(data));
    memcpy(data,(void*)"eeeeee", 6);
    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_BUFFER_FULL == buffering.Append(6, data));
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);

    //-------- 
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK ==buffering.GetData(6, dataOut));
    EXPECT_STREQ("bbbbcc", dataOut);
    EXPECT_EQ   (buffering.GetCurHeadPos(), 2);
    EXPECT_EQ   (buffering.GetCurTailPos(), 6);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 4);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_INVALID_LEN ==buffering.GetData(5, dataOut));

    //-------- 
    memset(data, 0x00, sizeof(data));
    memcpy(data,(void*)"eeeeee", 6);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK ==buffering.Append(6, data));

    ASSERT_TRUE(cumbuffer_defines::OP_RSLT_OK == buffering.PeekData(10, dataOut));
    EXPECT_EQ(strncmp("cdddeeeeee", dataOut, 10 ), 0 ); 

    EXPECT_EQ   (buffering.GetCurHeadPos(), 2);
    EXPECT_EQ   (buffering.GetCurTailPos(), 2);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_INVALID_LEN ==buffering.GetData(11, dataOut));

    //-------- 
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer_defines::OP_RSLT_OK ==buffering.GetData(10, dataOut));
    EXPECT_STREQ("cdddeeeeee", dataOut);
    EXPECT_EQ   (buffering.GetCurHeadPos(), 2);
    EXPECT_EQ   (buffering.GetCurTailPos(), 2);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 0);
}

//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

//g++ -L/usr/local/lib -lgtest -o test test.cpp 
//g++ -pthread -o test test.cpp /usr/local/lib/libgtest.a

