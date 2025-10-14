//
// Created by Nikolai Linetskii on 01.10.2025.
//

#include "lexer.h"

#include <format>
#include <string>

static inline bool is_space(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f';
}

namespace cc {
token lexer::get_next_token(file &f) {
  if (f.is_eof()) {
    return token{token_class::T_EOF};
  }

  int c;
  do {
    c = get_next_char();
  } while (is_space(c) && !f.is_eof());

  if (f.is_eof()) {
    return token{token_class::T_EOF};
  }

  char *cur_pos = f.data();
  switch (c) {
  case '=':
    if (f.peek() == '=') {
      get_next_char();
      return token{token_class::EQ_OP, std::string_view(cur_pos, 2)};
    } else {
      return token{c};
    }
    break;
  case '.':
    if (f.peek() == '.') {
      get_next_char();
      if (f.peek() == '.') {
        get_next_char();
        return token{token_class::ELLIPSIS, std::string_view(cur_pos, 3)};
      } else {
        throw std::runtime_error("Unexpected character after '..'");
      }
    } else {
      return token{c};
    }
  case '>':
    if (f.peek() == '>') {
      get_next_char();
      if (f.peek() == '=') {
        get_next_char();
        return token{token_class::RIGHT_ASSIGN, std::string_view(cur_pos, 3)};
      } else {
        return token{token_class::RIGHT_OP, std::string_view(cur_pos, 2)};
      }
    } else if (f.peek() == '=') {
      get_next_char();
      return token{token_class::GE_OP, std::string_view(cur_pos, 2)};
    } else {
      return token{c};
    }
    break;
  case '<':
    if (f.peek() == '<') {
      get_next_char();
      if (f.peek() == '=') {
        get_next_char();
        return token{token_class::LEFT_ASSIGN, std::string_view(cur_pos, 3)};
      } else {
        return token{token_class::LEFT_OP, std::string_view(cur_pos, 2)};
      }
    } else if (f.peek() == '=') {
      get_next_char();
      return token{token_class::LE_OP, std::string_view(cur_pos, 2)};
    } else if (f.peek() == '%') {
      get_next_char();
      return token{'{'};
    } else if (f.peek() == ':') {
      get_next_char();
      return token{'['};
    } else {
      return token{c};
    }
    break;
  case '+':
    if (f.peek() == '+') {
      get_next_char();
      return token{token_class::INC_OP, std::string_view(cur_pos, 2)};
    } else if (f.peek() == '=') {
      get_next_char();
      return token{token_class::ADD_ASSIGN, std::string_view(cur_pos, 2)};
    } else {
      return token{c};
    }
    break;
  case '-':
    if (f.peek() == '-') {
      get_next_char();
      return token{token_class::DEC_OP, std::string_view(cur_pos, 2)};
    } else if (f.peek() == '=') {
      get_next_char();
      return token{token_class::SUB_ASSIGN, std::string_view(cur_pos, 2)};
    } else if (f.peek() == '>') {
      get_next_char();
      return token{token_class::PTR_OP, std::string_view(cur_pos, 2)};
    } else {
      return token{c};
    }
    break;
  case '*':
    if (f.peek() == '=') {
      get_next_char();
      return token{token_class::MUL_ASSIGN, std::string_view(cur_pos, 2)};
    } else {
      return token{c};
    }
    break;
  case '/':
    if (f.peek() == '=') {
      get_next_char();
      return token{token_class::DIV_ASSIGN, std::string_view(cur_pos, 2)};
    } else {
      return token{c};
    }
    break;
  case '%':
    if (f.peek() == '=') {
      get_next_char();
      return token{token_class::MOD_ASSIGN, std::string_view(cur_pos, 2)};
    } else if (f.peek() == '%') {
      get_next_char();
      return token{'}'};
    } else {
      return token{c};
    }
    break;
  case '&':
    if (f.peek() == '&') {
      get_next_char();
      return token{token_class::AND_OP, std::string_view(cur_pos, 2)};
    } else if (f.peek() == '=') {
      get_next_char();
      return token{token_class::AND_ASSIGN, std::string_view(cur_pos, 2)};
    } else {
      return token{c};
    }
    break;
  case '|':
    if (f.peek() == '|') {
      get_next_char();
      return token{token_class::OR_OP, std::string_view(cur_pos, 2)};
    } else if (f.peek() == '=') {
      get_next_char();
      return token{token_class::OR_ASSIGN, std::string_view(cur_pos, 2)};
    } else {
      return token{c};
    }
    break;
  case '^':
    if (f.peek() == '=') {
      get_next_char();
      return token{token_class::XOR_ASSIGN, std::string_view(cur_pos, 2)};
    } else {
      return token{c};
    }
    break;
  case '!':
    if (f.peek() == '=') {
      get_next_char();
      return token{token_class::NE_OP, std::string_view(cur_pos, 2)};
    } else {
      return token{c};
    }
    break;
  case ':':
    if (f.peek() == '>') {
      get_next_char();
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
  default:
    throw std::runtime_error(std::format("Unexpected character '{}'", c));
  }
}

int lexer::get_next_char() {
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
} // namespace cc
