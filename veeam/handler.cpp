//
//  handler.cpp
//  veeam
//
//  Created by vitaliy on 03/11/2020.
//  Copyright © 2020 YesApp. All rights reserved.
//

#include "handler.hpp"
#include "md5.hpp"

#include <fstream>

void Handler::run()
{
    while (!(m_readBuffer.isDone() && m_readBuffer.isEmpty()))
    {
        auto hashMsg = m_readBuffer.remove();
        if (!hashMsg)
        {
            m_writeBuffer.m_shutdown = true;
            return;
        }
        const std::string hash = md5(hashMsg->getText());
        hashMsg->setText(hash);
        m_writeBuffer.add(std::move(hashMsg));
    }
}
