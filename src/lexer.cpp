//
// Created by Nikolai Linetskii on 01.10.2025.
//

#include "lexer.h"

#include <format>
#include <string>

static inline bool is_space(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f';
}

static inline bool is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline bool is_digit(char c) { return c >= '0' && c <= '9'; }

static inline bool is_identifier_char(char c) {
  return is_alpha(c) || is_digit(c) || c == '_';
}

static inline bool is_hex_digit(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

static inline bool is_oct_digit(char c) { return c >= '0' && c <= '7'; }

static inline bool is_binary_digit(char c) { return c == '0' || c == '1'; }

static inline bool is_keyword(std::string_view word) {
  return word == "auto" || word == "break" || word == "case" ||
         word == "char" || word == "const" || word == "continue" ||
         word == "default" || word == "do" || word == "double" ||
         word == "else" || word == "enum" || word == "extern" ||
         word == "float" || word == "for" || word == "goto" || word == "if" ||
         word == "int" || word == "long" || word == "restrict" ||
         word == "return" || word == "short" || word == "signed" ||
         word == "sizeof" || word == "static" || word == "struct" ||
         word == "switch" || word == "typedef" || word == "union" ||
         word == "unsigned" || word == "void" || word == "volatile" ||
         word == "while";
}

static bool is_integer_suffix(char c) {
  return c == 'u' || c == 'U' || c == 'l' || c == 'L';
}

static bool is_float_suffix(char c) {
  return c == 'f' || c == 'F' || c == 'l' || c == 'L';
}

static bool is_float_exponent(char c) { return c == 'e' || c == 'E'; }

static inline bool validate_integer_suffix(std::string_view suffix) {
  if (suffix.empty()) {
    return true;
  }
  if (suffix == "u" || suffix == "U" || suffix == "l" || suffix == "L" ||
      suffix == "ul" || suffix == "uL" || suffix == "Ul" || suffix == "UL" ||
      suffix == "lu" || suffix == "lU" || suffix == "Lu" || suffix == "LU") {
    return true;
  }
  return false;
}

static inline bool validate_float_suffix(std::string_view suffix) {
  if (suffix.empty()) {
    return true;
  }
  if (suffix == "f" || suffix == "F" || suffix == "l" || suffix == "L") {
    return true;
  }
  return false;
}

namespace cc {
token lexer::get_next_token() {
  if (m_file.is_eof()) {
    return token{token_class::T_EOF};
  }

  bool comment_found = false;
  do {
    while (is_space(m_file.peek())) {
      move_next();
    }
    if (m_file.peek() == '/') {
      move_next();
      if (m_file.peek() == '/') {
        process_single_line_comment();
        comment_found = true;
      } else if (m_file.peek() == '*') {
        process_multi_line_comment();
        comment_found = true;
      } else {
        move_back();
        comment_found = false;
      }
    }
  } while (comment_found);

  if (m_file.is_eof()) {
    return token{token_class::T_EOF};
  }

  char *tok_start = m_file.pos();
  switch (m_file.peek()) {
  case '=':
    move_next();
    if (m_file.peek() == '=') {
      move_next();
      return {token_class::EQ_OP, tok_start, m_file.pos()};
    } else {
      return token{*tok_start};
    }
    break;
  case '.':
    move_next();
    if (m_file.peek() == '.') {
      move_next();
      if (m_file.peek() == '.') {
        move_next();
        return {token_class::ELLIPSIS, tok_start, m_file.pos()};
      } else {
        throw std::runtime_error("Unexpected character after '..'");
      }
    } else if (is_digit(m_file.peek())) {
      // It's a floating point number starting with .digit
      move_back();
      token tok;
      if (parse_decimal_number(tok)) {
        return tok;
      } else {
        throw std::runtime_error("Invalid number literal");
      }
    } else {
      return token{*tok_start};
    }
  case '>':
    move_next();
    if (m_file.peek() == '>') {
      move_next();
      if (m_file.peek() == '=') {
        move_next();
        return {token_class::RIGHT_ASSIGN, tok_start, m_file.pos()};
      } else {
        return {token_class::RIGHT_OP, tok_start, m_file.pos()};
      }
    } else if (m_file.peek() == '=') {
      move_next();
      return {token_class::GE_OP, tok_start, m_file.pos()};
    } else {
      return token{*tok_start};
    }
    break;
  case '<':
    move_next();
    if (m_file.peek() == '<') {
      move_next();
      if (m_file.peek() == '=') {
        move_next();
        return {token_class::LEFT_ASSIGN, tok_start, m_file.pos()};
      } else {
        return {token_class::LEFT_OP, tok_start, m_file.pos()};
      }
    } else if (m_file.peek() == '=') {
      move_next();
      return {token_class::LE_OP, tok_start, m_file.pos()};
    } else if (m_file.peek() == '%') {
      move_next();
      return token{'{'};
    } else if (m_file.peek() == ':') {
      move_next();
      return token{'['};
    } else {
      return token{*tok_start};
    }
    break;
  case '+':
    move_next();
    if (m_file.peek() == '+') {
      move_next();
      return {token_class::INC_OP, tok_start, m_file.pos()};
    } else if (m_file.peek() == '=') {
      move_next();
      return {token_class::ADD_ASSIGN, tok_start, m_file.pos()};
    } else {
      return token{*tok_start};
    }
    break;
  case '-':
    move_next();
    if (m_file.peek() == '-') {
      move_next();
      return {token_class::DEC_OP, tok_start, m_file.pos()};
    } else if (m_file.peek() == '=') {
      move_next();
      return {token_class::SUB_ASSIGN, tok_start, m_file.pos()};
    } else if (m_file.peek() == '>') {
      move_next();
      return {token_class::PTR_OP, tok_start, m_file.pos()};
    } else {
      return token{*tok_start};
    }
    break;
  case '*':
    move_next();
    if (m_file.peek() == '=') {
      move_next();
      return {token_class::MUL_ASSIGN, tok_start, m_file.pos()};
    } else {
      return token{*tok_start};
    }
    break;
  case '/':
    move_next();
    if (m_file.peek() == '=') {
      move_next();
      return {token_class::DIV_ASSIGN, tok_start, m_file.pos()};
    } else {
      return token{*tok_start};
    }
    break;
  case '%':
    move_next();
    if (m_file.peek() == '=') {
      move_next();
      return {token_class::MOD_ASSIGN, tok_start, m_file.pos()};
    } else if (m_file.peek() == '%') {
      move_next();
      return token{'}'};
    } else {
      return token{*tok_start};
    }
    break;
  case '&':
    move_next();
    if (m_file.peek() == '&') {
      move_next();
      return {token_class::AND_OP, tok_start, m_file.pos()};
    } else if (m_file.peek() == '=') {
      move_next();
      return {token_class::AND_ASSIGN, tok_start, m_file.pos()};
    } else {
      return token{*tok_start};
    }
    break;
  case '|':
    move_next();
    if (m_file.peek() == '|') {
      move_next();
      return {token_class::OR_OP, tok_start, m_file.pos()};
    } else if (m_file.peek() == '=') {
      move_next();
      return {token_class::OR_ASSIGN, tok_start, m_file.pos()};
    } else {
      return token{*tok_start};
    }
    break;
  case '^':
    move_next();
    if (m_file.peek() == '=') {
      move_next();
      return {token_class::XOR_ASSIGN, tok_start, m_file.pos()};
    } else {
      return token{*tok_start};
    }
    break;
  case '!':
    move_next();
    if (m_file.peek() == '=') {
      move_next();
      return {token_class::NE_OP, tok_start, m_file.pos()};
    } else {
      return token{*tok_start};
    }
    break;
  case ':':
    move_next();
    if (m_file.peek() == '>') {
      move_next();
      return token{']'};
    } else {
      return token{*tok_start};
    }
  case ';':
  case '{':
  case '}':
  case '(':
  case ')':
  case ',':
  case '[':
  case ']':
  case '~':
  case '?':
    move_next();
    return token{*tok_start};
    break;
  }

  token tok;
  if (parse_identifier_or_keyword(tok)) {
    return tok;
  }

  if (parse_string_literal(tok)) {
    return tok;
  }

  if (parse_char_literal(tok)) {
    return tok;
  }

  if (is_digit(*tok_start)) {
    if (parse_decimal_number(tok) || parse_octal_number(tok) ||
        parse_hex_number(tok)) {
      return tok;
    } else {
      throw std::runtime_error("Invalid number literal");
    }
  }

  throw std::runtime_error(
      std::format("Unexpected character with code '{:d}' at {}:{}", *tok_start,
                  m_line + 1, m_column + 1));
}

int lexer::move_next() {
  int c = m_file.get();
  if (c == '\n') {
    m_line++;
    m_column = 0;
  } else {
    m_column++;
  }
  return c;
}

void lexer::move_back() {
  if (m_line == 0 && m_column == 0) {
    return;
  }
  m_file.unget();
  if (m_file.peek() == '\n') {
    char *line_start = m_file.pos() - 1;
    while (line_start > m_file.begin() && *line_start != '\n') {
      line_start--;
    }
    m_column = static_cast<int>(m_file.pos() - line_start);
    if (line_start == m_file.begin()) {
      m_column++;
    }
  } else {
    m_column--;
  }
}

bool lexer::parse_identifier_or_keyword(token &tok) {
  char *tok_start = m_file.pos();
  if (is_alpha(m_file.peek()) || m_file.peek() == '_') {
    if (m_file.peek() == 'L') {
      if (parse_string_literal(tok) || parse_char_literal(tok)) {
        return true;
      }
    }
    while (is_identifier_char(m_file.peek())) {
      move_next();
    }
    if (is_keyword({tok_start, m_file.pos()})) {
      tok = {token_class::KEYWORD, tok_start, m_file.pos()};
    }
    tok = {token_class::IDENTIFIER, tok_start, m_file.pos()};
    return true;
  }
  return false;
}

bool lexer::parse_string_literal(token &tok) {
  char *tok_start = m_file.pos();
  if (m_file.peek() == 'L') {
    move_next();
    if (m_file.peek() != '"') {
      move_back();
      return false;
    }
  }
  if (m_file.peek() == '"') {
    move_next();
    while (m_file.peek() != '"' && !m_file.is_eof()) {
      if (m_file.peek() == '\\') {
        move_next(); // Skip the escape character
        if (m_file.is_eof()) {
          break;
        }
      }
      move_next();
    }
    if (m_file.peek() == '"') {
      move_next();
      tok = {token_class::STRING_LITERAL, tok_start, m_file.pos()};
      return true;
    } else {
      throw std::runtime_error(
          std::format("Unterminated string literal at {:d}:{:d}", m_line + 1,
                      m_column + 1));
    }
  }

  return false;
}

bool lexer::parse_char_literal(token &tok) {
  char *tok_start = m_file.pos();
  if (m_file.peek() == '\'') {
    move_next();
    while (m_file.peek() != '\'' && !m_file.is_eof()) {
      if (m_file.peek() == '\\') {
        move_next(); // Skip the escape character
        if (m_file.is_eof()) {
          break;
        }
      }
      move_next();
    }
    if (m_file.peek() == '\'') {
      move_next();
      if (m_file.pos() - tok_start == 2) {
        throw std::runtime_error(std::format(
            "Empty character literal at {:d}:{:d}", m_line + 1, m_column + 1));
      }
      tok = {token_class::CHAR_CONSTANT, tok_start, m_file.pos()};
      return true;
    } else {
      throw std::runtime_error(
          std::format("Unterminated character literal at {:d}:{:d}", m_line + 1,
                      m_column + 1));
    }
  }

  return false;
}

bool lexer::parse_decimal_number(token &tok) {
  auto tok_class = token_class::INT_CONSTANT;
  char *tok_start = m_file.pos();
  if (!is_digit(m_file.peek()) && m_file.peek() != '.') {
    return false;
  }
  if (m_file.peek() == '0') {
    move_next();
    if (is_digit(m_file.peek()) || m_file.peek() == 'x' ||
        m_file.peek() == 'X') {
      move_back();
      return false;
    }
  }

  while (is_digit(m_file.peek())) {
    move_next();
  }
  auto integer_part = std::string_view(tok_start, m_file.pos());
  if (m_file.peek() == '.' || is_float_exponent(m_file.peek())) {
    auto frac_part = std::string_view();
    if (m_file.peek() == '.') {
      move_next();
      auto frac_start = m_file.pos();
      while (is_digit(m_file.peek())) {
        move_next();
      }
      frac_part = std::string_view(frac_start, m_file.pos());
    }
    if (integer_part.empty() && frac_part.empty()) {
      throw std::runtime_error("Invalid float literal");
    }
    if (m_file.peek() == 'e' || m_file.peek() == 'E') {
      move_next();
      if (m_file.peek() == '+' || m_file.peek() == '-') {
        move_next();
      }
      if (!is_digit(m_file.peek())) {
        throw std::runtime_error("Invalid float exponent");
      }
      while (is_digit(m_file.peek())) {
        move_next();
      }
    }
    tok_class = token_class::FLOAT_CONSTANT;
  }
  parse_number_suffix(tok_class);
  tok = {tok_class, tok_start, m_file.pos()};
  return true;
}

bool lexer::parse_octal_number(token &tok) {
  if (m_file.peek() != '0') {
    return false;
  }
  char *tok_start = m_file.pos();
  move_next();
  if (!is_oct_digit(m_file.peek())) {
    move_back();
    return false;
  }
  move_next();
  while (is_digit(m_file.peek())) {
    if (!is_oct_digit(m_file.peek())) {
      throw std::runtime_error("Invalid octal digit");
    }
    move_next();
  }
  parse_number_suffix(token_class::OCT_CONSTANT);
  tok = {token_class::OCT_CONSTANT, tok_start, m_file.pos()};
  return true;
}

bool lexer::parse_hex_number(token &tok) {
  if (m_file.peek() != '0') {
    return false;
  }
  char *tok_start = m_file.pos();

  move_next();
  if (m_file.peek() != 'x' && m_file.peek() != 'X') {
    move_back();
    return false;
  }
  move_next();

  if (!is_hex_digit(m_file.peek())) {
    throw std::runtime_error("Invalid hex digit");
  }

  while (is_hex_digit(m_file.peek())) {
    move_next();
  }
  parse_number_suffix(token_class::HEX_CONSTANT);
  tok = {token_class::HEX_CONSTANT, tok_start, m_file.pos()};
  return true;
}

void lexer::parse_number_suffix(int tok_class) {
  char *suffix_start = m_file.pos();
  while (is_alpha(m_file.peek())) {
    move_next();
  }
  auto suffix = std::string_view(suffix_start, m_file.pos());
  if (!suffix.empty()) {
    if (tok_class == token_class::INT_CONSTANT ||
        tok_class == token_class::OCT_CONSTANT ||
        tok_class == token_class::HEX_CONSTANT ||
        tok_class == token_class::BIN_CONSTANT) {
      if (!validate_integer_suffix(suffix)) {
        throw std::runtime_error("Invalid integer literal suffix");
      }
    } else if (tok_class == token_class::FLOAT_CONSTANT) {
      if (!validate_float_suffix(suffix)) {
        throw std::runtime_error("Invalid float literal suffix");
      }
    } else {
      throw std::runtime_error("Invalid literal suffix");
    }
  }
}

void lexer::process_single_line_comment() {
  while (m_file.peek() != '\n' && !m_file.is_eof()) {
    move_next();
  }
}

void lexer::process_multi_line_comment() {
  while (!m_file.is_eof()) {
    if (m_file.peek() == '*') {
      move_next();
      if (m_file.peek() == '/') {
        move_next();
        break;
      }
    } else {
      move_next();
    }
  }
  if (m_file.is_eof()) {
    throw std::runtime_error("Unterminated multi-line comment");
  }
}

} // namespace cc
