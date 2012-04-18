/*
 * Copyright 2012 The Fifteen Puzzle Project, <blackchval@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.

 * THIS SOFTWARE IS PROVIDED BY THE FIFTEEN PUZZLE PROJECT AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE FIFTEEN PUZZLE PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Logger.h"
#include "ConsoleLogger.h"
#include <time.h>

Logger* Logger::_loggerInstance = 0; // declare static pointer to Logger instance

/**
 * Logger::getInstance
 *
 * Static method for obtaining concrete logger instance
 * Realization of singleton pattern
 *
 */
Logger& Logger::getInstance()
{
    if ( ! _loggerInstance )
    {
        _loggerInstance = new ConsoleLogger;
    }
    return *_loggerInstance;
}

/**
 * Logger::timestamp
 *
 * Common method for writing timestamp
 * Timestamp is followed by a new-line character ('\n')
 *
 */
void Logger::timestamp()
{
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    this->write(asctime(timeinfo));
}

/**
 * Logger::operator<<
 *
 * Custom overload of << operator for string
 *
 */
Logger& Logger::operator << (const std::string message)
{
    this->write(message);
    return *this;
};

/**
 * Logger::append
 *
 * Just write string message
 *
 */
Logger& Logger::append(const std::string message)
{
    this->write(message);
    return *this;
};

/**
 * Logger::errorAt
 *
 * Begin error message and show function name where it happened
 * Message is followed by a new-line character ('\n')
 *
 * ex. "Error at Main::main"
 *
 */
Logger& Logger::errorAt(const std::string funcName)
{
    this->write(LOGGER_START_ERROR);
    this->write(funcName);
    this->write(LOGGER_ENDL);
    return *this;
}

/**
 * Logger::explain
 *
 * Explain error
 * No new-line character ('\n') is printed after message
 *
 * Should be called after errorAt method, because of special formatting
 *
 */
Logger& Logger::explain(const std::string message)
{
    this->write(LOGGER_START_EXPLAIN);
    this->write(message);
    return *this;
}

/**
 * Logger::endl
 *
 * Write new-line character ('\n')
 *
 */
Logger& Logger::endl()
{
    this->write(LOGGER_ENDL);
    return *this;
}
