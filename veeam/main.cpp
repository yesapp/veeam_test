//
//  main.cpp
//  veeam
//
//  Created by vitaliy on 03/11/2020.
//  Copyright © 2020 YesApp. All rights reserved.
//

#include <iostream>
#include <thread>
#include <vector>

#include "reader.hpp"
#include "handler.hpp"
#include "writer.hpp"
#include "buffer.hpp"
#include "exception.hpp"


int main(int argc, const char * argv[]) 
{
    if (argc < 3 || argc > 4)
    {
        std::cout << "Exception: Wrong argument number for: " << argv[0] << "\n";
        return -1;
    }
    
    const std::string inputFilePath = argv[1];
    const std::string outputFilePath = argv[2];
    
    int fileChunkSize = 1024*1024;
    int customFileChunkSize;
    if (argc == 4)
    {
        try
        {
            customFileChunkSize = std::stoi(argv[3]);
        }
        catch(const std::invalid_argument& e){
            std::cout << "Exception: Wrong file chunk size argument: " << argv[3] << "; Should be an integer; 1Mb will be used.\n";
            customFileChunkSize = 1;
        }
        catch(const std::out_of_range& e){
            std::cout << "Exception: Wrong file chunk size argument: " << argv[3] << "; The value is out of int range; 1Mb will be used.\n";
            customFileChunkSize = 1;
        }
        catch(...)
        {
            std::cout << "Exception: Wrong file chunk size argument: " << argv[3] << "; 1Mb will be used.\n";
            customFileChunkSize = 1;
        }
        fileChunkSize *= customFileChunkSize;
    }
    
    unsigned int optimalThreadNumber = std::thread::hardware_concurrency();
    const unsigned int threadNumber = optimalThreadNumber ? optimalThreadNumber : 4;
    
    ExceptionHandler exceptionHandler;
    
    ReadBuffer readBuffer;
    readBuffer.setOptimalBufferSize(threadNumber);
    
    WriteBuffer writeBuffer;
    
    Reader reader(readBuffer, exceptionHandler, inputFilePath);
    reader.setChunkSize(fileChunkSize);
    
    Handler handler(readBuffer, writeBuffer);
    
    Writer writer(writeBuffer, exceptionHandler, outputFilePath);
    
    std::vector<std::thread> threads;
    
    std::thread readerThread(&Reader::run, &reader);
    threads.emplace_back(std::move(readerThread));
    

    for(int i = 0; i < threadNumber; i++)
    {
        std::thread handlerThread(&Handler::run, &handler);
        threads.emplace_back(std::move(handlerThread));
    }
    
    std::thread writer_thread(&Writer::run, &writer);
    threads.emplace_back(std::move(writer_thread));
    
    for(auto& thr : threads) {
        thr.join();
    }
    
    for(auto &e: exceptionHandler.m_exceptions)
    {
        try
        {
            if(e != nullptr)
            {
                std::rethrow_exception(e);
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "Exception: " << e.what() << std::endl;
        }
    }
    
    return 0;
}
