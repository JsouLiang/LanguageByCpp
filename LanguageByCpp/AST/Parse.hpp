//
//  Parse.hpp
//  LanguageByCpp
//
//  Created by Liang on 2020/6/18.
//  Copyright © 2020 Liang. All rights reserved.
//

#ifndef Parse_hpp
#define Parse_hpp

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "Token.hpp"

class Parse {
public:
    explicit Parse(const std::string& filename)
    : keywords({{"if", KW_IF},
        {"else", KW_ELSE},
        {"while", KW_WHILE},
        {"null", KW_NULL},
        {"true", KW_TRUE},
        {"false", KW_FALSE},
        {"for", KW_FALSE},
        {"func", KW_FUNC},
        {"return", KW_RETURN},
        {"break", KW_BREAK},
        {"continue", KW_CONTINUE}}) {
            fs.open(filename);
            if (!fs.is_open()) {
                printf("ParserError: can not open source file");
            }
        }
    ~Parse(){
        fs.close();
    }
    
    /**
     读取并消耗一个字符：
     行号++
     */
    char getNextChar();
    
    /**
     查看 fs 流中下一个字符，但不消耗
     */
    char peekNextChar();
    
    static void printLex(const std::string& fileName);

private:
    /// 记录当前分析的行列号
    int line = 1;
    int column = 0;
    
    const std::unordered_map<std::string, Token> keywords;
    
    std::tuple<Token, std::string> next();
    
    std::fstream fs;
    
};

#endif /* Parse_hpp */
