#ifndef ONE_DARK_H_
#define ONE_DARK_H_

#include <array>

namespace less {
  struct HSLA {
    double h, s, l, a;
  };

  struct Color {
    std::array<double, 3> rgb;
    double alpha;
    Color(const std::array<double, 3> &, double);
    HSLA toHSL() const;
  };

  double clamp(double);
  HSLA toHSL(const Color &);
  Color hsla(double, double, double, double);
  Color hsla(const Color &, const HSLA &);
  Color hsl(double, double, double);
  double hue(const Color&);
  double saturation(const Color&);
  double lightness(const Color&);
  double red(const Color&);
  double green(const Color&);
  double blue(const Color&);
  double alpha(const Color&);
  Color lighten(const Color&, double);
  Color darken(const Color&, double);
  Color fade(const Color&, double);
}

namespace one_dark {
  // Config
  extern const double syntax_hue; // = 220;
  extern const double syntax_saturation; // = 13_p;
  extern const double syntax_brightness; // = 18_p;

  // Monochrome
  extern const less::Color mono_1; // = hsl(syntax_hue, 14_p, 71_p); // default text
  extern const less::Color mono_2; // = hsl(syntax_hue,  9_p, 55_p);
  extern const less::Color mono_3; // = hsl(syntax_hue, 10_p, 40_p);

  // Colors
  extern const less::Color hue_1; //   = hsl(187, 47_p, 55_p); // <-cyan
  extern const less::Color hue_2; //   = hsl(207, 82_p, 66_p); // <-blue
  extern const less::Color hue_3; //   = hsl(286, 60_p, 67_p); // <-purple
  extern const less::Color hue_4; //   = hsl( 95, 38_p, 62_p); // <-green

  extern const less::Color hue_5; //   = hsl(355, 65_p, 65_p); // <-red 1
  extern const less::Color hue_5_2; // = hsl(  5, 48_p, 51_p); // <-red 2

  extern const less::Color hue_6; //   = hsl( 29, 54_p, 61_p); // <-orange 1
  extern const less::Color hue_6_2; // = hsl( 39, 67_p, 69_p); // <-orange 2

  // Base colors
  extern const less::Color syntax_fg; //     = mono_1;
  extern const less::Color syntax_bg; //     = hsl(syntax_hue, syntax_saturation, syntax_brightness);
  extern const less::Color syntax_gutter; // = darken(syntax_fg, 26_p);
  extern const less::Color syntax_guide; //  = fade(syntax_fg, 15_p);
  extern const less::Color syntax_accent; // = hsl(syntax_hue, 100_p, 66_p);

  // General colors
  extern const less::Color syntax_background_color; //      = syntax_bg;
  extern const less::Color syntax_text_color; //            = syntax_fg;
  extern const less::Color syntax_cursor_color; //          = syntax_accent;
  extern const less::Color syntax_selection_color; //       = lighten(syntax_background_color, 10_p);
  extern const less::Color syntax_selection_flash_color; // = syntax_accent;

  extern const less::Color syntax_invisible_character_color; // = syntax_guide;
}

#endif  // ONE_DARK_H_
