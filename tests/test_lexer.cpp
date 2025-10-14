#define CATCH_CONFIG_MAIN
#include "file.h"
#include "lexer.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("get_next_token multichar", "[lexer]") {
  char test_data[] =
      "... >>= <<= == != ++ -- && || += -= *= /= %= &= ^= |= <= >=";
  file f(test_data, sizeof(test_data));
  cc::lexer l(f);

  auto t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::ELLIPSIS);
  REQUIRE(t1.m_value == "...");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::RIGHT_ASSIGN);
  REQUIRE(t1.m_value == ">>=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::LEFT_ASSIGN);
  REQUIRE(t1.m_value == "<<=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::EQ_OP);
  REQUIRE(t1.m_value == "==");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::NE_OP);
  REQUIRE(t1.m_value == "!=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::INC_OP);
  REQUIRE(t1.m_value == "++");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::DEC_OP);
  REQUIRE(t1.m_value == "--");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::AND_OP);
  REQUIRE(t1.m_value == "&&");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::OR_OP);
  REQUIRE(t1.m_value == "||");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::ADD_ASSIGN);
  REQUIRE(t1.m_value == "+=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::SUB_ASSIGN);
  REQUIRE(t1.m_value == "-=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::MUL_ASSIGN);
  REQUIRE(t1.m_value == "*=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::DIV_ASSIGN);
  REQUIRE(t1.m_value == "/=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::MOD_ASSIGN);
  REQUIRE(t1.m_value == "%=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::AND_ASSIGN);
  REQUIRE(t1.m_value == "&=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::XOR_ASSIGN);
  REQUIRE(t1.m_value == "^=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::OR_ASSIGN);
  REQUIRE(t1.m_value == "|=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::LE_OP);
  REQUIRE(t1.m_value == "<=");

  t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::GE_OP);
  REQUIRE(t1.m_value == ">=");
}

TEST_CASE("get_next_token right assign", "[lexer]") {
  char test_data[] = ">>=";
  file f(
      test_data,
      sizeof(test_data)); // Assuming file can be constructed from const char*
  cc::lexer l(f);
  cc::token t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::RIGHT_ASSIGN);
  cc::token t2 = l.get_next_token(f);
  REQUIRE(t2.m_token_class == cc::token_class::T_EOF);
}

TEST_CASE("get_next_token unexpected character", "[lexer]") {
  char test_data[] = "@";
  file f(
      test_data,
      sizeof(test_data)); // Assuming file can be constructed from const char*
  cc::lexer l(f);
  REQUIRE_THROWS_AS(l.get_next_token(f), std::runtime_error);
}
