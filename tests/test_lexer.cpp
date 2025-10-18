#define CATCH_CONFIG_MAIN
#include "file.h"
#include "lexer.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("get_next_token multichar", "[lexer]") {
  char test_data[] =
      "... >>= <<= == != ++ -- && || += -= *= /= %= &= ^= |= <= >=";
  file f(test_data, sizeof(test_data) - 1);
  cc::lexer l(f);

  auto t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::ELLIPSIS);
  REQUIRE(t1.m_value == "...");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::RIGHT_ASSIGN);
  REQUIRE(t1.m_value == ">>=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::LEFT_ASSIGN);
  REQUIRE(t1.m_value == "<<=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::EQ_OP);
  REQUIRE(t1.m_value == "==");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::NE_OP);
  REQUIRE(t1.m_value == "!=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::INC_OP);
  REQUIRE(t1.m_value == "++");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::DEC_OP);
  REQUIRE(t1.m_value == "--");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::AND_OP);
  REQUIRE(t1.m_value == "&&");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::OR_OP);
  REQUIRE(t1.m_value == "||");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::ADD_ASSIGN);
  REQUIRE(t1.m_value == "+=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::SUB_ASSIGN);
  REQUIRE(t1.m_value == "-=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::MUL_ASSIGN);
  REQUIRE(t1.m_value == "*=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::DIV_ASSIGN);
  REQUIRE(t1.m_value == "/=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::MOD_ASSIGN);
  REQUIRE(t1.m_value == "%=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::AND_ASSIGN);
  REQUIRE(t1.m_value == "&=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::XOR_ASSIGN);
  REQUIRE(t1.m_value == "^=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::OR_ASSIGN);
  REQUIRE(t1.m_value == "|=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::LE_OP);
  REQUIRE(t1.m_value == "<=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::GE_OP);
  REQUIRE(t1.m_value == ">=");

  t1 = l.get_next_token();
  REQUIRE(t1.m_token_class == cc::token_class::T_EOF);
}

TEST_CASE("get_next_token strings", "[lexer]") {
  char test_data[] = "\nidentifier another_identifier L\"string literal\" 'c'";
  file f(test_data, sizeof(test_data) - 1);
  cc::lexer l(f);

  auto token = l.get_next_token();
  REQUIRE(token.m_token_class == cc::token_class::IDENTIFIER);
  REQUIRE(token.m_value == "identifier");

  token = l.get_next_token();
  REQUIRE(token.m_token_class == cc::token_class::IDENTIFIER);
  REQUIRE(token.m_value == "another_identifier");

  token = l.get_next_token();
  REQUIRE(token.m_token_class == cc::token_class::STRING_LITERAL);
  REQUIRE(token.m_value == "L\"string literal\"");

  token = l.get_next_token();
  REQUIRE(token.m_token_class == cc::token_class::CHAR_CONSTANT);
  REQUIRE(token.m_value == "'c'");
}

TEST_CASE("get_next_token unexpected character", "[lexer]") {
  char test_data[] = "@";
  file f(test_data, sizeof(test_data));
  cc::lexer l(f);
  REQUIRE_THROWS_AS(l.get_next_token(), std::runtime_error);
}

TEST_CASE("get_next_token valid floats", "[lexer]") {
  std::vector<std::string> valid = {
      "0.0",     "123.456", ".5",   "5.",    "1e10",   "2E-2",
      "3.14e+2", "0.1f",    "1.2F", "1e-3F", "1.0e10", "1.0e-10",
      "1.0e+10", "1.0e10f", "0.",   ".0",    "0e0",    "0.0e0",
      "0.0e+0",  "0.0e-0",  "0.0f", "0.0F"};
  for (const auto &s : valid) {
    file f(const_cast<char *>(s.c_str()), s.size());
    cc::lexer l(f);
    cc::token tok = l.get_next_token();
    REQUIRE(tok.m_token_class == cc::token_class::FLOAT_CONSTANT);
    REQUIRE(tok.m_value == s);
  }
}

TEST_CASE("get_next_token invalid floats", "[lexer]") {
  std::vector<std::string> invalid = {"1e",    "1e+",   "1e-",     "1.0e",
                                      "1.0e+", "1.0e-", "1.0e10x", "1e10e10"};
  for (const auto &s : invalid) {
    file f(const_cast<char *>(s.c_str()), s.size());
    cc::lexer l(f);
    REQUIRE_THROWS_AS(l.get_next_token(), std::runtime_error);
  }
}

TEST_CASE("get_next_token decimal numbers", "[lexer]") {
  char test_data[] = "0 42 123456 987654321";
  file f(test_data, sizeof(test_data) - 1);
  cc::lexer l(f);

  auto t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::INT_CONSTANT);
  REQUIRE(t.m_value == "0");

  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::INT_CONSTANT);
  REQUIRE(t.m_value == "42");

  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::INT_CONSTANT);
  REQUIRE(t.m_value == "123456");

  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::INT_CONSTANT);
  REQUIRE(t.m_value == "987654321");
}

TEST_CASE("get_next_token octal numbers", "[lexer]") {
  char test_data[] = "00 0123 0777";
  file f(test_data, sizeof(test_data) - 1);
  cc::lexer l(f);

  auto t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::OCT_CONSTANT);
  REQUIRE(t.m_value == "00");

  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::OCT_CONSTANT);
  REQUIRE(t.m_value == "0123");

  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::OCT_CONSTANT);
  REQUIRE(t.m_value == "0777");
}

TEST_CASE("get_next_token hex numbers", "[lexer]") {
  char test_data[] = "0x1 0X2A 0xdeadbeef 0XCAFEBABE";
  file f(test_data, sizeof(test_data) - 1);
  cc::lexer l(f);

  auto t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::HEX_CONSTANT);
  REQUIRE(t.m_value == "0x1");

  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::HEX_CONSTANT);
  REQUIRE(t.m_value == "0X2A");

  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::HEX_CONSTANT);
  REQUIRE(t.m_value == "0xdeadbeef");

  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::HEX_CONSTANT);
  REQUIRE(t.m_value == "0XCAFEBABE");
}

