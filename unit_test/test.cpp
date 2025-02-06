#include "CumBuffer.h"
#include <gtest/gtest.h>

//-----------------------------------------------------------------------------
TEST(Basic, AppendAndGet) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering; 
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init()); //default buffer length
    EXPECT_EQ  (buffering.GetCurReadPos(), 0);
    EXPECT_EQ  (buffering.GetCurWritePos(), 0);
    ASSERT_TRUE(cumbuffer::OP_RSLT_NO_DATA == buffering.GetData(3, dataOut));

    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"aaa", 3);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(3, data));
    EXPECT_EQ  (buffering.GetCurWritePos(), 3);

    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"bbb", 3);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK ==buffering.Append(3, data));
    EXPECT_EQ  (buffering.GetCurWritePos(), 6);
    EXPECT_EQ  (buffering.GetCumulatedLen(), 6);

    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_INVALID_LEN == buffering.GetData(7, dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK == buffering.GetData(6, dataOut));
    EXPECT_STREQ("aaabbb", dataOut);
    EXPECT_EQ   (buffering.GetCurReadPos(), 6);
    EXPECT_EQ   (buffering.GetCurWritePos(), 6);


}

//-----------------------------------------------------------------------------
TEST(Basic, AppendAndGetLoop) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(9)); 
    EXPECT_EQ( buffering.GetCurReadPos(), 0);
    EXPECT_EQ( buffering.GetCurWritePos(), 0);

    for(int i =0; i < 30; i++)
    {
        memset(data, 0x00, sizeof(data));
        memcpy(data, (void*)"aaa", 3);
        ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(3, data));

        memset(data, 0x00, sizeof(data));
        memcpy(data, (void*)"abbb", 4);
        ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(4, data));
        EXPECT_EQ  (buffering.GetCumulatedLen(), 7);

        memset(dataOut, 0x00, sizeof(dataOut));
        ASSERT_TRUE (cumbuffer::OP_RSLT_INVALID_LEN == buffering.GetData(8, dataOut));
        ASSERT_TRUE (cumbuffer::OP_RSLT_OK == buffering.GetData(4, dataOut));
        EXPECT_STREQ("aaaa", dataOut);

        memset(dataOut, 0x00, sizeof(dataOut));
        ASSERT_TRUE (cumbuffer::OP_RSLT_OK == buffering.GetData(3, dataOut));
        EXPECT_STREQ("bbb", dataOut);
    }
}

//-----------------------------------------------------------------------------
TEST(Basic, LinearTest1) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering; 
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 
    EXPECT_EQ  (buffering.GetCapacity(),        10);
    EXPECT_EQ  (buffering.GetCurReadPos(),      0);
    EXPECT_EQ  (buffering.GetCurWritePos(),      0);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    0);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 10);

    // append 7
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"a.....b", 7);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(7, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),      0);
    EXPECT_EQ  (buffering.GetCurWritePos(),      7);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    7);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 3);

    // get 4
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK == buffering.GetData(4, dataOut));
    EXPECT_STREQ("a...", dataOut);
    EXPECT_EQ  (buffering.GetCurReadPos(),      4);
    EXPECT_EQ  (buffering.GetCurWritePos(),      7);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    3);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(),  7);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 3);

    // append 5
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"c***d",5);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(5, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),      4);
    EXPECT_EQ  (buffering.GetCurWritePos(),      2);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    8);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(),  2);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 2);

    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(8, dataOut));
    EXPECT_EQ(strncmp("..bc***d", dataOut, 8 ), 0 ); 
}

