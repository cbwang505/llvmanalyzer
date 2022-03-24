/**
 * @file src/types/meta.cpp
 * @brief Implementation of class Meta.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/meta.h"

namespace yaramod {

/**
 * Returns the string representation of the meta information.
 *
 * @return String representation.
 */
std::string Meta::getText() const
{
	return getKey() + " = " + getValue().getText();
}

/**
 * Returns the key of a single meta information.
 *
 * @return Key.
 */
const std::string& Meta::getKey() const
{
	return _key->getString();
}

/**
 * Returns the token iterator of the key of a single meta information.
 *
 * @return Key.
 */
TokenIt Meta::getKeyTokenIt() const
{
	return _key;
}

/**
 * Returns the value of a single meta information.
 *
 * @return Value.
 */
const Literal& Meta::getValue() const
{
	return _value->getLiteral();
}

/**
 * Returns the token iterator of the value of a single meta information.
 *
 * @return Key.
 */
TokenIt Meta::getValueTokenIt() const
{
	return _value;
}

/**
 * Set the key of a single meta information.
 *
 * @param key Key.
 */
void Meta::setKey(const std::string& key)
{
	_key->setValue(key);
}

/**
 * Sets the value of a single meta information.
 *
 * @param value Value.
 */
void Meta::setValue(const Literal& value)
{
	_value->setValue(value);
}

}
