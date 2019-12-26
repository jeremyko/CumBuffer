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

// NO THREAD SAFETY HERE !!! 
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string>
#include <string.h>
#include <stdint.h>
#include <cstdlib>
#include <cassert>

namespace cumbuffer
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
    CumBuffer() {
        buffer_ptr_=NULL; 
        cumulated_len_=0;
        curr_head_=0;
        curr_tail_=0;
        buffer_len_=0;
    }

    virtual ~CumBuffer() { 
        if(buffer_ptr_) { 
            delete [] buffer_ptr_; 
        } 
    };
    //-------------------------------------------------------------------------
    cumbuffer::OP_RESULT Init(size_t max_buffer_len = cumbuffer::DEFAULT_BUFFER_LEN) {
        buffer_len_ = max_buffer_len;
        try {
            buffer_ptr_ = new char [buffer_len_];
        } catch (std::exception& e) {
            std::cerr <<"["<< __func__ <<"-"<<__LINE__ <<"] alloc failed :"<<e.what()  <<"\n"; 
            err_msg_="alloc failed :";
            err_msg_+= e.what();
            return cumbuffer::OP_RSLT_ALLOC_FAILED;
        }
        return cumbuffer::OP_RSLT_OK;
    }
    //-------------------------------------------------------------------------
    cumbuffer::OP_RESULT    Append(size_t len, char* pData)
    {
#ifdef CUMBUFFER_DEBUG
        std::cout <<"["<< __func__ <<"-"<<__LINE__ <<"] len="<<len<< "["<< pData<<"]\n";  
        DebugPos(__LINE__);
#endif
        if( buffer_len_ < len ) {
            std::cerr <<"["<< __func__ <<"-"<<__LINE__ <<"] invalid len :"<<len <<"\n"; 
            err_msg_="invalid length";
            return cumbuffer::OP_RSLT_INVALID_LEN;
        } else if( buffer_len_ ==  cumulated_len_ ) {
            std::cerr <<"["<< __func__ <<"-"<<__LINE__ <<"] buffer full" <<"\n"; 
            err_msg_="buffer full";
            return cumbuffer::OP_RSLT_BUFFER_FULL;
        }
        if(curr_tail_ < curr_head_) {
            //tail 이 버퍼 끝을 지난 경우
            if(curr_head_ - curr_tail_ >= len) {
                memcpy(buffer_ptr_ + curr_tail_, pData, len);
                curr_tail_ += len;
                cumulated_len_ += len;
#ifdef CUMBUFFER_DEBUG
                DebugPos(__LINE__);
#endif
                return cumbuffer::OP_RSLT_OK;
            } else {
                std::cerr <<"["<< __func__ <<"-"<<__LINE__ <<"] buffer full" 
                    << ", curr_head= " << curr_head_ << ", curr_tail =" <<curr_tail_ << "\n"; 
                err_msg_="buffer full";
                return cumbuffer::OP_RSLT_BUFFER_FULL;
            }
        } else {
            if (buffer_len_ < curr_tail_ + len) {
                //tail 이후, 남은 버퍼로 모자라는 경우
                if( curr_tail_ > 0 && 
                    len - (buffer_len_ - curr_tail_)  <= curr_head_ ) {
                    //2번 나누어서 들어갈 공간이 있는 경우
#ifdef CUMBUFFER_DEBUG
                    DebugPos(__LINE__);
#endif
                    size_t first_block_len  = buffer_len_ - curr_tail_;
                    size_t second_block_len = len - first_block_len;
#ifdef CUMBUFFER_DEBUG
                    std::cout <<"["<< __func__ <<"-"<<__LINE__ 
                        <<"] first_block_len ="<<first_block_len  
                        << ", second_block_len="<<second_block_len<<"\n"; 
#endif
                    if(first_block_len>0) {
                        memcpy(buffer_ptr_+ curr_tail_ , pData, first_block_len); 
                    }
                    memcpy(buffer_ptr_ , pData+(first_block_len), second_block_len); 
                    curr_tail_ = second_block_len;
                    cumulated_len_ += len;
#ifdef CUMBUFFER_DEBUG
                    DebugPos(__LINE__);
#endif
                    return cumbuffer::OP_RSLT_OK;
                } else {
                    std::cerr <<"["<< __func__ <<"-"<<__LINE__ <<"] buffer full" <<"\n"; 
                    err_msg_="buffer full";
                    return cumbuffer::OP_RSLT_BUFFER_FULL;
                }
            } else {
                //most general case
                memcpy(buffer_ptr_+curr_tail_ , pData, len); 
                curr_tail_ += len;
                cumulated_len_ += len;
#ifdef CUMBUFFER_DEBUG
                DebugPos(__LINE__);
#endif
                return cumbuffer::OP_RSLT_OK;
            }
        }
        return cumbuffer::OP_RSLT_OK;
    }
    //-------------------------------------------------------------------------
    cumbuffer::OP_RESULT    PeekData(size_t len, char* data_out) {
        return GetData(len, data_out, true, false);
    }
    //-------------------------------------------------------------------------
    cumbuffer::OP_RESULT    ConsumeData(size_t len) {
        //PeekData 사용해서 처리한 data length 만큼 버퍼내 curr_head_ 를 이동.
        return GetData(len, NULL, false, true);
    }
    //-------------------------------------------------------------------------
    cumbuffer::OP_RESULT    GetData(size_t  len, 
                                            char*   data_out, 
                                            bool    is_peek =false, 
                                            bool    is_move_header_only=false)
    {
        if(is_peek  && is_move_header_only) {
            std::cerr <<"["<< __func__ <<"-"<<__LINE__ <<"] invalid usage" <<"\n"; 
            err_msg_="invalid usage";
            return cumbuffer::OP_RSLT_INVALID_USAGE;
        }
#ifdef CUMBUFFER_DEBUG
        DebugPos(__LINE__);
#endif
        cumbuffer::OP_RESULT nRslt = ValidateBuffer(len);
        if(cumbuffer::OP_RSLT_OK!=nRslt ) {
            std::cerr <<"["<< __func__ <<"-"<<__LINE__ <<"] invalid buffer :"<<err_msg_ <<"\n"; 
            return nRslt;
        }
        if(curr_tail_ > curr_head_) {
            //일반적인 경우
            if (curr_tail_ < curr_head_ + len) {
                std::cerr <<"["<< __func__ <<"-"<<__LINE__ <<"] invalid length :"<<len <<"\n"; 
                err_msg_="invalid length";
                return cumbuffer:: OP_RSLT_INVALID_LEN;
            } else {
                if(!is_move_header_only) {
                    memcpy(data_out, buffer_ptr_ + curr_head_, len);
                }
                if(!is_peek ) {
                    curr_head_ += len;
                }
            }
        } else {
            if (buffer_len_ < curr_head_ + len) {
                size_t first_block_len = buffer_len_ - curr_head_;
                size_t second_block_len = len - first_block_len;
#ifdef CUMBUFFER_DEBUG
                std::cout <<"["<< __func__ <<"-"<<__LINE__ <<"] first_block_len="
                        <<first_block_len  
                        << "/second_block_len="<<second_block_len<<"\n"; 
#endif
                if( curr_tail_ > 0 && curr_tail_ >= second_block_len ) {
                    if(!is_move_header_only) {
                        memcpy(data_out , buffer_ptr_+curr_head_, first_block_len); 
                        memcpy(data_out+first_block_len , buffer_ptr_, second_block_len); 
                    }
                    if(!is_peek ) {
                        curr_head_ =second_block_len ;
                    }
                } else {
                    std::cerr <<"["<< __func__ <<"-"<<__LINE__ <<"] invalid length :"<<len  
                              <<" / first_block_len ="<<first_block_len 
                              << "/second_block_len="<<second_block_len<<"\n"; 
                    err_msg_="invalid length";
                    return cumbuffer:: OP_RSLT_INVALID_LEN;
                }
            } else {
                if(!is_move_header_only) {
                    memcpy(data_out, buffer_ptr_ + curr_head_, len);
                }
                if(!is_peek ) {
                    curr_head_ += len;
                }
            }
        }
        if(!is_peek ) {
            cumulated_len_ -= len;
        }
#ifdef CUMBUFFER_DEBUG
        std::cout <<"["<< __func__ <<"-"<<__LINE__ <<"] out data ["<<data_out<<"]\n";
        DebugPos(__LINE__);
#endif
        return cumbuffer::OP_RSLT_OK;
    }
    //-------------------------------------------------------------------------
    cumbuffer::OP_RESULT ValidateBuffer(size_t len) {
        if(cumulated_len_ == 0 ) {
            err_msg_="no data";
            return cumbuffer::OP_RSLT_NO_DATA;
        } else if(cumulated_len_ < len) {
            err_msg_="invalid length";
            return cumbuffer:: OP_RSLT_INVALID_LEN;
        }
        return cumbuffer::OP_RSLT_OK;
    }
    //-------------------------------------------------------------------------
    size_t GetCumulatedLen() {
        return cumulated_len_ ;
    }
    //-------------------------------------------------------------------------
    size_t GetCapacity() {
        return buffer_len_ ;
    }
    //-------------------------------------------------------------------------
    size_t GetTotalFreeSpace() {
        return buffer_len_  - cumulated_len_;
    }
    //-------------------------------------------------------------------------
    uint64_t GetCurHeadPos() {
        return curr_head_; 
    }
    //-------------------------------------------------------------------------
    uint64_t GetCurTailPos() {
        return curr_tail_; 
    }
    //-------------------------------------------------------------------------
    //for direct buffer write
    uint64_t GetLinearFreeSpace() {
        //current maximun linear buffer size
        if(curr_tail_==buffer_len_) {
            //curr_tail_ is at last position
            return buffer_len_ - cumulated_len_ ; 
        } else if(curr_head_ < curr_tail_) {
            return buffer_len_- curr_tail_; 
        } else if(curr_head_ > curr_tail_) {
            return curr_head_-curr_tail_; 
        } else {
            return buffer_len_- curr_tail_;
        }
    }
    //------------------------------------------------------------------------
    //for direct buffer write
    char* GetLinearAppendPtr() {
        if(curr_tail_==buffer_len_) {
            //curr_tail_ is at last position
            if(buffer_len_!= cumulated_len_) {
                //and buffer has free space
                //-> append at 0  
                //curr_tail_ -> 버퍼 마지막 위치하고, 버퍼에 공간이 존재. -> 처음에 저장
                //XXX dangerous XXX 
                //this is not a simple get function, curr_tail_ changes !!
                curr_tail_ = 0;
            }
        }
        return (buffer_ptr_+ curr_tail_);
    }
    //-------------------------------------------------------------------------
    void IncreaseData(size_t len) {
        curr_tail_+= len;
        cumulated_len_ +=len;
        if (cumulated_len_ > buffer_len_) {
            std::cerr << "invalid len error!\n";
        }
        assert(cumulated_len_ <= buffer_len_);//XXX
    }
    //-------------------------------------------------------------------------
    void    DebugPos(int line) {
        std::cout <<"line=" <<line<<"/ curr_head_=" << curr_head_  
                << "/ curr_tail_= "  << curr_tail_ 
                <<" / buffer_len_=" << buffer_len_
                <<" / cumulated_len_=" << cumulated_len_ <<"\n";
    }
    //-------------------------------------------------------------------------
    void ReSet() {
        cumulated_len_=0;
        curr_head_=0;
        curr_tail_=0;
    }
    //-------------------------------------------------------------------------
    const char* GetErrMsg() { 
        return err_msg_.c_str() ; 
    }

  private:
    CumBuffer(const CumBuffer&) ; //= delete;
    CumBuffer& operator = (const CumBuffer &) ; //= delete;

    std::string err_msg_;
    char*       buffer_ptr_;
    size_t      buffer_len_;
    size_t      cumulated_len_;
    uint64_t    curr_head_  ; 
    uint64_t    curr_tail_  ; 
} ;

#endif




