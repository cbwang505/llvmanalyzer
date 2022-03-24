#pragma once

#include <retdec/utils/io/logger.h>

namespace retdec {
	namespace utils {
		namespace io {
			class LLVMIDAOutStream:public LoggerBuf
			{
			private:
				std::ostream& _out;
			public:

				LLVMIDAOutStream(std::ostream& stream);
				LLVMIDAOutStream(const LLVMIDAOutStream& buf) :_out(buf._out) {

				};
				
				template <typename T>
				LLVMIDAOutStream& operator << (const T& p)
				{					

						_out << p;
						INFO_MSG(p);
						return *this;
					
				};
				virtual std::ostream & real_buf() override;
				virtual LoggerBuf& operator << (std::string  const & p) override;
				virtual LoggerBuf& operator << (char*& p) override;
				virtual LoggerBuf& operator << (const char p[]) override;
			    virtual  LoggerBuf& operator <<(const LoggerBuf::StreamManipulator& p)override;
				
			};
			class LLVMIDALogger : public Logger {		
			private:				
			public:
				LLVMIDALogger(LoggerBuf& stream, bool verbose);
				~LLVMIDALogger();
			private:;
			};

			
		};
	};
};
;