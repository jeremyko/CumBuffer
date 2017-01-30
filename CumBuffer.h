#ifndef __CUMBUFFER_HPP__
#define __CUMBUFFER_HPP__
/****************************************************************************
 Copyright (c) 2016, ko jung hyun
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
// https://github.com/jeremyko/CumBuffer

// NO THREAD SAFETY HERE
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include <string.h>
#include <stdint.h>

//#define CUMBUFFER_DEBUG

namespace cumbuffer_defines
{
    const int DEFAULT_BUFFER_LEN = 1024 * 4;

    enum OP_RESULT
    {
        OP_RSLT_OK = 0,
        OP_RSLT_NO_DATA,
        OP_RSLT_BUFFER_FULL,
        OP_RSLT_ALLOC_FAILED,
        OP_RSLT_INVALID_LEN,
        OP_RSLT_INVALID_USAGE
    } ;
} ;

///////////////////////////////////////////////////////////////////////////////
class CumBuffer
{
  public:
    CumBuffer() { }

    virtual ~CumBuffer() { if(pBuffer_) { delete [] pBuffer_; } };

    //------------------------------------------------------------------------
    cumbuffer_defines::OP_RESULT    Init(int nMaxBufferLen = cumbuffer_defines::DEFAULT_BUFFER_LEN)
    {
        pBuffer_ = NULL;
        nCumulatedLen_=0;
        nCurHead_=0;
        nCurTail_=0;
        nBufferLen_ = nMaxBufferLen;

        try
        {
            pBuffer_ = new char [nBufferLen_];
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << "\n";
            return cumbuffer_defines::OP_RSLT_ALLOC_FAILED;
        }

        return cumbuffer_defines::OP_RSLT_OK;
    }

    //------------------------------------------------------------------------
    cumbuffer_defines::OP_RESULT    Append(size_t nLen, char* pData)
    {
#ifdef CUMBUFFER_DEBUG
        std::cout <<"\n["<< __func__ <<"]----------------  \n"; //debug
        std::cout <<"    nLen=" << nLen  << " [" << pData<< "] \n";
        DebugPos(__LINE__);
#endif

        if( nBufferLen_ < nLen )
        {
#ifdef CUMBUFFER_DEBUG
            std::cout << "    ln:" << __LINE__ << " / invalid len!\n"; //debug
#endif
            return cumbuffer_defines::OP_RSLT_INVALID_LEN;
        }
        else if( nBufferLen_ ==  nCumulatedLen_ )
        {
#ifdef CUMBUFFER_DEBUG
            std::cout << "    ln:" << __LINE__ << " / buffer full!\n"; //debug
#endif
            return cumbuffer_defines::OP_RSLT_BUFFER_FULL;
        }

        if(nCurTail_ < nCurHead_)
        {
            //tail 이 버퍼 끝을 지난 경우
            if(nCurHead_ - nCurTail_ >= nLen)
            {
                memcpy(pBuffer_ + nCurTail_, pData, nLen);
                nCurTail_ += nLen;
                nCumulatedLen_ += nLen;
#ifdef CUMBUFFER_DEBUG
                DebugPos(__LINE__);
#endif

                return cumbuffer_defines::OP_RSLT_OK;
            }
            else
            {
#ifdef CUMBUFFER_DEBUG
                std::cout << "    ln:" << __LINE__ << " / buffer full!\n"; //debug
#endif
                return cumbuffer_defines::OP_RSLT_BUFFER_FULL;
            }
        }
        else
        {
            //std::cout << "ln:" << __LINE__ << " / debug\n"; //debug
            if (nBufferLen_ < nCurTail_ + nLen)
            {
                //tail 이후, 남은 버퍼로 모자라는 경우
                if( nCurTail_ > 0 && 
                    nLen - (nBufferLen_ - nCurTail_)  <= nCurHead_ )
                {
                    //2번 나누어서 들어갈 공간이 있는 경우
#ifdef CUMBUFFER_DEBUG
                    std::cout <<"    nLen / nBufferLen_/ nCurTail_ : " 
                              << nLen << "/" << nBufferLen_<< "/"<< nCurTail_ << "\n";
#endif

                    int nFirstBlockLen = nBufferLen_ - nCurTail_;
                    int nSecondBlockLen = nLen - nFirstBlockLen;

#ifdef CUMBUFFER_DEBUG
                    std::cout << "    ln:" << __LINE__ << " / nFirstBlockLen ="<<nFirstBlockLen<<"\n"; //debug
                    std::cout << "    ln:" << __LINE__ << " / nSecondBlockLen="<<nSecondBlockLen<<"\n"; //debug
#endif

                    if(nFirstBlockLen>0)
                    {
                        memcpy(pBuffer_+ nCurTail_ , pData, nFirstBlockLen); 
                    }

                    memcpy(pBuffer_ , pData+(nFirstBlockLen), nSecondBlockLen); 

                    nCurTail_ = nSecondBlockLen;
                    nCumulatedLen_ += nLen;
#ifdef CUMBUFFER_DEBUG
                    DebugPos(__LINE__);
#endif
                    return cumbuffer_defines::OP_RSLT_OK;
                }
                else
                {
#ifdef CUMBUFFER_DEBUG
                    std::cout << "    ln:" << __LINE__ << " / buffer full!\n"; //debug
#endif
                    return cumbuffer_defines::OP_RSLT_BUFFER_FULL;
                }
            }
            else
            {
                //가장 일반적인 경우
                memcpy(pBuffer_+nCurTail_ , pData, nLen); //from start 
                nCurTail_ += nLen;
                nCumulatedLen_ += nLen;
#ifdef CUMBUFFER_DEBUG
                DebugPos(__LINE__);
#endif
                return cumbuffer_defines::OP_RSLT_OK;
            }
        }

        return cumbuffer_defines::OP_RSLT_OK;
    }

    //------------------------------------------------------------------------
    cumbuffer_defines::OP_RESULT    PeekData(size_t nLen, char* pDataOut)
    {
        return GetData(nLen, pDataOut, true, false);
    }

    //------------------------------------------------------------------------
    cumbuffer_defines::OP_RESULT    MoveCurHeader(size_t nLen)
    {
        //PeekData 사용해서 처리한 data length 만큼 버퍼내 nCurHead_ 를 이동.
        return GetData(nLen, NULL, false, true);
    }

    //------------------------------------------------------------------------
    cumbuffer_defines::OP_RESULT    GetData(size_t  nLen, 
                                            char*   pDataOut, 
                                            bool    bPeek=false, 
                                            bool    bMoveHeaderOnly=false)
    {
        if(bPeek && bMoveHeaderOnly)
        {
#ifdef CUMBUFFER_DEBUG
            std::cout << "    ln:" << __LINE__ << " / invalid usage\n"; //debug
#endif
            return cumbuffer_defines::OP_RSLT_INVALID_USAGE;
        }

#ifdef CUMBUFFER_DEBUG
        DebugPos(__LINE__);
#endif

        cumbuffer_defines::OP_RESULT nRslt = ValidateBuffer(nLen);
        if(cumbuffer_defines::OP_RSLT_OK!=nRslt )
        {
#ifdef CUMBUFFER_DEBUG
            DebugPos(__LINE__);
#endif
            return nRslt;
        }

        if(nCurTail_ > nCurHead_)
        {
            //일반적인 경우
            if (nCurTail_ < nCurHead_ + nLen)
            {
#ifdef CUMBUFFER_DEBUG
                std::cout << "    ln:" << __LINE__ << " / invalid request len:"<< nLen<<"\n"; //debug
#endif
                return cumbuffer_defines:: OP_RSLT_INVALID_LEN;
            }
            else
            {
                if(!bMoveHeaderOnly)
                {
                    memcpy(pDataOut, pBuffer_ + nCurHead_, nLen);
                }
                if(!bPeek)
                {
                    nCurHead_ += nLen;
                }
            }
        }
        else// if(nCurTail_ <= nCurHead_)
        {
            if (nBufferLen_ < nCurHead_ + nLen)
            {
                size_t nFirstBlockLen = nBufferLen_ - nCurHead_;
                size_t nSecondBlockLen = nLen - nFirstBlockLen;
#ifdef CUMBUFFER_DEBUG
                std::cout << "    ln:" << __LINE__ << " / nFirstBlockLen ="<<nFirstBlockLen<<"\n"; //debug
                std::cout << "    ln:" << __LINE__ << " / nSecondBlockLen="<<nSecondBlockLen<<"\n"; //debug
#endif

                if( nCurTail_ > 0 && 
                    nCurTail_ >= nSecondBlockLen )
                {
                    if(!bMoveHeaderOnly)
                    {
                        memcpy(pDataOut , pBuffer_+nCurHead_, nFirstBlockLen); 
                        memcpy(pDataOut+nFirstBlockLen , pBuffer_, nSecondBlockLen); 
                    }

                    if(!bPeek)
                    {
                        nCurHead_ =nSecondBlockLen ;
                    }
                }
                else
                {
#ifdef CUMBUFFER_DEBUG
                    std::cout << "    ln:" << __LINE__ << " / invalid request len:"<< nLen<<"\n"; //debug
#endif
                    return cumbuffer_defines:: OP_RSLT_INVALID_LEN;
                }
            }
            else
            {
                if(!bMoveHeaderOnly)
                {
                    memcpy(pDataOut, pBuffer_ + nCurHead_, nLen);
                }

                if(!bPeek)
                {
                    nCurHead_ += nLen;
                }
            }
        }

        if(!bPeek)
        {
            nCumulatedLen_ -= nLen;
        }

#ifdef CUMBUFFER_DEBUG
        DebugPos(__LINE__);
#endif

        return cumbuffer_defines::OP_RSLT_OK;
    }

    //------------------------------------------------------------------------
    cumbuffer_defines::OP_RESULT ValidateBuffer(size_t nLen)
    {
        if(nCumulatedLen_ == 0 )
        {
            return cumbuffer_defines::OP_RSLT_NO_DATA;
        }
        else if(nCumulatedLen_ < nLen)
        {
            return cumbuffer_defines:: OP_RSLT_INVALID_LEN;
        }

        return cumbuffer_defines::OP_RSLT_OK;
    }
        
    //------------------------------------------------------------------------
    size_t GetCumulatedLen()
    {
        return nCumulatedLen_ ;
    }

    //------------------------------------------------------------------------
    size_t GetCapacity()
    {
        return nBufferLen_ ;
    }

    //------------------------------------------------------------------------
    size_t GetFreeSpace()
    {
        return nBufferLen_  - nCumulatedLen_;
    }

    //------------------------------------------------------------------------
    uint64_t GetCurHeadPos()
    {
        return nCurHead_; 
    }

    //------------------------------------------------------------------------
    uint64_t GetCurTailPos()
    {
        return nCurTail_; 
    }

    //------------------------------------------------------------------------
    void    DebugPos(int nLine)
    {
        std::cout << "    line=" <<nLine<<"\t/ nCurHead_=" << nCurHead_  << "/ nCurTail_= " << nCurTail_ << "\n";
    }

    //------------------------------------------------------------------------
    void ReSet()
    {
        nCumulatedLen_=0;
        nCurHead_=0;
        nCurTail_=0;
    }

  private:

    char*       pBuffer_;
    size_t      nBufferLen_;
    size_t      nCumulatedLen_;
    uint64_t    nCurHead_ __attribute__ ((aligned (64))) ; 
    uint64_t    nCurTail_ __attribute__ ((aligned (64))) ; 
};

#endif



