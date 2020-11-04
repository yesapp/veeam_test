//
//  exception.h
//  veeam
//
//  Created by vitaliy on 03/11/2020.
//  Copyright © 2020 YesApp. All rights reserved.
//

#ifndef exception_hpp
#define exception_hpp

#include <mutex>
#include <exception>
#include <stdexcept>
#include <memory>

struct ExceptionHandler
{
    std::mutex e_mutex;
    std::vector<std::exception_ptr> m_exceptions;
    
    void addException(const std::exception_ptr& e)
    {
        std::unique_lock<std::mutex> locker(e_mutex);
        m_exceptions.emplace_back(e);
    }
};

#endif /* exception_hpp */
