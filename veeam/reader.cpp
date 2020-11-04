//
//  reader.cpp
//  veeam
//
//  Created by vitaliy on 03/11/2020.
//  Copyright © 2020 YesApp. All rights reserved.
//

#include "reader.hpp"
#include "message.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <memory>

#include <sys/types.h>
#include <sys/stat.h>

void Reader::run()
{
    try {
        std::ifstream file(m_fileName, std::ifstream::binary);
        file.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
        
        struct stat fileStatus;
        stat(m_fileName.c_str(), &fileStatus);
        
        size_t totalSize = fileStatus.st_size;
        size_t totalChunks = totalSize / m_chunkSize;
        size_t lastChunkSize = totalSize % m_chunkSize;
        
        if (lastChunkSize != 0)
        {
            totalChunks++;
        }
        else
        {
            lastChunkSize = m_chunkSize;
        }
        
        /* the loop of chunking */
        for (size_t chunkId = 0; chunkId < totalChunks; chunkId++)
        {
            size_t thisChunkSize =
            chunkId == totalChunks - 1 /* if last chunk */
            ? lastChunkSize /* then fill chunk with remaining bytes */
            : m_chunkSize; /* else fill entire chunk */
            
            std::string textChunk(thisChunkSize, ' ');
            
            file.read(&textChunk[0], thisChunkSize);
            
            bool isLast = (chunkId == totalChunks - 1);
            auto hashMsg = std::make_unique<Message>(chunkId, textChunk, isLast);
            
            m_readBuffer.add(std::move(hashMsg));
        }
        file.close();
        
        m_readBuffer.setIsDone(true);
    }
    catch (...) {
        m_exceptionHandler.addException(std::current_exception());
        m_readBuffer.m_shutdown = true;
    }
}