//-----------------------------------------------------------------------------
TEST(Basic, LinearTest2) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 
    EXPECT_EQ   (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ   (buffering.GetLinearFreeSpace(), 10);

    memcpy(data,(void*)"abcde", 5);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(5, data));
    EXPECT_EQ   (buffering.GetTotalFreeSpace(), 5);
    EXPECT_EQ   (buffering.GetLinearFreeSpace(), 5);

    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(5, dataOut));
    EXPECT_EQ(strncmp("abcde", dataOut, 5 ), 0 ); 
    EXPECT_EQ   (buffering.GetTotalFreeSpace(), 5);
    EXPECT_EQ   (buffering.GetLinearFreeSpace(), 5);

    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.ConsumeData(2));
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(3, dataOut));
    EXPECT_EQ(strncmp("cde", dataOut, 3 ), 0 ); 
    EXPECT_EQ   (buffering.GetTotalFreeSpace(), 7);
    EXPECT_EQ   (buffering.GetLinearFreeSpace(), 5);

    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.ConsumeData(2));
    EXPECT_EQ   (buffering.GetTotalFreeSpace(), 9);
    EXPECT_EQ   (buffering.GetLinearFreeSpace(), 5);
}

//-----------------------------------------------------------------------------
TEST(Basic, LinearTest3) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering; 
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 
    EXPECT_EQ  (buffering.GetCapacity(),        10);
    EXPECT_EQ  (buffering.GetCurReadPos(),      0);
    EXPECT_EQ  (buffering.GetCurWritePos(),      0);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    0);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 10);

    // append 3
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"aaa", 3);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(3, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),      0);
    EXPECT_EQ  (buffering.GetCurWritePos(),      3);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    3);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 7);

    // append 4
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"bbbb", 4);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(4, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),      0);
    EXPECT_EQ  (buffering.GetCurWritePos(),      7);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    7);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 3);

    // append 3
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"ccc", 3);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(3, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),      0);
    EXPECT_EQ  (buffering.GetCurWritePos(),      10);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 0);

    // get 5
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK == buffering.GetData(5, dataOut));
    EXPECT_STREQ("aaabb", dataOut);
    EXPECT_EQ  (buffering.GetCurReadPos(),      5);
    EXPECT_EQ  (buffering.GetCurWritePos(),      10);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    5);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 5);

    // get 2
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK == buffering.GetData(2, dataOut));
    EXPECT_STREQ("bb", dataOut);
    EXPECT_EQ  (buffering.GetCurReadPos(),      7);
    EXPECT_EQ  (buffering.GetCurWritePos(),      10);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    3);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 7);

    // get 3
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK == buffering.GetData(3, dataOut));
    EXPECT_STREQ("ccc", dataOut);
    EXPECT_EQ  (buffering.GetCurReadPos(),      10);
    EXPECT_EQ  (buffering.GetCurWritePos(),      10);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    0);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 10);
    
    // append 3
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"ddd", 3);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(3, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),      10);
    EXPECT_EQ  (buffering.GetCurWritePos(),      3);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    3);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 7);

    // append 4
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"eeee", 4);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(4, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),      10);
    EXPECT_EQ  (buffering.GetCurWritePos(),      7);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    7);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 3);
    
    // get 4
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK == buffering.GetData(4, dataOut));
    EXPECT_STREQ("ddde", dataOut);
    EXPECT_EQ  (buffering.GetCurReadPos(),      4);
    EXPECT_EQ  (buffering.GetCurWritePos(),      7);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    3);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 3); //!!!

    // get 3
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK == buffering.GetData(3, dataOut));
    EXPECT_STREQ("eee", dataOut);
    EXPECT_EQ  (buffering.GetCurReadPos(),      7);
    EXPECT_EQ  (buffering.GetCurWritePos(),      7);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    0);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 3); //!!!
    
    // append 3
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"fff", 3);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(3, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),      7);
    EXPECT_EQ  (buffering.GetCurWritePos(),      10);
    EXPECT_EQ  (buffering.GetCumulatedLen(),    3);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(), 7);
}    

//-----------------------------------------------------------------------------
TEST(Basic, LinearTest4) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 

    memcpy(data,(void*)"abcde", 5);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(5, data));
    memcpy(data,(void*)"ghijk", 5);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(5, data));
    
    //buffer full
    EXPECT_EQ   (buffering.GetCurReadPos(), 0);
    EXPECT_EQ   (buffering.GetCurWritePos(), 10);

    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.GetData(10, dataOut));
    EXPECT_STREQ("abcdeghijk", dataOut);

    EXPECT_EQ   (buffering.GetCurReadPos(), 10);
    EXPECT_EQ   (buffering.GetCurWritePos(), 10);

    ASSERT_TRUE(cumbuffer::OP_RSLT_NO_DATA==buffering.GetData(1, dataOut));

    EXPECT_EQ   (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ   (buffering.GetLinearFreeSpace(),10); 
}

