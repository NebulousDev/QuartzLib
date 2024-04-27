#pragma once

namespace Quartz
{
	/*====================================================
	|                QUARTZLIB WHITESPACE                |
	=====================================================*/

	template<typename CharType>
	inline bool IsWhitespace(const CharType& readChar)
	{
		return readChar == ' ' || readChar == '\n' || readChar == '\t' || readChar == '\r';
	}
}