/**
 * @file /include/yaramod/yaramod_error.h
 * @brief Declaration of class YaramodError.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#pragma once

#include <string>

namespace yaramod {

class YaramodError : public std::exception
{
public:
   YaramodError(const std::string& errorMsg) : _errorMsg(errorMsg) {}
   YaramodError(const std::string& errorMsg1, const std::string& errorMsg2) : _errorMsg(errorMsg1)
   {
      _errorMsg.append(", ").append(errorMsg2);
   }
   YaramodError(const YaramodError&) = default;

   const std::string& getErrorMessage() const noexcept
   {
      return _errorMsg;
   }

   virtual const char* what() const noexcept override
   {
      return _errorMsg.c_str();
   }

protected:
   std::string _errorMsg;
};

} //namespace yaramod
