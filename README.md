# CumBuffer #

### What ###

accumulating buffer for c++. 

Let's take network programming as an example.
It is necessary to accumulate small pieces of data of arbitrary length to be received. We will process this data when a whole packet length is received. This is a small c ++ class for this case.

### Usage ###

see test.cpp

    #include "CumBuffer.h"

    CumBuffer buffering;
    
    if(cumbuffer_defines::OP_RSLT_OK != buffering.Init(9)) //create buffer with 9 bytes
    {
        return false; 
    } 
    
    char data   [100];
    char dataOut[100];
    
    //append 3 bytes 
    memset(data, 0x00, sizeof(data));
    memcpy(data, (void*)"aaa", 3);
    if(cumbuffer_defines::OP_RSLT_OK != buffering.Append(3, data))
    {
        return false;
    }

    //append 4 bytes
    memset(data, 0x00, sizeof(data));
    memcpy(data, (void*)"abbb", 4);
    if(cumbuffer_defines::OP_RSLT_OK != buffering.Append(4, data))
    {
        return false;
    }

    if(buffering.GetCumulatedLen()!=7) 
    {
        return false;
    }

    //get 4 bytes
    memset(dataOut, 0x00, sizeof(dataOut));
    if(cumbuffer_defines::OP_RSLT_OK != buffering.GetData(4, dataOut))
    {
        return false;
    }

    if( strcmp("aaaa", dataOut)!=0)
    {
        return false;
    }
    
    //get 3 bytes
    memset(dataOut, 0x00, sizeof(dataOut));
    if(cumbuffer_defines::OP_RSLT_OK != buffering.GetData(3, dataOut))
    {
        return false;
    }

    if( strcmp("bbb", dataOut)!=0)
    {
        return false;
    }

### benchmark ###

https://gist.github.com/jeremyko/5ddd7796da25918962da0f6ad34e02ae
