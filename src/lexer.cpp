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
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static inline bool is_digit(char c) { return c >= '0' && c <= '9'; }

namespace cc {
token lexer::get_next_token(file &f) {
  if (f.is_eof()) {
    return token{token_class::T_EOF};
  }

  char *start_pos = nullptr;
  int c = 0;
  do {
    start_pos = f.pos();
    c = move_next();
  } while (is_space(c) && !f.is_eof());

  if (f.is_eof()) {
    return token{token_class::T_EOF};
  }

  switch (c) {
  case '=':
    if (f.peek() == '=') {
      move_next();
      return {token_class::EQ_OP, start_pos, f.pos()};
    } else {
      return token{c};
    }
    break;
  case '.':
    if (f.peek() == '.') {
      move_next();
      if (f.peek() == '.') {
        move_next();
        return {token_class::ELLIPSIS, start_pos, f.pos()};
      } else {
        throw std::runtime_error("Unexpected character after '..'");
      }
    } else {
      return token{c};
    }
  case '>':
    if (f.peek() == '>') {
      move_next();
      if (f.peek() == '=') {
        move_next();
        return {token_class::RIGHT_ASSIGN, start_pos, f.pos()};
      } else {
        return {token_class::RIGHT_OP, start_pos, f.pos()};
      }
    } else if (f.peek() == '=') {
      move_next();
      return {token_class::GE_OP, start_pos, f.pos()};
    } else {
      return token{c};
    }
    break;
  case '<':
    if (f.peek() == '<') {
      move_next();
      if (f.peek() == '=') {
        move_next();
        return {token_class::LEFT_ASSIGN, start_pos, f.pos()};
      } else {
        return {token_class::LEFT_OP, start_pos, f.pos()};
      }
    } else if (f.peek() == '=') {
      move_next();
      return {token_class::LE_OP, start_pos, f.pos()};
    } else if (f.peek() == '%') {
      move_next();
      return token{'{'};
    } else if (f.peek() == ':') {
      move_next();
      return token{'['};
    } else {
      return token{c};
    }
    break;
  case '+':
    if (f.peek() == '+') {
      move_next();
      return {token_class::INC_OP, start_pos, f.pos()};
    } else if (f.peek() == '=') {
      move_next();
      return {token_class::ADD_ASSIGN, start_pos, f.pos()};
    } else {
      return token{c};
    }
    break;
  case '-':
    if (f.peek() == '-') {
      move_next();
      return {token_class::DEC_OP, start_pos, f.pos()};
    } else if (f.peek() == '=') {
      move_next();
      return {token_class::SUB_ASSIGN, start_pos, f.pos()};
    } else if (f.peek() == '>') {
      move_next();
      return {token_class::PTR_OP, start_pos, f.pos()};
    } else {
      return token{c};
    }
    break;
  case '*':
    if (f.peek() == '=') {
      move_next();
      return {token_class::MUL_ASSIGN, start_pos, f.pos()};
    } else {
      return token{c};
    }
    break;
  case '/':
    if (f.peek() == '=') {
      move_next();
      return {token_class::DIV_ASSIGN, start_pos, f.pos()};
    } else {
      return token{c};
    }
    break;
  case '%':
    if (f.peek() == '=') {
      move_next();
      return {token_class::MOD_ASSIGN, start_pos, f.pos()};
    } else if (f.peek() == '%') {
      move_next();
      return token{'}'};
    } else {
      return token{c};
    }
    break;
  case '&':
    if (f.peek() == '&') {
      move_next();
      return {token_class::AND_OP, start_pos, f.pos()};
    } else if (f.peek() == '=') {
      move_next();
      return {token_class::AND_ASSIGN, start_pos, f.pos()};
    } else {
      return token{c};
    }
    break;
  case '|':
    if (f.peek() == '|') {
      move_next();
      return {token_class::OR_OP, start_pos, f.pos()};
    } else if (f.peek() == '=') {
      move_next();
      return {token_class::OR_ASSIGN, start_pos, f.pos()};
    } else {
      return token{c};
    }
    break;
  case '^':
    if (f.peek() == '=') {
      move_next();
      return {token_class::XOR_ASSIGN, start_pos, f.pos()};
    } else {
      return token{c};
    }
    break;
  case '!':
    if (f.peek() == '=') {
      move_next();
      return {token_class::NE_OP, start_pos, f.pos()};
    } else {
      return token{c};
    }
    break;
  case ':':
    if (f.peek() == '>') {
      move_next();
      return token{']'};
    } else {
      return token{c};
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
    return token{c};
    break;
  }

  if (is_alpha(c)) {
    if (c == 'L' && (f.peek() == '"' || f.peek() == '\'')) {
      auto quote = move_next();
      auto token_class =
          quote == '"' ? token_class::STRING_LITERAL : token_class::CONSTANT;
      if (parse_string_or_char_literal(f, quote)) {
        return {token_class, start_pos, f.pos()};
      } else {
        throw std::runtime_error("Unterminated string literal");
      }
    }
    while (is_alpha(f.peek()) || (f.peek() >= '0' && f.peek() <= '9')) {
      move_next();
    }
    return {token_class::IDENTIFIER, start_pos, f.pos()};
  }

  if (c == '"' || c == '\'') {
    auto token_class =
        c == '"' ? token_class::STRING_LITERAL : token_class::CONSTANT;
    if (parse_string_or_char_literal(f, c)) {
      return {token_class, start_pos, f.pos()};
    } else {
      throw std::runtime_error("Unterminated string literal");
    }
  }

  throw std::runtime_error(std::format("Unexpected character '{}'", c));
}

int lexer::move_next() {
  int c = m_file.get();
  if (c == '\n') {
    m_line++;
    m_column = 0;
  } else if (c == '\t') {
    m_column += 8 - (m_column % 8);
  } else {
    m_column++;
  }
  return c;
}

bool lexer::parse_string_or_char_literal(file &f, char quote) {
  while (f.peek() != quote && !f.is_eof()) {
    move_next();
  }
  if (f.peek() == quote) {
    move_next();
    return true;
  }

  return false;
}
} // namespace cc