//-----------------------------------------------------------------------------
TEST(Basic, IncreaseData) 
{
    char dataOut[100];

    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 
    EXPECT_EQ   (buffering.GetTotalFreeSpace(), 10);

    memcpy( buffering.GetLinearAppendPtr(),(void*)"abcde", 5);

    buffering.IncreaseData(5);
    EXPECT_EQ   (buffering.GetTotalFreeSpace(), 5);

    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(5, dataOut));
    EXPECT_EQ(strncmp("abcde", dataOut, 5 ), 0 ); 
}

//-----------------------------------------------------------------------------
TEST(Basic, ConsumeData) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 
    memcpy(data,(void*)"abcde", 5);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(5, data));

    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(5, dataOut));
    EXPECT_EQ(strncmp("abcde", dataOut, 5 ), 0 ); 

    ASSERT_TRUE(cumbuffer::OP_RSLT_INVALID_LEN == buffering.ConsumeData(11));
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.ConsumeData(2));
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(3, dataOut));
    EXPECT_EQ(strncmp("cde", dataOut, 3 ), 0 ); 
    EXPECT_EQ   (buffering.GetCurReadPos(), 2);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 3);
}


//-----------------------------------------------------------------------------
TEST(Exceptional, BufferFull) 
{
    char data[100];
    char dataOut[100];

    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 

    ASSERT_TRUE(cumbuffer::OP_RSLT_INVALID_LEN == buffering.Append(11, data));
    ASSERT_TRUE(cumbuffer::OP_RSLT_INVALID_LEN == buffering.Append(20, data));

    memcpy(data,(void*)"aaaaa", 5);
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Append(5, data));

    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(5, dataOut));
    EXPECT_EQ(strncmp("aaaaa", dataOut, 5 ), 0 ); 

    ASSERT_TRUE(cumbuffer::OP_RSLT_INVALID_LEN == buffering.GetData(10, dataOut));

    //-------- 
    memset(data, 0x00, sizeof(data));
    memcpy(data, (void*)"bbbbb", 5);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK == buffering.Append(5, data));
    
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(10, dataOut));
    EXPECT_EQ(strncmp("aaaaabbbbb", dataOut, 10 ), 0 ); 
    
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);
    EXPECT_EQ   (buffering.GetCurWritePos(), 10);
    ASSERT_TRUE (cumbuffer::OP_RSLT_INVALID_LEN ==buffering.GetData(11, dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_INVALID_LEN ==buffering.GetData(20, dataOut));

    //-------- append when full 
    memset(data, 0x00, sizeof(data));
    memcpy(data, (void*)"ccc", 3);
    ASSERT_TRUE(cumbuffer::OP_RSLT_BUFFER_FULL == buffering.Append(3, data));
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);

    //-------- 
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.GetData(3, dataOut));
    EXPECT_STREQ("aaa", dataOut);
    EXPECT_EQ   (buffering.GetCurReadPos(), 3);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 7);
    ASSERT_TRUE (cumbuffer::OP_RSLT_BUFFER_FULL == buffering.Append(4, data));
    ASSERT_TRUE (cumbuffer::OP_RSLT_INVALID_LEN ==buffering.GetData(8, dataOut));

    //-------- 
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"ccc", 3);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(3, data));

    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(10, dataOut));
    EXPECT_EQ(strncmp("aabbbbbccc", dataOut, 10 ), 0 ); 

    EXPECT_EQ   (buffering.GetCurWritePos(), 3);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);
    ASSERT_TRUE (cumbuffer::OP_RSLT_INVALID_LEN ==buffering.GetData(11, dataOut));

    //-------- append when full 
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"ddd", 3);
    ASSERT_TRUE(cumbuffer::OP_RSLT_BUFFER_FULL == buffering.Append(3, data));
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);

    //-------- 
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.GetData(3, dataOut));
    EXPECT_STREQ("aab", dataOut);
    EXPECT_EQ   (buffering.GetCurReadPos(), 6);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 7);
    ASSERT_TRUE (cumbuffer::OP_RSLT_INVALID_LEN ==buffering.GetData(8, dataOut));

    //-------- 
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"ddd", 3);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(3, data));
    EXPECT_EQ   (buffering.GetCurWritePos(), 6);
    EXPECT_EQ   (buffering.GetCurReadPos(), 6);

    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(10, dataOut));
    EXPECT_EQ(strncmp("bbbbcccddd", dataOut, 10 ), 0 ); 

    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);
    ASSERT_TRUE (cumbuffer::OP_RSLT_INVALID_LEN ==buffering.GetData(11, dataOut));

    //-------- append when full 
    memset(data, 0x00, sizeof(data));
    memcpy(data,(void*)"eeeeee", 6);
    ASSERT_TRUE(cumbuffer::OP_RSLT_BUFFER_FULL == buffering.Append(6, data));
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);

    //-------- 
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.GetData(6, dataOut));
    EXPECT_STREQ("bbbbcc", dataOut);
    EXPECT_EQ   (buffering.GetCurReadPos(), 2);
    EXPECT_EQ   (buffering.GetCurWritePos(), 6);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 4);
    ASSERT_TRUE (cumbuffer::OP_RSLT_INVALID_LEN ==buffering.GetData(5, dataOut));

    //-------- 
    memset(data, 0x00, sizeof(data));
    memcpy(data,(void*)"eeeeee", 6);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(6, data));

    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(10, dataOut));
    EXPECT_EQ(strncmp("cdddeeeeee", dataOut, 10 ), 0 ); 

    EXPECT_EQ   (buffering.GetCurReadPos(), 2);
    EXPECT_EQ   (buffering.GetCurWritePos(), 2);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 10);
    ASSERT_TRUE (cumbuffer::OP_RSLT_INVALID_LEN ==buffering.GetData(11, dataOut));

    //-------- 
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.GetData(10, dataOut));
    EXPECT_STREQ("cdddeeeeee", dataOut);
    EXPECT_EQ   (buffering.GetCurReadPos(), 2);
    EXPECT_EQ   (buffering.GetCurWritePos(), 2);
    EXPECT_EQ   (buffering.GetCumulatedLen(), 0);
}
//-----------------------------------------------------------------------------
TEST(Exceptional, BufferFull2) 
{
    CumBuffer buffering;
    char data[100];
    char dataOut[100];
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    0);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),10);
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"123456789", 9);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(9, data));
    //---
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    9);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   9);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 1);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),1);
    //---
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.GetData(5, dataOut));
    EXPECT_STREQ("12345", dataOut);
    EXPECT_EQ  (buffering.GetCurReadPos(),     5);
    EXPECT_EQ  (buffering.GetCurWritePos(),    9);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   4);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 6);
    //--- write pos < read pos
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"abc", 3); 
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(3, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),     5);
    EXPECT_EQ  (buffering.GetCurWritePos(),    2);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   7);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 3);
    // |0123456789|
    // ------------
    // |bc   6789a|
    // |  w       |
    // |     r    |
    //
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(7, dataOut));
    EXPECT_EQ(strncmp("6789abc", dataOut, 7 ), 0 ); 
    //--- make buffer full 
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"xxxxx", 5);
    ASSERT_TRUE(cumbuffer::OP_RSLT_BUFFER_FULL == buffering.Append(5, data));
    EXPECT_EQ   (buffering.GetCumulatedLen(), 7);
}

