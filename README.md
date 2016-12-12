# CumBuffer #

### What ###

accumulating buffer for c++ 

### When ###

if you want accumulate data comming from other source (from socket etc.) but don't want dynamic allocation for buffering.

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

