//
//  writer.cpp
//  veeam
//
//  Created by vitaliy on 03/11/2020.
//  Copyright © 2020 YesApp. All rights reserved.
//

#include "writer.hpp"
#include <fstream>

void Writer::run()
{
    try {
        std::ofstream file(m_fileName);
        file.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
        
        while (!(m_writeBuffer.isDone() && m_writeBuffer.isEmpty()))
        {
            auto hashLayer = m_writeBuffer.remove();
            if (m_writeBuffer.m_shutdown)
            {
                break;
            }
            if (hashLayer.m_layerId == -1)
            {
                continue;
            }
            
            for(auto &msg: hashLayer.m_layerVector)
            {
                file << msg->getText() << "\n";
                if(msg->isLast())
                {
                    break;
                }
            }
        }
        
        file.close();
    }
    catch (...) {
        m_exceptionHandler.addException(std::current_exception());
    }
}