//-----------------------------------------------------------------------------
TEST(Basic, DirectWriteGet) 
{
    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),     0);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),10);

    //direct write to buffer
    memcpy( buffering.GetLinearAppendPtr(),(void*)"abcde", 5);
    buffering.IncreaseData(5); //after write, IncreaseData should be called

    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),     5);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   5);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 5);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),5);

    //direct access to read buffer (for no memcpy)
    EXPECT_EQ(strncmp("abcde", buffering.GetUnReadDataPtr(), 5 ), 0 ); 
    buffering.ConsumeData(5); //when buffer usage is finished, ConsumeData should be called

    EXPECT_EQ  (buffering.GetCurReadPos(),     5);
    EXPECT_EQ  (buffering.GetCurWritePos(),     5);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),5); //XXX linear available space !
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
}

//-----------------------------------------------------------------------------
TEST(AutoGrowingBuffer, AutoGrow1) 
{
    CumBuffer buffering;
    char data[100];
    char dataOut[100];
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.InitAutoGrowing(10)); 
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    0);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),10);
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"abcdefghij", 10);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(10, data));
    //---
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    10);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   10);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 0);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),0);
    // |0123456789|
    // ------------
    // |abcdefghij|
    // |          w|
    // |r         |
    //--- auto grow
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"xxxxx", 5);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(5, data));
    // |01234567890123456789|
    // ----------------------
    // |abcdefghijxxxxx     |
    // |               w    |
    // |r                   |
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    15);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   15);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 5);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),5);
    //---
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(15, dataOut));
    EXPECT_EQ(strncmp("abcdefghijxxxxx", dataOut, 15 ), 0 ); 
}

