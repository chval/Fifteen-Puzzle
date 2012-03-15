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

#ifndef LOGGER_H
#define LOGGER_H

#define LOGGER_ENDL                 "\n"
#define LOGGER_START_ERROR          "=> Error at "
#define LOGGER_START_EXPLAIN        " > "

#include <string>
#include <sstream>

/**
 * Abstract Logger
 *
 */
class Logger
{
public:
    static Logger& getInstance();
    Logger& errorAt(std::string funcName);
    Logger& explain(std::string message);
    Logger& append(std::string s);
    Logger& endl();
    void timestamp();
    Logger& operator << (std::string message);

    template <typename T> Logger& operator << (T message);
    template <typename T> Logger& explain(T message);
    template <typename T> Logger& append(T message);
private:
    static Logger *_loggerInstance;
protected:
    virtual void write(std::string message) = 0;
};

/**
 * Logger::operator<<
 *
 * Overload of << operator for all types
 *
 */
template <typename T> Logger& Logger::operator << (T message)
{
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << message;
    this->write(ss.str());
    return *this;
};

/**
 * Logger::explain
 *
 * Explain error
 * No new-line character ('\n') is printed after message
 *
 * Should be called after errorAt method, because of special formatting
 *
 */
template <typename T> Logger& Logger::explain(T message)
{
    this->write(LOGGER_START_EXPLAIN);
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << message;
    this->write(ss.str());
    return *this;
};


/**
 * Logger::append
 *
 * Just write message
 *
 */
template <typename T> Logger& Logger::append(T message)
{
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << message;
    this->write(ss.str());
    return *this;
}

#endif // LOGGER_H
