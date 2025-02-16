// Copyright (C) 2020  Matthew "strager" Glazar
// See end of file for extended copyright information.

#include <array>
#include <cstring>
#include <gtest/gtest.h>
#include <quick-lint-js/characters.h>
#include <quick-lint-js/container/concat.h>
#include <quick-lint-js/container/padded-string.h>
#include <quick-lint-js/fe/source-code-span.h>
#include <quick-lint-js/port/char8.h>
#include <quick-lint-js/util/algorithm.h>
#include <quick-lint-js/util/narrow-cast.h>
#include <quick-lint-js/web-demo-location.h>
#include <vector>

namespace quick_lint_js {
namespace {
TEST(Test_Web_Demo_Location, ranges_on_first_line) {
  Padded_String code(u8"let x = 2;"_sv);
  Web_Demo_Locator l(&code);
  Web_Demo_Source_Range x_range = l.range(Source_Code_Span(&code[4], &code[5]));

  EXPECT_EQ(x_range.begin, 4);
  EXPECT_EQ(x_range.end, 5);
}

TEST(Test_Web_Demo_Location, ranges_on_second_line) {
  for (String8_View line_terminator : line_terminators_except_ls_ps) {
    Padded_String code(
        concat(u8"let x = 2;"_sv, line_terminator, u8"let y = 3;"_sv));
    const Char8* y = strchr(code.c_str(), u8'y');
    Web_Demo_Locator l(&code);
    Web_Demo_Source_Range x_range = l.range(Source_Code_Span(y, y + 1));

    EXPECT_EQ(x_range.begin, y - code.c_str());
    EXPECT_EQ(x_range.end, y + 1 - code.c_str());
  }
}

TEST(Test_Web_Demo_Location, lf_cr_is_two_line_terminators) {
  Padded_String code(u8"let x = 2;\n\rlet y = 3;"_sv);
  const Char8* y = strchr(code.c_str(), u8'y');
  Web_Demo_Locator l(&code);
  Web_Demo_Source_Range y_range = l.range(Source_Code_Span(y, y + 1));

  EXPECT_EQ(y_range.begin, y - code.c_str());
}

TEST(Test_Web_Demo_Location, location_after_null_byte) {
  Padded_String code(String8(u8"hello\0beautiful\nworld"_sv));
  const Char8* r = &code[18];
  ASSERT_EQ(*r, u8'r');

  Web_Demo_Locator l(&code);
  Web_Demo_Source_Range r_range = l.range(Source_Code_Span(r, r + 1));

  EXPECT_EQ(r_range.begin, r - code.c_str());
}

TEST(Test_Web_Demo_Location, position_backwards) {
  Padded_String code(u8"ab\nc\n\nd\nefg\nh"_sv);

  std::vector<Web_Demo_Source_Offset> expected_positions;
  {
    Web_Demo_Locator l(&code);
    for (int i = 0; i < narrow_cast<int>(code.size()); ++i) {
      expected_positions.push_back(l.position(&code[i]));
    }
  }

  std::vector<Web_Demo_Source_Offset> actual_positions;
  {
    Web_Demo_Locator l(&code);
    for (int i = narrow_cast<int>(code.size()) - 1; i >= 0; --i) {
      actual_positions.push_back(l.position(&code[i]));
    }
  }
  reverse(actual_positions);

  EXPECT_EQ(actual_positions, expected_positions);
}

TEST(Test_Web_Demo_Location, position_after_multi_byte_character) {
  // U+2603 has three UTF-8 code units: e2 98 83
  // U+2603 has one UTF-16 code unit: 2603
  Padded_String code(u8"\u2603 x"_sv);
  const Char8* x = strchr(code.c_str(), u8'x');
  Web_Demo_Locator l(&code);
  EXPECT_EQ(l.position(x), 2);
}

TEST(Test_Web_Demo_Location, position_after_wide_multi_byte_character) {
  // U+1f496 has four UTF-8 code units: f0 9f 92 96
  // U+1f496 has two UTF-16 code units: D83D DC96
  Padded_String code(u8"\U0001f496 x"_sv);
  const Char8* x = strchr(code.c_str(), u8'x');
  Web_Demo_Locator l(&code);
  EXPECT_EQ(l.position(x), 3);
}
}
}

// quick-lint-js finds bugs in JavaScript programs.
// Copyright (C) 2020  Matthew "strager" Glazar
//
// This file is part of quick-lint-js.
//
// quick-lint-js is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// quick-lint-js is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with quick-lint-js.  If not, see <https://www.gnu.org/licenses/>.