//-----------------------------------------------------------------------------
TEST(AutoGrowingBuffer, AutoGrow2) 
{
    CumBuffer buffering;
    char data[100];
    char dataOut[100];
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.InitAutoGrowing(10)); 
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    0);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),10);
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"abcdefghi", 9);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(9, data));
    //---
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    9);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   9);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 1);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),1);
    //---
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.GetData(5, dataOut));
    EXPECT_STREQ("abcde", dataOut);
    EXPECT_EQ  (buffering.GetCurReadPos(),     5);
    EXPECT_EQ  (buffering.GetCurWritePos(),    9);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   4);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 6);
    //--- make buffer full. write pos < read pos
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"!!!!!!", 6); 
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(6, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),     5);
    EXPECT_EQ  (buffering.GetCurWritePos(),    5);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   10);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 0);
    // |0123456789|
    // ------------
    // |!!!!!fghi!|
    // |     w    |
    // |     r    |
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(10, dataOut));
    EXPECT_EQ(strncmp("fghi!!!!!!", dataOut, 10 ), 0 ); 
    //--- auto grow
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"?????", 5);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(5, data));
    // |01234567890123456789|
    // ----------------------
    // |fghi!!!!!!?????     |
    // |               w    |
    // |r                   |
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    15);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   15);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 5);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),5);
    //---
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(15, dataOut));
    EXPECT_EQ(strncmp("fghi!!!!!!?????", dataOut, 15 ), 0 ); 
}

//-----------------------------------------------------------------------------
TEST(AutoGrowingBuffer, AutoGrow3) 
{
    CumBuffer buffering;
    char data[100];
    char dataOut[100];
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.InitAutoGrowing(10)); 
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    0);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),10);
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"123456789", 9);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(9, data));
    //---
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    9);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   9);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 1);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),1);
    //---
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.GetData(5, dataOut));
    EXPECT_STREQ("12345", dataOut);
    EXPECT_EQ  (buffering.GetCurReadPos(),     5);
    EXPECT_EQ  (buffering.GetCurWritePos(),    9);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   4);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 6);
    //--- write pos < read pos
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"abc", 3); 
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(3, data));
    EXPECT_EQ  (buffering.GetCurReadPos(),     5);
    EXPECT_EQ  (buffering.GetCurWritePos(),    2);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   7);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 3);
    // |0123456789|
    // ------------
    // |bc   6789a|
    // |  w       |
    // |     r    |
    //
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(7, dataOut));
    EXPECT_EQ(strncmp("6789abc", dataOut, 7 ), 0 ); 
    //--- auto grow
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"xxxxx", 5);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(5, data));
    // |01234567890123456789|
    // ----------------------
    // |6789abcxxxxx        |
    // |            w       |
    // |r                   |
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    12);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   12);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 8);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),8);
    //---
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(12, dataOut));
    EXPECT_EQ(strncmp("6789abcxxxxx", dataOut, 12 ), 0 ); 
}

