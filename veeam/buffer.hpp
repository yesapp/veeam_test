//
//  buffer.hpp
//  veeam
//
//  Created by vitaliy on 03/11/2020.
//  Copyright © 2020 YesApp. All rights reserved.
//

#ifndef buffer_hpp
#define buffer_hpp

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>
#include <memory>

#include "message.hpp"
#include "exception.hpp"

class ReadBuffer
{
public:
    ReadBuffer():
        m_isDone(false),
        m_shutdown(false)
    {}
    
    void add(std::unique_ptr<Message> msg);
	std::unique_ptr<Message> remove();
    bool isEmpty() const;
    size_t size() const;
    bool isDone() const;
    void setIsDone(bool bDone);
    void setOptimalBufferSize(int optSize);
    int getOptimalBufferSize() const;
    
    std::atomic_bool m_shutdown;
    
private:
    mutable std::recursive_mutex rw_mutex;
    std::condition_variable_any cond;
    
    std::queue<std::unique_ptr<Message>> m_readBuffer;
    int m_optimalBufferSize = 4;
    bool m_isDone;
    
};

struct HashLayer
{
    int m_layerId = -1;
    std::vector<std::unique_ptr<Message>> m_layerVector;
};

class WriteBuffer
{
public:
    WriteBuffer():
        m_isDone(false),
        m_shutdown(false)
    {}
    
    void add(std::unique_ptr<Message> msg);
    HashLayer remove();
    bool isEmpty() const;
    bool isDone() const;
    void setIsDone(bool bDone);
    void setOptimalBufferSize(int optSize);
    int getOptimalBufferSize() const;
    HashLayer* getLayerById(int id);
    
    std::atomic_bool m_shutdown;
    
private:
    bool isFull() const;    
    
    mutable std::recursive_mutex rw_mutex;
    std::condition_variable_any cond;
    
    std::vector<HashLayer> m_writeBuffer;
    bool m_isDone;
    int m_optimalBufferSize = 4;
};



#endif /* buffer_hpp */
