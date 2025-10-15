//
// Created by Nikolai Linetskii on 01.10.2025.
//

#ifndef CPPPROJECT_LEXER_H
#define CPPPROJECT_LEXER_H
#include <string>

#include "file.h"

namespace cc {
enum token_class {
  T_EOF = 255,
  IDENTIFIER = 254,
  KEYWORD = 253,
  CONSTANT = 252,
  STRING_LITERAL = 251,
  ELLIPSIS = 250,     // ...
  RIGHT_ASSIGN = 249, // >>=
  LEFT_ASSIGN = 248,  // <<=
  ADD_ASSIGN = 247,   // +=
  SUB_ASSIGN = 246,   // -=
  MUL_ASSIGN = 245,   // *=
  DIV_ASSIGN = 244,   // /=
  MOD_ASSIGN = 243,   // %=
  AND_ASSIGN = 242,   // &=
  XOR_ASSIGN = 241,   // ^=
  OR_ASSIGN = 240,    // |=
  RIGHT_OP = 239,     // >>
  LEFT_OP = 238,      // <<
  INC_OP = 237,       // ++
  DEC_OP = 236,       // --
  PTR_OP = 235,       // ->
  AND_OP = 234,       // &&
  OR_OP = 233,        // ||
  LE_OP = 232,        // <=
  GE_OP = 231,        // >=
  EQ_OP = 230,        // ==
  NE_OP = 229         // !=
};

struct token {
  explicit token(int token_class) : m_token_class(token_class) {}
  token(int token_class, const char *start, const char *end)
      : m_token_class(token_class), m_value(start, end) {}
  token(int token_class, std::string &&buffer)
      : m_token_class(token_class), m_buffer(std::move(buffer)) {
    m_value = m_buffer;
  }

  int m_token_class = token_class::T_EOF;
  std::string_view m_value;
  std::string m_buffer;
};

class lexer {
public:
  explicit lexer(file &f) : m_file(f) {}
  token get_next_token(file &f);
  bool has_more_tokens() const { return !m_file.is_eof(); }

private:
  int move_next();
  bool parse_string_or_char_literal(file &f, char quote);

private:
  file &m_file;
  int m_line = 0;
  int m_column = 0;
};
} // namespace cc

#endif // CPPPROJECT_LEXER_H
