//
//  reader.hpp
//  veeam
//
//  Created by vitaliy on 03/11/2020.
//  Copyright © 2020 YesApp. All rights reserved.
//

#ifndef reader_hpp
#define reader_hpp

#include <string>
#include "buffer.hpp"
#include "exception.hpp"

class Reader
{
public:
    Reader(ReadBuffer& readBuffer, ExceptionHandler& eh, const std::string& filename):
        m_readBuffer(readBuffer),
        m_exceptionHandler(eh),
        m_fileName(filename)
    {}
    
    void setChunkSize(int chunkSize)
    {
        m_chunkSize = chunkSize;
    }
    void run();    
    
private:
    ReadBuffer &m_readBuffer;
    std::string m_fileName;
    int m_chunkSize = 1024*1024;
    ExceptionHandler &m_exceptionHandler;
};

#endif /* reader_hpp */
