# CumBuffer #

### What ###

accumulating byte buffer for c++. 

Let's take network programming as an example.
It is necessary to accumulate small pieces of data of arbitrary length to be received. We will process this data when a whole packet length is received. This is a small c ++ class for this case.

### Usage ###
just copy CumBuffer.h to your project.

see test.cpp
```cpp
#include "CumBuffer.h"

CumBuffer buffering;

if(cumbuffer::OP_RSLT_OK != buffering.Init(9)){ //create buffer with 9 bytes
    return false; 
} 
char data   [100];
char dataOut[100];

//append 3 bytes 
memset(data, 0x00, sizeof(data));
memcpy(data, (void*)"aaa", 3);
if(cumbuffer::OP_RSLT_OK != buffering.Append(3, data)){
    return false;
}

//append 4 bytes
memset(data, 0x00, sizeof(data));
memcpy(data, (void*)"abbb", 4);
if(cumbuffer::OP_RSLT_OK != buffering.Append(4, data)){
    return false;
}
if(buffering.GetCumulatedLen()!=7) {
    return false;
}

//get 4 bytes
memset(dataOut, 0x00, sizeof(dataOut));
if(cumbuffer::OP_RSLT_OK != buffering.GetData(4, dataOut)){
    return false;
}
if( strcmp("aaaa", dataOut)!=0){
    return false;
}

//get 3 bytes
memset(dataOut, 0x00, sizeof(dataOut));
if(cumbuffer::OP_RSLT_OK != buffering.GetData(3, dataOut)){
    return false;
}
if( strcmp("bbb", dataOut)!=0){
    return false;
}
```
#### Usage : direct buffer write, read ###

```cpp
    CumBuffer buffering;
    ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(10)); 
    EXPECT_EQ  (buffering.GetCurHeadPos(),     0);
    EXPECT_EQ  (buffering.GetCurTailPos(),     0);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),10);

    //direct write to buffer
    memcpy( buffering.GetLinearAppendPtr(),(void*)"abcde", 5);
    buffering.IncreaseData(5); //after write, IncreaseData should be called

    EXPECT_EQ  (buffering.GetCurHeadPos(),     0);
    EXPECT_EQ  (buffering.GetCurTailPos(),     5);
    EXPECT_EQ  (buffering.GetCumulatedLen(),   5);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 5);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),5);

    //direct access to read buffer (for no memcpy)
    EXPECT_EQ(strncmp("abcde", buffering.GetUnReadDataPtr(), 5 ), 0 ); 
    buffering.ConsumeData(5); //when buffer usage is finished, ConsumeData should be called

    EXPECT_EQ  (buffering.GetCurHeadPos(),     5);
    EXPECT_EQ  (buffering.GetCurTailPos(),     5);
    EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
    EXPECT_EQ  (buffering.GetLinearFreeSpace(),5); //XXX linear available space !
    EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
```

