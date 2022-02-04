# CumBuffer #

### What ###

Accumulating byte buffer for c++. 


### Usage ###
Just copy CumBuffer.h to your project.

There are three buffer usage options : 
- auto growing buffer write, read
- fixed length buffer write, read
- direct buffer write, read

(see unit_test/test.cpp file)

---------------
***option 1 : auto growing buffer write, read***

If the buffer space runs out, it automatically doubles in size.

```cpp
#include "CumBuffer.h"

CumBuffer buffering;
char data[100];
char dataOut[100];
//create auto growing buffer with default 10 bytes
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
memset(data,0x00, sizeof(data));
memcpy(data,(void*)"xxxxx", 5);
ASSERT_TRUE (cumbuffer::OP_RSLT_OK ==buffering.Append(5, data));
//--- At this time, the buffer size is automatically doubled.
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
```
---------------
***option 2 : fixed length buffer write, read***

Fixed length buffer. If there is insufficient space, an buffer full error is returned.

```cpp
#include "CumBuffer.h"

CumBuffer buffering;
char data   [100];
char dataOut[100];
//create buffer with fixed 9 bytes
ASSERT_TRUE(cumbuffer::OP_RSLT_OK == buffering.Init(9)); 

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
---------------
***option 3 : direct buffer write, read***

This is useful if you want to minimize memory copying.

```cpp
#include "CumBuffer.h"

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
EXPECT_EQ  (buffering.GetCumulatedLen(),   0);
EXPECT_EQ  (buffering.GetTotalFreeSpace(), 10);
EXPECT_EQ  (buffering.GetLinearFreeSpace(),5); //XXX linear available space !
    
```

