#include "one-dark.h"
#include <algorithm>
#include <cmath>

// the following code was adapted from
// https://github.com/less/less.js/blob/v4.1.2/packages/less/src/less/tree/color.js
// https://github.com/less/less.js/blob/v4.1.2/packages/less/src/less/functions/color.js
// licensed under the Apache License Version 2.0

less::Color::Color(const std::array<double, 3> &rgb, double alpha): rgb(rgb), alpha(alpha) {}

less::HSLA less::Color::toHSL() const {
  const double r = this->rgb[0] / 255.0, g = this->rgb[1] / 255.0, b = this->rgb[2] / 255.0, a = this->alpha;

  const double max = std::max({r, g, b}), min = std::min({r, g, b});
  double h;
  double s;
  const double l = (max + min) / 2.0;
  const double d = max - min;

  if (max == min) {
    h = s = 0.0;
  } else {
    s = l > 0.5 ? d / (2.0 - max - min) : d / (max + min);

    if (max == r) {
      h = (g - b) / d + (g < b ? 6.0 : 0.0);
    }
    else if (max == g) {
      h = (b - r) / d + 2.0;
    }
    else if (max == b) {
      h = (r - g) / d + 4.0;
    }
    h /= 6.0;
  }
  return { h * 360.0, s, l, a };
}

double less::clamp(double val) {
  return std::min(1.0, std::max(0.0, val));
}

less::HSLA less::toHSL(const Color &color) {
  return color.toHSL();
}

less::Color less::hsla(double h, double s, double l, double a) {
  double m1;
  double m2;

  auto hue = [&](double h) {
    h = h < 0.0 ? h + 1.0 : (h > 1.0 ? h - 1.0 : h);
    if (h * 6.0 < 1.0) {
      return m1 + (m2 - m1) * h * 6;
    }
    else if (h * 2.0 < 1.0) {
      return m2;
    }
    else if (h * 3.0 < 2.0) {
      return m1 + (m2 - m1) * (2.0 / 3.0 - h) * 6.0;
    }
    else {
      return m1;
    }
  };

  h = std::fmod(h, 360.0) / 360.0;
  s = clamp(s); l = clamp(l); a = clamp(a);

  m2 = l <= 0.5 ? l * (s + 1.0) : l + s - l * s;
  m1 = l * 2.0 - m2;

  const std::array<double, 3> rgb = {
    hue(h + 1.0 / 3.0) * 255.0,
    hue(h)             * 255.0,
    hue(h - 1.0 / 3.0) * 255.0
  };
  return Color(rgb, a);
}

less::Color less::hsla(const Color &origColor, const HSLA &hsl) {
  return hsla(hsl.h, hsl.s, hsl.l, hsl.a);
}

less::Color less::hsl(double h, double s, double l) {
  const double a = 1.0;
  const Color color = hsla(h, s, l, a);
  return color;
}

double less::hue(const Color& color) {
  return toHSL(color).h;
}

double less::saturation(const Color& color) {
  return toHSL(color).s;
}

double less::lightness(const Color& color) {
  return toHSL(color).l;
}

double less::red(const Color& color) {
  return color.rgb[0];
}

double less::green(const Color& color) {
  return color.rgb[1];
}

double less::blue(const Color& color) {
  return color.rgb[2];
}

double less::alpha(const Color& color) {
  return toHSL(color).a;
}

less::Color less::lighten(const Color &color, double amount) {
  auto hsl = toHSL(color);

  hsl.l += amount;
  hsl.l = clamp(hsl.l);
  return hsla(color, hsl);
}

less::Color less::darken(const Color& color, double amount) {
  auto hsl = toHSL(color);

  hsl.l -= amount;
  hsl.l = clamp(hsl.l);
  return hsla(color, hsl);
}

less::Color less::fade(const Color& color, double amount) {
  auto hsl = toHSL(color);

  hsl.a = amount;
  hsl.a = clamp(hsl.a);
  return hsla(color, hsl);
}

constexpr double operator "" _p(unsigned long long int v) {
  return v / 100.0;
}

// the following code was adapted from
// https://github.com/atom/atom/blob/v1.58.0/packages/one-dark-syntax/styles/colors.less
// https://github.com/atom/atom/blob/v1.58.0/packages/one-dark-syntax/styles/syntax-variables.less
// licensed under the MIT license

using namespace less;

// Config
const double one_dark::syntax_hue = 220;
const double one_dark::syntax_saturation = 13_p;
const double one_dark::syntax_brightness = 18_p;

// Monochrome
const Color one_dark::mono_1 = hsl(syntax_hue, 14_p, 71_p); // default text
const Color one_dark::mono_2 = hsl(syntax_hue,  9_p, 55_p);
const Color one_dark::mono_3 = hsl(syntax_hue, 10_p, 40_p);

// Colors
const Color one_dark::hue_1   = hsl(187, 47_p, 55_p); // <-cyan
const Color one_dark::hue_2   = hsl(207, 82_p, 66_p); // <-blue
const Color one_dark::hue_3   = hsl(286, 60_p, 67_p); // <-purple
const Color one_dark::hue_4   = hsl( 95, 38_p, 62_p); // <-green

const Color one_dark::hue_5   = hsl(355, 65_p, 65_p); // <-red 1
const Color one_dark::hue_5_2 = hsl(  5, 48_p, 51_p); // <-red 2

const Color one_dark::hue_6   = hsl( 29, 54_p, 61_p); // <-orange 1
const Color one_dark::hue_6_2 = hsl( 39, 67_p, 69_p); // <-orange 2

// Base colors
const Color one_dark::syntax_fg     = mono_1;
const Color one_dark::syntax_bg     = hsl(syntax_hue, syntax_saturation, syntax_brightness);
const Color one_dark::syntax_gutter = darken(syntax_fg, 26_p);
const Color one_dark::syntax_guide  = fade(syntax_fg, 15_p);
const Color one_dark::syntax_accent = hsl(syntax_hue, 100_p, 66_p);

// General colors
const Color one_dark::syntax_background_color      = syntax_bg;
const Color one_dark::syntax_text_color            = syntax_fg;
const Color one_dark::syntax_cursor_color          = syntax_accent;
const Color one_dark::syntax_selection_color       = lighten(syntax_background_color, 10_p);
const Color one_dark::syntax_selection_flash_color = syntax_accent;

const Color one_dark::syntax_invisible_character_color = syntax_guide;
