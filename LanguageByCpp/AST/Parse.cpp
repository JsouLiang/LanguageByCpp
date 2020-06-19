//
//  Parse.cpp
//  LanguageByCpp
//
//  Created by Liang on 2020/6/18.
//  Copyright © 2020 Liang. All rights reserved.
//

#include "Parse.hpp"
#include "Utils.hpp"

inline char Parse::getNextChar() {
    column++;
    return static_cast<char>(fs.get());
}

inline char Parse::peekNextChar() {
    return static_cast<char>(fs.peek());
}

std::tuple<Token, std::string> Parse::next() {
    char currentChar = getNextChar();
    if (currentChar == EOF) {
        return std::make_tuple(TK_EOF, "");
    }
    /// 忽略回车，注释、换行、空格、tab 等无用字符
    /// anyone(args,'a','b','c')
    /// -> args == 'a' || args == 'b' || args == 'c'
    while (anyone(currentChar, ' ', '\n', '\r', '\t')) {
        /// 新的一行
        if (currentChar == '\n') {
            line++; column = 0;
        }
        currentChar = getNextChar();
    }
    if (currentChar == EOF) {
        return std::make_tuple(TK_EOF, "");
    }
    
    /// 以# 开头的单行注释
    while (currentChar == '#') {
        while (currentChar != '\n' && currentChar != EOF) {
            currentChar = getNextChar();
        }
        /// 消除注释后面的空行，比如
        /// #注释
        /// <这是空行>
        /// <这是空行>
        /// #注释
        while (currentChar == '\n') {
            line++; column = 0;
            currentChar = getNextChar();
        }
    }
    
    if (currentChar == EOF) {
        return std::make_tuple(TK_EOF, "");
    }
    
    /// 整数，小数
    if (isNumber(currentChar)) {
        std::string lexeme{currentChar};
        bool isDouble = false;
        char nextChar = peekNextChar();
        while (isNumber(currentChar) || (!isDouble && nextChar == '.') ) {
            if (currentChar == '.') {
                isDouble = true;
            }
            currentChar = getNextChar();
            nextChar = peekNextChar();
            lexeme += currentChar;
        }
        return !isDouble ? std::make_tuple(LIT_INT, lexeme) : std::make_tuple(LIT_DOUBLE, lexeme);
    }
    
    /// 标识符 + 关键字
    if (isLetter(currentChar)) {
        std::string lexeme{currentChar};
        char nextChar = peekNextChar();
        while (isLetter(nextChar) || isNumber(nextChar)) {
            currentChar = getNextChar();
            lexeme += currentChar;
            nextChar = peekNextChar();
        }
        auto result = keywords.find(lexeme);
        return result != keywords.end()
        ? std::make_tuple(result->second, lexeme)   /// 关键字
        : std::make_tuple(TK_IDENT, lexeme);        /// 标识符
    }
    
    /// 字符字面量 'a'
    if (currentChar == '\'') {
        std::string lexeme{getNextChar()};
        if (peekNextChar() != '\'') {
            printf(
                   "SynxaxError: a character literal should surround with "
                   "single-quote");
        }
        currentChar = getNextChar();
        return std::make_tuple(LIT_CHAR, lexeme);
    }
    
    /// 字符串
    if (currentChar == '"') {
        std::string lexeme;
        char nextChar = peekNextChar();
        while (nextChar != '"') {
            currentChar = getNextChar();
            lexeme += currentChar;
            nextChar = peekNextChar();
        }
        /// 吃掉结尾的 "
        currentChar = getNextChar();
        return std::make_tuple(LIT_STR, lexeme);
    }
    
    switch (currentChar) {
        case '[':
            return std::make_tuple(TK_LBRACKET, "[");
        case ']':
            return std::make_tuple(TK_RBRACKET, "]");
        case '{':
            return std::make_tuple(TK_LBRACE, "{");
        case '}':
            return std::make_tuple(TK_RBRACE, "}");
        case '(':
            return std::make_tuple(TK_LPAREN, "(");
        case ')':
            return std::make_tuple(TK_RPAREN, ")");
        case ',':
            return std::make_tuple(TK_COMMA, ",");
        case '~':
            return std::make_tuple(TK_BITNOT, "~");
        case '+':
            if (peekNextChar() == '=') {
                currentChar = getNextChar();
                return std::make_tuple(TK_PLUS_AGN, "+=");
            }
            return std::make_tuple(TK_PLUS, "+");
        case '-':
            if (peekNextChar() == '=') {
                currentChar = getNextChar();
                return std::make_tuple(TK_MINUS_AGN, "-=");
            }
            return std::make_tuple(TK_MINUS, "-");
            
        case '*':
            if (peekNextChar() == '=') {
                currentChar = getNextChar();
                return std::make_tuple(TK_TIMES_AGN, "*=");
            }
            return std::make_tuple(TK_TIMES, "*");
            
        case '/':
            if (peekNextChar() == '=') {
                currentChar = getNextChar();
                return std::make_tuple(TK_DIV_AGN, "/=");
            }
            return std::make_tuple(TK_DIV, "/");
        case '%':
            if (peekNextChar() == '=') {
                currentChar = getNextChar();
                return std::make_tuple(TK_MOD_AGN, "%=");
            }
            return std::make_tuple(TK_MOD, "%");
        case '=':
            if (peekNextChar() == '=') {
                currentChar = getNextChar();
                return std::make_tuple(TK_EQ, "==");
            }
            return std::make_tuple(TK_ASSIGN, "=");
        case '>':
            if (peekNextChar() == '=') {
                currentChar = getNextChar();
                return std::make_tuple(TK_GE, ">=");
            }
            return std::make_tuple(TK_GT, ">");
        case '<':
            if (peekNextChar() == '=') {
                currentChar = getNextChar();
                return std::make_tuple(TK_LE, "<=");
            }
            return std::make_tuple(TK_LT, "<");
        case '!':
            if (peekNextChar() == '=') {
                currentChar = getNextChar();
                return std::make_tuple(TK_NE, "!=");
            }
            return std::make_tuple(TK_LOGNOT, "!");
        case '|':
            if (peekNextChar() == '|') {
                currentChar = getNextChar();
                return std::make_tuple(TK_LOGOR, "||");
            }
            return std::make_tuple(TK_BITOR, "|");
        case '&':
            if (peekNextChar() == '&') {
                currentChar = getNextChar();
                return std::make_tuple(TK_LOGAND, "&&");
            }
            return std::make_tuple(TK_BITAND, "&");
            
        default:
            break;
    }
    /// 没有匹配的 Token，即无效 token
    return std::make_tuple(INVALID, "invalid");
}

void Parse::printLex(const std::string &fileName) {
    Parse parse(fileName);
    std::tuple<Token, std::string> tk;
    do {
        tk = parse.next();
        std::cout << "[" << std::get<0>(tk) << "," << std::get<1>(tk) << "]\n";

    } while (std::get<0>(tk) != TK_EOF);
}
