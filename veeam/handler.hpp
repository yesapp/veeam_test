//
//  handler.hpp
//  veeam
//
//  Created by vitaliy on 03/11/2020.
//  Copyright © 2020 YesApp. All rights reserved.
//

#ifndef handler_hpp
#define handler_hpp

#include "buffer.hpp"
#include "message.hpp"
#include "exception.hpp"

#include <memory>

class Handler
{
public:
    Handler(ReadBuffer& readBuffer, WriteBuffer& writeBuffer):
        m_readBuffer(readBuffer),
        m_writeBuffer(writeBuffer)
    {}

	Handler(const Handler&) = delete;
	Handler& operator=(const Handler &) = delete;
    
    void run();
    
private:
    ReadBuffer &m_readBuffer;
    WriteBuffer &m_writeBuffer;
    
    std::unique_ptr<Message> m_message;
};

#endif /* handler_hpp */
