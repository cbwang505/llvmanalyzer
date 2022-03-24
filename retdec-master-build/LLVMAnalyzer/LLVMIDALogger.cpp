#include "LLVMIDALogger.h"
#include "utils.h"

namespace retdec {
	namespace utils {
		namespace io {
			

			LLVMIDALogger::LLVMIDALogger(LoggerBuf& stream, bool verbose) : Logger(stream, verbose)
			{
				
			}

			LLVMIDALogger::~LLVMIDALogger()
			{
			};

			std::ostream & LLVMIDAOutStream::real_buf()  {
				return _out;
			};
			
			LLVMIDAOutStream::LLVMIDAOutStream(std::ostream & stream):_out(stream)
			{
			};


			inline LoggerBuf& LLVMIDAOutStream::operator << (std::string  const & p)
			{

				_out << p;
				msg(p.c_str());
				
				return *static_cast<LoggerBuf*>(this);
			};

			inline LoggerBuf& LLVMIDAOutStream::operator << (char*& p)
			{
				_out << p;
				msg(p);
				
				return *static_cast<LoggerBuf*>(this);
			}
			inline LoggerBuf& LLVMIDAOutStream::operator << (const char p[])
			{
			
				_out << p;
				msg(p);
			
				return *static_cast<LoggerBuf*>(this);
			}

			inline LoggerBuf& LLVMIDAOutStream::operator << (const LoggerBuf::StreamManipulator& p)
			{

				_out << p;
				msg("\n");				
				return *static_cast<LoggerBuf*>(this);
			}
		};
	};
};