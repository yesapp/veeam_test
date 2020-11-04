//
//  buffer.cpp
//  veeam
//
//  Created by vitaliy on 03/11/2020.
//  Copyright © 2020 YesApp. All rights reserved.
//

#include "buffer.hpp"

void ReadBuffer::add(std::unique_ptr<Message> msg)
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    cond.wait(locker, [this](){ return m_readBuffer.size() < this->getOptimalBufferSize(); });
    m_readBuffer.push(std::move(msg));
    cond.notify_all();
}

std::unique_ptr<Message> ReadBuffer::remove()
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    cond.wait(locker, [this](){
        if (m_shutdown)
        {
            return true;
        }
        return m_readBuffer.size() > 0;
    });
    if (m_shutdown)
    {
        cond.notify_all();
        return nullptr;
    }
	std::unique_ptr<Message> last = std::move(m_readBuffer.front());
    m_readBuffer.pop();
    cond.notify_all();
    return last;
}

void ReadBuffer::setIsDone(bool bDone)
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    m_isDone = bDone;
}

bool ReadBuffer::isDone() const
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    return m_isDone;
}

bool ReadBuffer::isEmpty() const
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    if(m_readBuffer.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

size_t ReadBuffer::size() const
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    return m_readBuffer.size();
}

void ReadBuffer::setOptimalBufferSize(int optSize)
{
    m_optimalBufferSize = optSize;
}

int ReadBuffer::getOptimalBufferSize() const
{
    return m_optimalBufferSize;
}

void WriteBuffer::add(std::unique_ptr<Message> msg)
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    int id = msg->getId();
    int layerId = id / this->getOptimalBufferSize();
    int msgOrdinalNumber = id % this->getOptimalBufferSize();
    
    bool isLast = msg->isLast();
    if (isLast)
    {
        this->setIsDone(true);
    }
    
    if ((m_writeBuffer.size() == 0) || (layerId > m_writeBuffer.back().m_layerId))
    {
        HashLayer newLayer;
        newLayer.m_layerId = layerId;
        newLayer.m_layerVector = std::vector<std::unique_ptr<Message>>(this->getOptimalBufferSize());
        newLayer.m_layerVector[msgOrdinalNumber] =  std::move(msg);
        m_writeBuffer.push_back(std::move(newLayer));
    }
    else
    {
        auto hashLayerPtr = this->getLayerById(layerId);
        if (hashLayerPtr != nullptr)
        {
            auto &hashLayer = *hashLayerPtr;
            hashLayer.m_layerVector[msgOrdinalNumber] = std::move(msg);
        }
    }
    cond.notify_all();
}

bool WriteBuffer::isFull() const
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    const auto& hashLayer = m_writeBuffer.front();
    for(auto &msg: hashLayer.m_layerVector)
    {
        if (msg == nullptr)
        {
            return false;
        }
        if(msg->isLast() && this->isDone())
        {
            return true;
        }
        
    }
    return true;
}

HashLayer WriteBuffer::remove()
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    HashLayer lastLayer;
    cond.wait(locker, [this](){
        if(m_shutdown)
        {
            return true;
        }
        return m_writeBuffer.size() > 0;        
    });
    if(m_shutdown)
    {
        cond.notify_all();
        return lastLayer;
    }
    
    if (isFull())
    {
        lastLayer = std::move(m_writeBuffer.front());
        m_writeBuffer.erase(m_writeBuffer.begin());
    }
    cond.notify_all();
    return lastLayer;
}

void WriteBuffer::setIsDone(bool bDone)
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    m_isDone = bDone;
}

bool WriteBuffer::isDone() const
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    return m_isDone;
}

bool WriteBuffer::isEmpty() const
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    if(m_writeBuffer.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void WriteBuffer::setOptimalBufferSize(int optSize)
{
    m_optimalBufferSize = optSize;
}

int WriteBuffer::getOptimalBufferSize() const
{
    return m_optimalBufferSize;
}

HashLayer* WriteBuffer::getLayerById(int id)
{
    std::unique_lock<std::recursive_mutex> locker(rw_mutex);
    for(auto& layer: m_writeBuffer)
    {
        if(layer.m_layerId == id)
        {
            return &layer;
        }
    }
    return nullptr;
}
