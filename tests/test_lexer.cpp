#define CATCH_CONFIG_MAIN
#include "file.h"
#include "lexer.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("get_next_token basic", "[lexer]") {
  char test_data[] = "== =";
  file f(
      test_data,
      sizeof(test_data)); // Assuming file can be constructed from const char*
  cc::lexer l(f);
  cc::token t1 = l.get_next_token(f);
  REQUIRE(t1.m_token_class == cc::token_class::EQ_OP);
  cc::token t2 = l.get_next_token(f);
  REQUIRE(t2.m_token_class == '=');
  cc::token t3 = l.get_next_token(f);
  REQUIRE(t3.m_token_class == cc::token_class::T_EOF);
}