TEST_CASE("get_next_token invalid numbers", "[lexer]") {
  std::vector<std::string> edge = {"0x", "0X", "0b", "0xGHI"};
  for (const auto &s : edge) {
    file f(const_cast<char *>(s.c_str()), s.size());
    cc::lexer l(f);
    REQUIRE_THROWS_AS(l.get_next_token(), std::runtime_error);
  }
}

TEST_CASE("get_next_token comments", "[lexer]") {
  char test_data[] = R"(
// This is a single-line comment
int main() {
  /* This is a
     multi-line comment */
  return 0; // Another comment
}
)";
  file f(test_data, sizeof(test_data) - 1);
  cc::lexer l(f);
  auto t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::KEYWORD);
  REQUIRE(t.m_value == "int");
  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::IDENTIFIER);
  REQUIRE(t.m_value == "main");
  t = l.get_next_token();
  REQUIRE(t.m_token_class == static_cast<int>('('));
  t = l.get_next_token();
  REQUIRE(t.m_token_class == static_cast<int>(')'));
  t = l.get_next_token();
  REQUIRE(t.m_token_class == static_cast<int>('{'));
  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::KEYWORD);
  REQUIRE(t.m_value == "return");
  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::INT_CONSTANT);
  REQUIRE(t.m_value == "0");
  t = l.get_next_token();
  REQUIRE(t.m_token_class == static_cast<int>(';'));
  t = l.get_next_token();
  REQUIRE(t.m_token_class == static_cast<int>('}'));
  t = l.get_next_token();
  REQUIRE(t.m_token_class == cc::token_class::T_EOF);
}
