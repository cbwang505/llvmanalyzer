/**
* @file src/utils/io/logger.cpp
* @brief Implementation of a logging class.
* @copyright (c) 2020 Avast Software, licensed under the MIT license
*/

#include <iomanip>
#include <map>
#include <sstream>

#include "retdec/utils/io/logger.h"
#include "retdec/utils/os.h"
#include "retdec/utils/time.h"

#ifdef OS_WINDOWS
#include <io.h>
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <unistd.h>
#endif

namespace retdec {
	namespace utils {
		namespace io {

			LoggerBufStream::LoggerBufStream(std::ostream& stream):_out ( stream)
			{
				
			};
			LoggerBufStream::LoggerBufStream(const LoggerBufStream& from) :LoggerBufStream(from._out)
			{

			};
			LoggerBufStream::~LoggerBufStream()
			{
				delete &_out;
			}
			inline	LoggerBuf& LoggerBufStream::operator << (std::string  const & p)
			{
				_out << p;			
				return *static_cast<LoggerBuf*>(this);
			};
			inline LoggerBuf& LoggerBufStream::operator << (char*& p)
			{
				_out << p;
				return *static_cast<LoggerBuf*>(this);
			}
			inline LoggerBuf& LoggerBufStream::operator << (const char p[])
			{
				_out << p;
				return *static_cast<LoggerBuf*>(this);
			}

			inline LoggerBuf& LoggerBufStream::operator << (const StreamManipulator& p)
			{			
				_out << p;
				return *static_cast<LoggerBuf*>(this);
			}
			//////////
			//
			// Logger
			//
			//////
			Logger::Logger(std::ostream& stream, bool verbose):Logger(*(new LoggerBufStream(stream)), verbose){
			}
			Logger::Logger(LoggerBuf& stream, bool verbose) :
				_outbuf(stream),
				_verbose(verbose)
			{
#ifdef OS_WINDOWS
				// On windows we need to try to set ENABLE_VIRTUAL_TERMINAL_PROCESSING.
				// This will enable ANSI support in terminal. This is best effort
				// implementation approach.
				//
				// Source: https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
				_terminalNotSupported = true;
				HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
				if (hOut == INVALID_HANDLE_VALUE)
					return;

				DWORD dwMode = 0;
				if (!GetConsoleMode(hOut, &dwMode))
					return;

				_modifiedTerminalProperty = dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				if (!SetConsoleMode(hOut, dwMode))
					return;

				_terminalNotSupported = false;
#endif
			}

			Logger::Logger(const Logger& from) :Logger(from._outbuf, from._verbose)
			{
				//Logger& clone = from.Clone();
				//Logger(clone._outbuf, clone._verbose);
				_currentBrush = from._currentBrush;
			}
			 Logger* Logger::Clone()
			{
				return new Logger(*this);
			};
			Logger::~Logger()
			{
				if (_currentBrush != Color::Default)
					*this << Color::Default;
				//delete &_outbuf;
			}

			Logger& Logger::operator << (const Action& p)
			{
				if (p == Phase) {
					return *this << Color::Yellow << "Running phase: ";
				}
				if (p == SubPhase) {
					return *this << Color::Yellow << " -> ";
				}
				if (p == SubSubPhase) {
					return *this << Color::Yellow << "     -> ";
				}
				if (p == SubSubPhase) {
					return *this << Color::Yellow << "         -> ";
				}
				if (p == Error) {
					return *this << "Error: ";
				}
				if (p == Warning) {
					return *this << Color::DarkCyan << "Warning: ";
				}
				if (p == ElapsedTime) {
					std::stringstream formatted;
					formatted << std::fixed << std::setprecision(2) << getElapsedTime();
					return *this << " ( " << formatted.str() << "s )";
				}

				return *this;
			}

			Logger& Logger::operator << (const Color& lc)
			{
#ifdef __IDP__				
				return *this;
#endif				
				static std::string ansiMap[static_cast<int>(Color::Default) + 1] = {
					/*Color::Red*/     "\u001b[0;1;31m",
					/*Color::Green*/   "\u001b[0;1;32m",
					/*Color::Blue*/    "\u001b[0;1;34m",
					/*Color::Yellow*/  "\u001b[0;1;33m",
					/*Color::DarkCyan*/"\u001b[0;1;36m",
					/*Color::Default*/ "\u001b[0m"
				};

				if (_terminalNotSupported || isRedirected(_outbuf.real_buf()))
					return *this;

				_currentBrush = lc;
				return *this << ansiMap[static_cast<int>(lc)];
			}

			bool Logger::isRedirected(const std::ostream& stream) const
			{
#ifdef OS_WINDOWS
				// On windows POSIX functions isatty and fileno
				// generate Warning.
				auto isConsole = _isatty;
				auto fileDescriptor = _fileno;
#else
				auto isConsole = isatty;
				auto fileDescriptor = fileno;
#endif

				if (stream.rdbuf() == std::cout.rdbuf()) {
					return isConsole(fileDescriptor(stdout)) == 0;
				}
				else if (stream.rdbuf() == std::cerr.rdbuf()) {
					return isConsole(fileDescriptor(stderr)) == 0;
				}

				return true;
			}

			//////////
			//
			// FileLogger
			//
			//////

			FileLogger::FileLogger(const std::string& file, bool verbose) :
				Logger(_file, verbose)
			{
				_file.open(file, std::ofstream::out);
				if (!_file)
					throw std::runtime_error("unable to open file \"" + file + "\" for writing.");
			}
			
			
		}
	}
}
