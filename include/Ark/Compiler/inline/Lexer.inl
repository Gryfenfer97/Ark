// check if a given character is a valid hex char
#define CHECK_IF_HEXCHAR(chr) (('a' <= chr && chr <= 'f') || ('A' <= chr && chr <= 'F') || ('0' <= chr && chr <= '9'))

inline TokenType Lexer::guessType(const std::string& value) noexcept
{
    if (value.empty())
        return TokenType::Mismatch;

    // assuming we already detected ()[]{}, strings, shorthands and comments
    if (Utils::isDouble(value))  // works on (\+|-)?[[:digit:]]+(\.[[:digit:]]+)?([e|E](\+|-)?[[:digit]]+)?
        return TokenType::Number;
    else if (isOperator(value))
        return TokenType::Operator;
    else if (isKeyword(value))
        return TokenType::Keyword;
    else if (value[0] == '&' && value.size() > 1 && isIdentifier(value))
        return TokenType::Capture;
    else if (value.size() > 3 && value[0] == value[1] && value[1] == value[2] && value[2] == '.')
        return TokenType::Spread;
    else if (value[0] == '.' && value.size() > 1 && isIdentifier(value))
        return TokenType::GetField;
    // otherwise, identifier if it starts with [a-zA-Z_]
    else if (isIdentifier(value))
        return TokenType::Identifier;
    return TokenType::Mismatch;
}

inline bool Lexer::isKeyword(const std::string& value) noexcept
{
    return std::find(keywords.begin(), keywords.end(), value) != keywords.end();
}

inline bool Lexer::isIdentifier(const std::string& value) noexcept
{
    return utf8valid(value.c_str());
}

inline bool Lexer::isOperator(const std::string& value) noexcept
{
    return std::find(operators.begin(), operators.end(), value) != operators.end();
}

inline bool Lexer::endOfControlChar(const std::string& sequence, char next) noexcept
{
    switch (sequence[0])
    {
        case 'x':
            // \x[any number of hex digits]
            // if it's not a hex digit then it's most likely the end for us
            return !CHECK_IF_HEXCHAR(next);

        case 'u':
            return sequence.size() == 5;

        case 'U':
            return sequence.size() == 9;

        case '"':
        case 'n':
        case 'a':
        case 'b':
        case 't':
        case 'r':
        case 'f':
        case '\\':
        case '0':
            return true;
    }
    return false;
}

inline void Lexer::throwTokenizingError(const std::string& message, const std::string& match, std::size_t line, std::size_t col, const std::string& context)
{
    std::vector<std::string> ctx = Utils::splitString(context, '\n');

    std::stringstream ss;
    ss << message << "\n";
    ss << internal::makeTokenBasedErrorCtx(match, line, col, context);

    throw Ark::SyntaxError(ss.str());
}

#undef CHECK_IF_HEXCHAR
