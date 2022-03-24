/**
* @file include/retdec/utils/io/logger.h
* @brief Implementation of a logging class.
* @copyright (c) 2020 Avast Software, licensed under the MIT license
*/

#ifndef RETDEC_UTILS_IO_LOGGER_H
#define RETDEC_UTILS_IO_LOGGER_H

#include <filesystem>
#include <iostream>
#include <fstream>

namespace retdec {
	namespace utils {
		namespace io {

			class LoggerBuf {
			public:
				typedef std::ostream& ( *StreamManipulator) (std::ostream& p);
			
				LoggerBuf(){

				};
				~LoggerBuf() {

				};
				LoggerBuf(const LoggerBuf& buf)=delete;
				virtual std::ostream & real_buf() = 0;
				template <typename T>
				LoggerBuf& operator << (const T& p);

				virtual	 LoggerBuf& operator <<(std::string  const & p) = 0;
				virtual  LoggerBuf& operator <<(char*& p) = 0;
				virtual  LoggerBuf& operator <<(const char p[]) = 0;
				virtual  LoggerBuf& operator <<(const StreamManipulator& p) = 0;
							
			};
			class  LoggerBufStream :public LoggerBuf {
			public:
				LoggerBufStream(std::ostream& stream);
				~LoggerBufStream();
				LoggerBufStream(const LoggerBufStream& from);			
				virtual std::ostream & real_buf() override {
					return _out;
				};
				virtual	 LoggerBuf& operator <<(std::string  const & p) override;
				virtual  LoggerBuf& operator <<(char*& p) override;
				virtual  LoggerBuf& operator <<(const char p[]) override;
				virtual  LoggerBuf& operator <<(const StreamManipulator& p)override;

			protected:
				std::ostream & _out;
			};
			/**
			 * @brief Provides Logger inteface that is used for logging events during decompilation.
			 */
			class Logger {
			public:
				using Ptr = std::unique_ptr<Logger>;
				using Ptr_Shared = std::shared_ptr<Logger>;

			public:
				enum Action : int {
					Phase,
					SubPhase,
					SubSubPhase,
					ElapsedTime,
					Error,
					Warning,
					NoAction
				};

				enum class Color : int {
					Red,
					Green,
					Blue,
					Yellow,
					DarkCyan,
					Default
				};

			

			public:
				Logger(LoggerBuf& stream, bool verbose = true);
				Logger(std::ostream& stream, bool verbose = true);
				Logger(const Logger& logger);
				~Logger();
				virtual Logger* Clone();
				/*
				template <typename T>
				Logger& operator << (const T& p);
*/
				Logger& operator << (LoggerBuf::StreamManipulator manip);
				Logger& operator << (const Action& ia);
				Logger& operator << (const Color& lc);
				Logger& operator << (const std::string& p);
				Logger& operator << (char*& p);				
				Logger& operator << (const char p[]);

			private:
				bool isRedirected(const std::ostream& stream) const;

			protected:
				LoggerBuf& _outbuf;

				bool _verbose = true;
				Color _currentBrush = Color::Default;

				bool _modifiedTerminalProperty = false;
				bool _terminalNotSupported = false;
			};

			class FileLogger : public Logger {
			public:
				FileLogger(const std::string& file, bool verbose = true);

			private:
				std::ofstream _file;
			};

			/*template<typename T>
			inline Logger& Logger::operator << (const T& p)
			{
				if (!_verbose)
					return *this;

				_out << p;

				return *this;
			}   */
			inline Logger& Logger::operator << (const std::string& p)
			{
				if (!_verbose)
					return *this;

				_outbuf << p;

				return *this;
			}
			inline Logger& Logger::operator << (LoggerBuf::StreamManipulator p)
			{
				if (!_verbose)
					return *this;

				_outbuf << p;

				return *this;
			}
			inline Logger& Logger::operator << (char*& p)
			{
				if (!_verbose)
					return *this;

				_outbuf << p;

				return *this;
			}
			inline Logger& Logger::operator << (const char p[])
			{
				if (!_verbose)
					return *this;

				_outbuf << p;

				return *this;
			}

			
		}
	}
}

#endif
