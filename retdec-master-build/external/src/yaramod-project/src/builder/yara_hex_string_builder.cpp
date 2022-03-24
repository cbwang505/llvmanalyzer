#include "yaramod/builder/yara_hex_string_builder.h"
#include "yaramod/utils/utils.h"

namespace yaramod {

/**
 * Default constructor preparing HexStringBuilder.
 */
YaraHexStringBuilder::YaraHexStringBuilder()
	: _tokenStream(std::make_shared<TokenStream>())
{
}

/**
 * Constructor for creating byte without any wildcard.
 *
 * @param byte Byte.
 */
YaraHexStringBuilder::YaraHexStringBuilder(std::uint8_t byte)
	: _tokenStream(std::make_shared<TokenStream>())
	, _units()
{
	std::uint64_t b1 = (byte & 0xF0) >> 4;
	TokenIt t1 = _tokenStream->emplace_back(HEX_NIBBLE, b1, numToStr(b1, std::hex, false, true));
	std::uint64_t b2 = (byte & 0x0F);
	TokenIt t2 = _tokenStream->emplace_back(HEX_NIBBLE, b2, numToStr(b2, std::hex, false, true));
	_units.push_back(std::make_shared<HexStringNibble>(t1));
	_units.push_back(std::make_shared<HexStringNibble>(t2));
}

/**
 * Constructor for creating sequence of bytes without any wildcard.
 *
 * @param bytes Bytes.
 */
YaraHexStringBuilder::YaraHexStringBuilder(const std::vector<std::uint8_t>& bytes)
	: _tokenStream(std::make_shared<TokenStream>())
	, _units()
{
	_units.reserve(2 * bytes.size());
	for (auto byte : bytes)
	{
		std::uint64_t b1 = (byte & 0xF0) >> 4;
		TokenIt t1 = _tokenStream->emplace_back(HEX_NIBBLE, b1, numToStr(b1, std::hex, false, true));
		std::uint64_t b2 = (byte & 0x0F);
		TokenIt t2 = _tokenStream->emplace_back(HEX_NIBBLE, b2, numToStr(b2, std::hex, false, true));
		_units.push_back(std::make_shared<HexStringNibble>(t1));
		_units.push_back(std::make_shared<HexStringNibble>(t2));
	}
}

/**
 * Constructor for creating custom unit.
 *
 * @param unit Hex string unit.
 */
YaraHexStringBuilder::YaraHexStringBuilder(const std::shared_ptr<HexStringUnit>& unit)
	: _tokenStream(std::make_shared<TokenStream>())
	, _units()
{
	_units.push_back(unit);
}

/**
 * Constructor for creating custom unit.
 *
 * @param unit Hex string unit.
 */
YaraHexStringBuilder::YaraHexStringBuilder(std::shared_ptr<HexStringUnit>&& unit)
	: _tokenStream(std::make_shared<TokenStream>())
	, _units()
{
	_units.push_back(std::move(unit));
}

/**
 * Constructor for creating custom units.
 *
 * @param units Hex string units.
 */
YaraHexStringBuilder::YaraHexStringBuilder(const std::vector<std::shared_ptr<HexStringUnit>>& units)
	: _tokenStream(std::make_shared<TokenStream>())
 	, _units(units)
{
}

/**
 * Constructor for creating custom units.
 *
 * @param units Hex string units.
 */
YaraHexStringBuilder::YaraHexStringBuilder(std::vector<std::shared_ptr<HexStringUnit>>&& units)
	: _tokenStream(std::make_shared<TokenStream>())
	, _units(std::move(units))
{
}

/**
 * Constructor assigning builder existing TokenStream.
 *
 * @param ts TokenStream.
 */
YaraHexStringBuilder::YaraHexStringBuilder(const std::shared_ptr<TokenStream>& ts)
	: _tokenStream(ts)
{
}

/**
 * Constructor for creating byte without any wildcard.
 *
 * @param ts TokenStream.
 * @param byte Byte.
 */
YaraHexStringBuilder::YaraHexStringBuilder(const std::shared_ptr<TokenStream>& ts, std::uint8_t byte)
	: _tokenStream(ts)
	, _units()
{
	std::uint64_t b1 = (byte & 0xF0) >> 4;
	TokenIt t1 = _tokenStream->emplace_back(HEX_NIBBLE, b1, numToStr(b1, std::hex, false, true));
	std::uint64_t b2 = (byte & 0x0F);
	TokenIt t2 = _tokenStream->emplace_back(HEX_NIBBLE, b2, numToStr(b2, std::hex, false, true));
	_units.push_back(std::make_shared<HexStringNibble>(t1));
	_units.push_back(std::make_shared<HexStringNibble>(t2));
}

/**
 * Constructor for creating sequence of bytes without any wildcard.
 *
 * @param ts TokenStream.
 * @param bytes Bytes.
 */
YaraHexStringBuilder::YaraHexStringBuilder(const std::shared_ptr<TokenStream>& ts, const std::vector<std::uint8_t>& bytes)
	: _tokenStream(ts)
	, _units()
{
	_units.reserve(2 * bytes.size());
	for (auto byte : bytes)
	{
		std::uint64_t b1 = (byte & 0xF0) >> 4;
		TokenIt t1 = _tokenStream->emplace_back(HEX_NIBBLE, b1, numToStr(b1, std::hex, false, true));
		std::uint64_t b2 = (byte & 0x0F);
		TokenIt t2 = _tokenStream->emplace_back(HEX_NIBBLE, b2, numToStr(b2, std::hex, false, true));
		_units.push_back(std::make_shared<HexStringNibble>(t1));
		_units.push_back(std::make_shared<HexStringNibble>(t2));
	}
}

/**
 * Constructor for creating custom unit.
 *
 * @param ts TokenStream.
 * @param unit Hex string unit.
 */
YaraHexStringBuilder::YaraHexStringBuilder(const std::shared_ptr<TokenStream>& ts, const std::shared_ptr<HexStringUnit>& unit)
	: _tokenStream(ts)
	, _units()
{
	_units.push_back(unit);
}

/**
 * Constructor for creating custom unit.
 *
 * @param ts TokenStream.
 * @param unit Hex string unit.
 */
YaraHexStringBuilder::YaraHexStringBuilder(const std::shared_ptr<TokenStream>& ts, std::shared_ptr<HexStringUnit>&& unit)
	: _tokenStream(ts)
	, _units()
{
	_units.push_back(std::move(unit));
}

/**
 * Constructor for creating custom units.
 *
 * @param ts TokenStream.
 * @param units Hex string units.
 */
YaraHexStringBuilder::YaraHexStringBuilder(const std::shared_ptr<TokenStream>& ts, const std::vector<std::shared_ptr<HexStringUnit>>& units)
	: _tokenStream(ts)
	, _units(units)
{
}

/**
 * Constructor for creating custom units.
 *
 * @param ts TokenStream.
 * @param units Hex string units.
 */
YaraHexStringBuilder::YaraHexStringBuilder(const std::shared_ptr<TokenStream>& ts, std::vector<std::shared_ptr<HexStringUnit>>&& units)
	: _tokenStream(ts)
	, _units(std::move(units))
{
}

/**
 * Returns the built hex string and resets the builder back to default state.
 *
 * @param acceptor TokenStream to move-append all our tokens.
 * @return Built hex string.
 */
std::shared_ptr<HexString> YaraHexStringBuilder::get(const std::shared_ptr<TokenStream>& acceptor /*= nullptr*/, bool addHexParentheses /*= true*/) const
{
	if (addHexParentheses)
	{
		_tokenStream->emplace(_tokenStream->begin(), HEX_START_BRACKET, "{");
		_tokenStream->emplace_back(HEX_END_BRACKET, "}");
	}
	if (acceptor)
	{
		acceptor->moveAppend(_tokenStream.get());
		return std::make_shared<HexString>(acceptor, _units);
	}
	else
		return std::make_shared<HexString>(std::move(_tokenStream), _units);
}

/**
 * Returns the units already present in the hex string.
 *
 * @return Hex string units.
 */
const std::vector<std::shared_ptr<HexStringUnit>>& YaraHexStringBuilder::getUnits() const
{
	return _units;
}

/**
 * Creates the full wildcard unit.
 *
 * For example:
 * @code
 * ??
 * @endcode
 *
 * @return Builder.
 */
YaraHexStringBuilder wildcard()
{
	auto ts = std::make_shared<TokenStream>();
	std::vector<std::shared_ptr<HexStringUnit>> units;
	auto token1 = ts->emplace_back(HEX_WILDCARD_LOW, "?");
	auto token2 = ts->emplace_back(HEX_WILDCARD_HIGH, "?");
	units.push_back(std::make_shared<HexStringWildcard>(token1));
	units.push_back(std::make_shared<HexStringWildcard>(token2));
	return YaraHexStringBuilder(ts, std::move(units));
}

/**
 * Creates the wildcard unit with wildcard on low nibble.
 *
 * For example:
 * @code
 * 9?
 * @endcode
 *
 * @param high High nibble.
 *
 * @return Builder.
 */
YaraHexStringBuilder wildcardLow(std::uint8_t high)
{
	auto ts = std::make_shared<TokenStream>();
	std::vector<std::shared_ptr<HexStringUnit>> units;
	std::uint64_t b1 = (high & 0x0F);
	TokenIt token1 = ts->emplace_back(HEX_NIBBLE, b1, numToStr(b1, std::hex, false, true));
	units.push_back(std::make_shared<HexStringNibble>(token1));
	TokenIt token2 = ts->emplace_back(HEX_WILDCARD_LOW, "?");
	units.push_back(std::make_shared<HexStringWildcard>(token2));
	return YaraHexStringBuilder(ts, std::move(units));
}

/**
 * Creates the wildcard unit with wildcard on high nibble.
 *
 * For example:
 * @code
 * ?9
 * @endcode
 *
 * @param low Low nibble.
 *
 * @return Builder.
 */
YaraHexStringBuilder wildcardHigh(std::uint8_t low)
{
	auto ts = std::make_shared<TokenStream>();
	std::vector<std::shared_ptr<HexStringUnit>> units;
	TokenIt token1 = ts->emplace_back(HEX_WILDCARD_HIGH, "?");
	units.push_back(std::make_shared<HexStringWildcard>(token1));
	std::uint64_t b2 = (low & 0x0F);
	TokenIt token2 = ts->emplace_back(HEX_NIBBLE, b2, numToStr(b2, std::hex, false, true));
	units.push_back(std::make_shared<HexStringNibble>(token2));
	return YaraHexStringBuilder(ts, std::move(units));
}

/**
 * Creates the jump unit with no low or high bound.
 *
 * For example:
 * @code
 * [-]
 * @endcode
 *
 * @return Builder.
 */
YaraHexStringBuilder jumpVarying()
{
	auto ts = std::make_shared<TokenStream>();
	ts->emplace_back(HEX_JUMP_LEFT_BRACKET, "[");
	ts->emplace_back(DASH, "-");
	ts->emplace_back(HEX_JUMP_RIGHT_BRACKET, "]");
	return YaraHexStringBuilder(ts, std::make_shared<HexStringJump>());
}

/**
 * Creates the fixed jump unit.
 *
 * For example:
 * @code
 * [5]
 * @endcode
 *
 * @return Builder.
 */
YaraHexStringBuilder jumpFixed(std::uint64_t value)
{
	auto ts = std::make_shared<TokenStream>();
	ts->emplace_back(HEX_JUMP_LEFT_BRACKET, "[");
	TokenIt t = ts->emplace_back(HEX_NIBBLE, value);
	ts->emplace_back(HEX_JUMP_RIGHT_BRACKET, "]");

	return YaraHexStringBuilder(ts, std::make_shared<HexStringJump>(t, t));
}

/**
 * Creates the jump unit with just low bound set.
 *
 * For example:
 * @code
 * [5-]
 * @endcode
 *
 * @return Builder.
 */
YaraHexStringBuilder jumpVaryingRange(std::uint64_t low)
{
	auto ts = std::make_shared<TokenStream>();
	ts->emplace_back(HEX_JUMP_LEFT_BRACKET, "[");
	TokenIt t = ts->emplace_back(HEX_NIBBLE, low);
	ts->emplace_back(DASH, "-");
	ts->emplace_back(HEX_JUMP_RIGHT_BRACKET, "]");

	return YaraHexStringBuilder(ts, std::make_shared<HexStringJump>(t));
}

/**
 * Creates the jump unit with low and high bound set.
 *
 * For example:
 * @code
 * [5-7]
 * @endcode
 *
 * @return Builder.
 */
YaraHexStringBuilder jumpRange(std::uint64_t low, std::uint64_t high)
{
	auto ts = std::make_shared<TokenStream>();
	ts->emplace_back(HEX_JUMP_LEFT_BRACKET, "[");
	TokenIt t1 = ts->emplace_back(HEX_NIBBLE, low);
	ts->emplace_back(DASH, "-");
	TokenIt t2 = ts->emplace_back(HEX_NIBBLE, high);
	ts->emplace_back(HEX_JUMP_RIGHT_BRACKET, "]");

	return YaraHexStringBuilder(ts, std::make_shared<HexStringJump>(t1, t2));
}

/**
 * Creates the alternative between multiple hex string units.
 *
 * For example:
 * @code
 * ( 11 | 22 | 33 )
 * @endcode
 *
 * @param units Units.
 *
 * @return Builder.
 */
template <>
YaraHexStringBuilder alt(const std::vector<YaraHexStringBuilder>& units)
{
	std::vector<std::shared_ptr<HexString>> hexStrings;
	hexStrings.reserve(units.size());

	auto ts = std::make_shared<TokenStream>();
	for (std::size_t i = 0; i < units.size(); ++i)
	{
		hexStrings.push_back(units[i].get(ts, false)); //filling up ts while getting the hexStrings
		if (i + 1 < units.size()) {
			ts->emplace_back(HEX_ALT, "|"); // add '|' in between the hexStrings
		}
	}
	return YaraHexStringBuilder(ts, std::make_shared< HexStringOr >(hexStrings));
}

YaraHexStringBuilder _alt(const std::shared_ptr<TokenStream>& ts, std::vector<std::shared_ptr<HexString>>& hexStrings, const YaraHexStringBuilder& unit)
{
	hexStrings.push_back(unit.get(ts, false));
	ts->emplace_back(HEX_ALT_RIGHT_BRACKET, ")");
	return YaraHexStringBuilder(ts, std::make_shared< HexStringOr >(hexStrings));
}

}