//-----------------------------------------------------------------------------
TEST(AutoGrowingBuffer, AutoGrow4) 
{
    CumBuffer buffering;
    char data[100];
    char dataOut[100];
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.InitAutoGrowing(10)); 
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    0);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),10);
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"abcdef", 6);
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(6, data));
    // |0123456789|
    // ------------
    // |abcdef    |
    // |      w   |
    // |r         |
    //---
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),    6);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   6);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 4);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),4);
    //---
    memset(dataOut, 0x00, sizeof(dataOut));
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.GetData(2, dataOut));
    EXPECT_STREQ("ab", dataOut);
    // |0123456789|
    // ------------
    // |abcdef    |
    // |      w   |
    // |  r       |
    EXPECT_EQ  (buffering.GetCurReadPos(),     2);
    EXPECT_EQ  (buffering.GetCurWritePos(),    6);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   4);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 6);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),4);
    //--- auto grow
    memset(data,0x00, sizeof(data));
    memcpy(data,(void*)"1234567", 7); 
    ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(7, data));
    // |01234567890123456789|
    // ----------------------
    // |abcdef1234567       |
    // |             w      |
    // |  r                 |
    EXPECT_EQ  (buffering.GetCurReadPos(),     2);
    EXPECT_EQ  (buffering.GetCurWritePos(),    13);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   11);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 9);
    //
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.PeekData(11, dataOut));
    EXPECT_EQ(strncmp("cdef1234567", dataOut, 11 ), 0 ); 
}


//-----------------------------------------------------------------------------
TEST(IncreaseBufferAndCopy, CopyData) {
    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),     0);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),10);

    //direct write to buffer
    memcpy( buffering.GetLinearAppendPtr(),(void*)"abcde", 5);
    buffering.IncreaseData(5); 

    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),     5);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   5);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 5);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),5);

    buffering.IncreaseBufferAndCopyExisting(20); 
    EXPECT_EQ  (buffering.GetCapacity(), 20);
    EXPECT_EQ  (buffering.GetCurReadPos(),     0);
    EXPECT_EQ  (buffering.GetCurWritePos(),     5);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   5);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 15);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),15);
    
    memcpy( buffering.GetLinearAppendPtr(),(void*)"1234512345xxxxx", 15);
    buffering.IncreaseData(15);

    EXPECT_EQ(strncmp("abcde1234512345xxxxx", buffering.GetUnReadDataPtr(), 20 ), 0 ); 
    buffering.ConsumeData(20); 

    EXPECT_EQ  (buffering.GetCurReadPos(),     20);
    EXPECT_EQ  (buffering.GetCurWritePos(),     20);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 20);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),20);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);

    buffering.IncreaseBufferAndCopyExisting(40); 
    EXPECT_EQ  (buffering.GetCapacity(), 40);
    EXPECT_EQ  (buffering.GetCurReadPos(),     20);
    EXPECT_EQ  (buffering.GetCurWritePos(),     20);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 40);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),20);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    //::testing::GTEST_FLAG(filter) = "Basic.IncreaseData";
    //::testing::GTEST_FLAG(filter) = "Basic.LinearFreeSpaceBoundaryCase";
    //::testing::GTEST_FLAG(filter) = "Basic.Calculations";
    //::testing::GTEST_FLAG(filter) = "Basic.DirectWriteGet";
    //::testing::GTEST_FLAG(filter) = "Exceptional.BufferFull";
    //::testing::GTEST_FLAG(filter) = "AutoGrowingBuffer.AutoGrow4";
    // ::testing::GTEST_FLAG(filter) = "IncreaseBufferAndCopy.CopyData";
    return RUN_ALL_TESTS();
}

