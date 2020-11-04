//
//  writer.hpp
//  veeam
//
//  Created by vitaliy on 03/11/2020.
//  Copyright © 2020 YesApp. All rights reserved.
//

#ifndef writer_hpp
#define writer_hpp

#include "buffer.hpp"
#include "message.hpp"
#include "exception.hpp"

#include <memory>

class Writer
{
public:
    Writer(WriteBuffer& WriteBuffer, ExceptionHandler& exceptionHandler, const std::string& filename):
        m_writeBuffer(WriteBuffer), m_exceptionHandler(exceptionHandler), m_fileName(filename)
    {}
    
    void run();
    
private:
    WriteBuffer &m_writeBuffer;
    std::string m_fileName;
    ExceptionHandler &m_exceptionHandler;
};

#endif /* writer_hpp */
