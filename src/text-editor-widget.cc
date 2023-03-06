#include "text-editor-widget.h"
#include "layout-cache.h"
#include <grammar-registry.h>
#include <text-editor.h>
#include <display-marker.h>
#include <decoration-manager.h>
#include <selection.h>
#include <select-next.h>

extern "C" TreeSitterGrammar *atom_language_c();
extern "C" TreeSitterGrammar *atom_language_cpp();
extern "C" TreeSitterGrammar *atom_language_javascript();
extern "C" TreeSitterGrammar *atom_language_python();

#if !GLIB_CHECK_VERSION(2, 73, 2)
#define G_CONNECT_DEFAULT ((GConnectFlags)0)
#endif

#define LINE_HEIGHT_FACTOR 1.5

static void atom_text_editor_widget_dispose(GObject *);
static void atom_text_editor_widget_finalize(GObject *);
static void atom_text_editor_widget_set_property(GObject *, guint, const GValue *, GParamSpec *);
static void atom_text_editor_widget_get_property(GObject *, guint, GValue *, GParamSpec *);
static void atom_text_editor_widget_realize(GtkWidget *);
static void atom_text_editor_widget_unrealize(GtkWidget *);
static void atom_text_editor_widget_size_allocate(GtkWidget *, GtkAllocation *);
static void get_style_property_for_path(GtkWidget *, const std::vector<std::string> &, const gchar *, GValue *);
static PangoLayout *create_layout(AtomTextEditorWidget *, const DisplayLayer::ScreenLine &);
static PangoLayout *create_layout(AtomTextEditorWidget *, double);
static gboolean atom_text_editor_widget_draw(GtkWidget *, cairo_t *);
static gboolean atom_text_editor_widget_key_press_event(GtkWidget *, GdkEventKey *);
static gboolean atom_text_editor_widget_key_release_event(GtkWidget *, GdkEventKey *);
static void atom_text_editor_widget_commit(GtkIMContext *, gchar *, gpointer);
static void atom_text_editor_widget_handle_pressed(GtkGestureMultiPress *, gint, gdouble, gdouble, gpointer);
static void atom_text_editor_widget_handle_released(GtkGestureMultiPress *, gint, gdouble, gdouble, gpointer);
static void atom_text_editor_widget_handle_drag_update(GtkGestureDrag *, gdouble, gdouble, gpointer);
static void update(AtomTextEditorWidget *, bool = true);
static void get_row_and_column(AtomTextEditorWidget *, double, double, int &, int &);
static void atom_text_editor_widget_move_up(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_down(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_left(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_right(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_to_first_character_of_line(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_to_end_of_line(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_to_beginning_of_word(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_to_end_of_word(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_to_previous_subword_boundary(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_to_next_subword_boundary(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_to_top(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_to_bottom(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_all(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_up(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_down(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_left(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_right(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_to_first_character_of_line(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_to_end_of_line(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_to_beginning_of_word(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_to_end_of_word(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_to_previous_subword_boundary(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_to_next_subword_boundary(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_to_top(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_to_bottom(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_line(AtomTextEditorWidget *);
static void atom_text_editor_widget_consolidate_selections(AtomTextEditorWidget *);
static void atom_text_editor_widget_add_selection_above(AtomTextEditorWidget *);
static void atom_text_editor_widget_add_selection_below(AtomTextEditorWidget *);
static void atom_text_editor_widget_select_next(AtomTextEditorWidget *);
static void atom_text_editor_widget_insert_newline(AtomTextEditorWidget *);
static void atom_text_editor_widget_insert_newline_above(AtomTextEditorWidget *);
static void atom_text_editor_widget_insert_newline_below(AtomTextEditorWidget *);
static void atom_text_editor_widget_backspace(AtomTextEditorWidget *);
static void atom_text_editor_widget_delete(AtomTextEditorWidget *);
static void atom_text_editor_widget_delete_to_beginning_of_word(AtomTextEditorWidget *);
static void atom_text_editor_widget_delete_to_end_of_word(AtomTextEditorWidget *);
static void atom_text_editor_widget_delete_to_beginning_of_subword(AtomTextEditorWidget *);
static void atom_text_editor_widget_delete_to_end_of_subword(AtomTextEditorWidget *);
static void atom_text_editor_widget_indent(AtomTextEditorWidget *);
static void atom_text_editor_widget_outdent_selected_rows(AtomTextEditorWidget *);
static void atom_text_editor_widget_delete_line(AtomTextEditorWidget *);
static void atom_text_editor_widget_duplicate_lines(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_line_up(AtomTextEditorWidget *);
static void atom_text_editor_widget_move_line_down(AtomTextEditorWidget *);
static void atom_text_editor_widget_undo(AtomTextEditorWidget *);
static void atom_text_editor_widget_redo(AtomTextEditorWidget *);

class Layout {
  PangoLayout *layout;
public:
  Layout(AtomTextEditorWidget *self, const DisplayLayer::ScreenLine &screen_line) {
    layout = create_layout(self, screen_line);
  }
  Layout(AtomTextEditorWidget *self, double row) {
    layout = create_layout(self, row);
  }
  Layout(const Layout &other) {
    layout = other.layout;
    g_object_ref(layout);
  }
  ~Layout() {
    g_object_unref(layout);
  }
  Layout &operator =(const Layout &other) {
    g_object_unref(layout);
    layout = other.layout;
    g_object_ref(layout);
    return *this;
  }
  void draw(cairo_t *cr, double x, double y, bool align_right = false) const {
    PangoLayoutLine *layout_line = pango_layout_get_line_readonly(layout, 0);
    if (align_right) {
      PangoRectangle rectangle;
      pango_layout_line_get_pixel_extents(layout_line, NULL, &rectangle);
      x -= rectangle.width;
    }
    cairo_move_to(cr, x, y);
    pango_cairo_show_layout_line(cr, layout_line);
  }
  double index_to_x(int index) const {
    const char *text = pango_layout_get_text(layout);
    index = g_utf8_offset_to_pointer(text, index) - text;
    int x_pos;
    pango_layout_line_index_to_x(pango_layout_get_line_readonly(layout, 0), index, false, &x_pos);
    return pango_units_to_double(x_pos);
  }
  int x_to_index(double x) const {
    int index, trailing;
    pango_layout_line_x_to_index(pango_layout_get_line_readonly(layout, 0), pango_units_from_double(x), &index, &trailing);
    const char *text = pango_layout_get_text(layout);
    const char *pointer = text + index;
    for (; trailing > 0; trailing--) {
      pointer = g_utf8_next_char(pointer);
    }
    return g_utf8_pointer_to_offset(text, pointer);
  }
};

class StyleCache {
  struct Key {
    const gchar *property;
    std::vector<std::string> path;
  };
  struct Hash {
    size_t operator ()(const Key &key) const {
      size_t seed = 0;
      hash_combine(seed, key.property);
      hash_combine(seed, key.path);
      return seed;
    }
  };
  struct Equal {
    bool operator ()(const Key &lhs, const Key &rhs) const {
      return lhs.property == rhs.property && lhs.path == rhs.path;
    }
  };
  std::unordered_map<Key, std::pair<GdkRGBA, size_t>, Hash, Equal> color_cache;
  std::unordered_map<Key, std::pair<PangoStyle, size_t>, Hash, Equal> font_style_cache;
  std::unordered_map<Key, std::pair<PangoWeight, size_t>, Hash, Equal> font_weight_cache;
  size_t generation = 0;
public:
  void increment_generation() {
    generation++;
  }
  void collect_garbage() {
    for (auto iterator = color_cache.begin(); iterator != color_cache.end();) {
      if (iterator->second.second != generation) {
        iterator = color_cache.erase(iterator);
      } else {
        ++iterator;
      }
    }
    for (auto iterator = font_style_cache.begin(); iterator != font_style_cache.end();) {
      if (iterator->second.second != generation) {
        iterator = font_style_cache.erase(iterator);
      } else {
        ++iterator;
      }
    }
    for (auto iterator = font_weight_cache.begin(); iterator != font_weight_cache.end();) {
      if (iterator->second.second != generation) {
        iterator = font_weight_cache.erase(iterator);
      } else {
        ++iterator;
      }
    }
  }
  void get_property(GtkWidget *widget, const std::vector<std::string> &path, const gchar *property, GdkRGBA *color) {
    Key key{property, path};
    auto iterator = color_cache.find(key);
    if (iterator != color_cache.end()) {
      iterator->second.second = generation;
      *color = iterator->second.first;
    } else {
      GValue value = G_VALUE_INIT;
      get_style_property_for_path(widget, path, property, &value);
      *color = *(GdkRGBA *)g_value_get_boxed(&value);
      g_value_unset(&value);
      color_cache.insert({key, {*color, generation}});
    }
  }
  void get_property(GtkWidget *widget, const std::vector<std::string> &path, const gchar *property, PangoStyle *style) {
    Key key{property, path};
    auto iterator = font_style_cache.find(key);
    if (iterator != font_style_cache.end()) {
      iterator->second.second = generation;
      *style = iterator->second.first;
    } else {
      GValue value = G_VALUE_INIT;
      get_style_property_for_path(widget, path, property, &value);
      *style = (PangoStyle)g_value_get_enum(&value);
      g_value_unset(&value);
      font_style_cache.insert({key, {*style, generation}});
    }
  }
  void get_property(GtkWidget *widget, const std::vector<std::string> &path, const gchar *property, PangoWeight *weight) {
    Key key{property, path};
    auto iterator = font_weight_cache.find(key);
    if (iterator != font_weight_cache.end()) {
      iterator->second.second = generation;
      *weight = iterator->second.first;
    } else {
      GValue value = G_VALUE_INIT;
      get_style_property_for_path(widget, path, property, &value);
      *weight = (PangoWeight)g_value_get_enum(&value);
      g_value_unset(&value);
      font_weight_cache.insert({key, {*weight, generation}});
    }
  }
};

static int count_digits(int n) {
  int digits = 1;
  while (n >= 10) {
    digits++;
    n /= 10;
  }
  return digits;
}

typedef struct {
  GtkWidget parent_instance;
  TextEditor *text_editor;
  SelectNext *select_next;
  GtkAdjustment *hadjustment;
  GtkAdjustment *vadjustment;
  GtkScrollablePolicy hscroll_policy;
  GtkScrollablePolicy vscroll_policy;
  GdkWindow *text_window;
  GtkIMContext *im_context;
  GtkGesture *multipress_gesture;
  GtkGesture *drag_gesture;
  PangoFontDescription *font_description;
  double ascent;
  double line_height;
  double char_width;
  LayoutCache<AtomTextEditorWidget, Layout> *layout_cache;
  StyleCache *style_cache;
  double gutter_width;
  Range initial_screen_range;
} AtomTextEditorWidgetPrivate;
G_DEFINE_TYPE_WITH_CODE(AtomTextEditorWidget, atom_text_editor_widget, GTK_TYPE_WIDGET,
  G_ADD_PRIVATE(AtomTextEditorWidget)
  G_IMPLEMENT_INTERFACE(GTK_TYPE_SCROLLABLE, NULL)
)

#define GET_PRIVATE(x) (AtomTextEditorWidgetPrivate *)atom_text_editor_widget_get_instance_private(ATOM_TEXT_EDITOR_WIDGET(x))

typedef enum {
  PROP_0,
  PROP_HADJUSTMENT,
  PROP_VADJUSTMENT,
  PROP_HSCROLL_POLICY,
  PROP_VSCROLL_POLICY,
  N_PROPERTIES
} AtomTextEditorWidgetProperty;

AtomTextEditorWidget *atom_text_editor_widget_new(GFile *file) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(g_object_new(ATOM_TYPE_TEXT_EDITOR_WIDGET, NULL));
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  TextBuffer *buffer;
  if (file) {
    gchar *path = g_file_get_path(file);
    buffer = TextBuffer::loadSync(path);
    g_free(path);
  } else {
    buffer = new TextBuffer();
  }
  static GrammarRegistry *grammar_registry = nullptr;
  if (grammar_registry == nullptr) {
    grammar_registry = new GrammarRegistry();
    grammar_registry->addGrammar(atom_language_c());
    grammar_registry->addGrammar(atom_language_cpp());
    grammar_registry->addGrammar(atom_language_javascript());
    grammar_registry->addGrammar(atom_language_python());
  }
  grammar_registry->autoAssignLanguageMode(buffer);
  priv->text_editor = new TextEditor(buffer);
  priv->select_next = new SelectNext(priv->text_editor);
  const double padding = round(priv->char_width);
  priv->gutter_width = padding * 4 + round(count_digits(priv->text_editor->getScreenLineCount()) * priv->char_width);
  return self;
}

#define ADD_SIGNAL(name, vfunc) g_signal_new(name, ATOM_TYPE_TEXT_EDITOR_WIDGET, (GSignalFlags)(G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(AtomTextEditorWidgetClass, vfunc), NULL, NULL, NULL, G_TYPE_NONE, 0)

static void atom_text_editor_widget_class_init(AtomTextEditorWidgetClass *klass) {
  G_OBJECT_CLASS(klass)->dispose = atom_text_editor_widget_dispose;
  G_OBJECT_CLASS(klass)->finalize = atom_text_editor_widget_finalize;
  G_OBJECT_CLASS(klass)->set_property = atom_text_editor_widget_set_property;
  G_OBJECT_CLASS(klass)->get_property = atom_text_editor_widget_get_property;
  GTK_WIDGET_CLASS(klass)->realize = atom_text_editor_widget_realize;
  GTK_WIDGET_CLASS(klass)->unrealize = atom_text_editor_widget_unrealize;
  GTK_WIDGET_CLASS(klass)->size_allocate = atom_text_editor_widget_size_allocate;
  GTK_WIDGET_CLASS(klass)->draw = atom_text_editor_widget_draw;
  GTK_WIDGET_CLASS(klass)->key_press_event = atom_text_editor_widget_key_press_event;
  GTK_WIDGET_CLASS(klass)->key_release_event = atom_text_editor_widget_key_release_event;
  klass->move_up = atom_text_editor_widget_move_up;
  klass->move_down = atom_text_editor_widget_move_down;
  klass->move_left = atom_text_editor_widget_move_left;
  klass->move_right = atom_text_editor_widget_move_right;
  klass->move_to_first_character_of_line = atom_text_editor_widget_move_to_first_character_of_line;
  klass->move_to_end_of_line = atom_text_editor_widget_move_to_end_of_line;
  klass->move_to_beginning_of_word = atom_text_editor_widget_move_to_beginning_of_word;
  klass->move_to_end_of_word = atom_text_editor_widget_move_to_end_of_word;
  klass->move_to_previous_subword_boundary = atom_text_editor_widget_move_to_previous_subword_boundary;
  klass->move_to_next_subword_boundary = atom_text_editor_widget_move_to_next_subword_boundary;
  klass->move_to_top = atom_text_editor_widget_move_to_top;
  klass->move_to_bottom = atom_text_editor_widget_move_to_bottom;
  klass->select_all = atom_text_editor_widget_select_all;
  klass->select_up = atom_text_editor_widget_select_up;
  klass->select_down = atom_text_editor_widget_select_down;
  klass->select_left = atom_text_editor_widget_select_left;
  klass->select_right = atom_text_editor_widget_select_right;
  klass->select_to_first_character_of_line = atom_text_editor_widget_select_to_first_character_of_line;
  klass->select_to_end_of_line = atom_text_editor_widget_select_to_end_of_line;
  klass->select_to_beginning_of_word = atom_text_editor_widget_select_to_beginning_of_word;
  klass->select_to_end_of_word = atom_text_editor_widget_select_to_end_of_word;
  klass->select_to_previous_subword_boundary = atom_text_editor_widget_select_to_previous_subword_boundary;
  klass->select_to_next_subword_boundary = atom_text_editor_widget_select_to_next_subword_boundary;
  klass->select_to_top = atom_text_editor_widget_select_to_top;
  klass->select_to_bottom = atom_text_editor_widget_select_to_bottom;
  klass->select_line = atom_text_editor_widget_select_line;
  klass->consolidate_selections = atom_text_editor_widget_consolidate_selections;
  klass->add_selection_above = atom_text_editor_widget_add_selection_above;
  klass->add_selection_below = atom_text_editor_widget_add_selection_below;
  klass->select_next = atom_text_editor_widget_select_next;
  klass->insert_newline = atom_text_editor_widget_insert_newline;
  klass->insert_newline_above = atom_text_editor_widget_insert_newline_above;
  klass->insert_newline_below = atom_text_editor_widget_insert_newline_below;
  klass->backspace = atom_text_editor_widget_backspace;
  klass->delete_ = atom_text_editor_widget_delete;
  klass->delete_to_beginning_of_word = atom_text_editor_widget_delete_to_beginning_of_word;
  klass->delete_to_end_of_word = atom_text_editor_widget_delete_to_end_of_word;
  klass->delete_to_beginning_of_subword = atom_text_editor_widget_delete_to_beginning_of_subword;
  klass->delete_to_end_of_subword = atom_text_editor_widget_delete_to_end_of_subword;
  klass->indent = atom_text_editor_widget_indent;
  klass->outdent_selected_rows = atom_text_editor_widget_outdent_selected_rows;
  klass->delete_line = atom_text_editor_widget_delete_line;
  klass->duplicate_lines = atom_text_editor_widget_duplicate_lines;
  klass->move_line_up = atom_text_editor_widget_move_line_up;
  klass->move_line_down = atom_text_editor_widget_move_line_down;
  klass->undo = atom_text_editor_widget_undo;
  klass->redo = atom_text_editor_widget_redo;
  ADD_SIGNAL("move-up", move_up);
  ADD_SIGNAL("move-down", move_down);
  ADD_SIGNAL("move-left", move_left);
  ADD_SIGNAL("move-right", move_right);
  ADD_SIGNAL("move-to-first-character-of-line", move_to_first_character_of_line);
  ADD_SIGNAL("move-to-end-of-line", move_to_end_of_line);
  ADD_SIGNAL("move-to-beginning-of-word", move_to_beginning_of_word);
  ADD_SIGNAL("move-to-end-of-word", move_to_end_of_word);
  ADD_SIGNAL("move-to-previous-subword-boundary", move_to_previous_subword_boundary);
  ADD_SIGNAL("move-to-next-subword-boundary", move_to_next_subword_boundary);
  ADD_SIGNAL("move-to-top", move_to_top);
  ADD_SIGNAL("move-to-bottom", move_to_bottom);
  ADD_SIGNAL("select-all", select_all);
  ADD_SIGNAL("select-up", select_up);
  ADD_SIGNAL("select-down", select_down);
  ADD_SIGNAL("select-left", select_left);
  ADD_SIGNAL("select-right", select_right);
  ADD_SIGNAL("select-to-first-character-of-line", select_to_first_character_of_line);
  ADD_SIGNAL("select-to-end-of-line", select_to_end_of_line);
  ADD_SIGNAL("select-to-beginning-of-word", select_to_beginning_of_word);
  ADD_SIGNAL("select-to-end-of-word", select_to_end_of_word);
  ADD_SIGNAL("select-to-previous-subword-boundary", select_to_previous_subword_boundary);
  ADD_SIGNAL("select-to-next-subword-boundary", select_to_next_subword_boundary);
  ADD_SIGNAL("select-to-top", select_to_top);
  ADD_SIGNAL("select-to-bottom", select_to_bottom);
  ADD_SIGNAL("select-line", select_line);
  ADD_SIGNAL("consolidate-selections", consolidate_selections);
  ADD_SIGNAL("add-selection-above", add_selection_above);
  ADD_SIGNAL("add-selection-below", add_selection_below);
  ADD_SIGNAL("select-next", select_next);
  ADD_SIGNAL("insert-newline", insert_newline);
  ADD_SIGNAL("insert-newline-above", insert_newline_above);
  ADD_SIGNAL("insert-newline-below", insert_newline_below);
  ADD_SIGNAL("backspace", backspace);
  ADD_SIGNAL("delete", delete_);
  ADD_SIGNAL("delete-to-beginning-of-word", delete_to_beginning_of_word);
  ADD_SIGNAL("delete-to-end-of-word", delete_to_end_of_word);
  ADD_SIGNAL("delete-to-beginning-of-subword", delete_to_beginning_of_subword);
  ADD_SIGNAL("delete-to-end-of-subword", delete_to_end_of_subword);
  ADD_SIGNAL("indent", indent);
  ADD_SIGNAL("outdent-selected-rows", outdent_selected_rows);
  ADD_SIGNAL("delete-line", delete_line);
  ADD_SIGNAL("duplicate-lines", duplicate_lines);
  ADD_SIGNAL("move-line-up", move_line_up);
  ADD_SIGNAL("move-line-down", move_line_down);
  ADD_SIGNAL("undo", undo);
  ADD_SIGNAL("redo", redo);
  g_object_class_override_property(G_OBJECT_CLASS(klass), PROP_HADJUSTMENT, "hadjustment");
  g_object_class_override_property(G_OBJECT_CLASS(klass), PROP_VADJUSTMENT, "vadjustment");
  g_object_class_override_property(G_OBJECT_CLASS(klass), PROP_HSCROLL_POLICY, "hscroll-policy");
  g_object_class_override_property(G_OBJECT_CLASS(klass), PROP_VSCROLL_POLICY, "vscroll-policy");
  gtk_widget_class_set_css_name(GTK_WIDGET_CLASS(klass), "atom-text-editor");
}

static void atom_text_editor_widget_init(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->hadjustment = NULL;
  priv->vadjustment = NULL;
  priv->im_context = gtk_im_multicontext_new();
  g_signal_connect_object(priv->im_context, "commit", G_CALLBACK(atom_text_editor_widget_commit), self, G_CONNECT_DEFAULT);
  priv->multipress_gesture = gtk_gesture_multi_press_new(GTK_WIDGET(self));
  g_signal_connect_object(priv->multipress_gesture, "pressed", G_CALLBACK(atom_text_editor_widget_handle_pressed), self, G_CONNECT_DEFAULT);
  g_signal_connect_object(priv->multipress_gesture, "released", G_CALLBACK(atom_text_editor_widget_handle_released), self, G_CONNECT_DEFAULT);
  priv->drag_gesture = gtk_gesture_drag_new(GTK_WIDGET(self));
  g_signal_connect_object(priv->drag_gesture, "drag_update", G_CALLBACK(atom_text_editor_widget_handle_drag_update), self, G_CONNECT_DEFAULT);
  GSettings *settings = g_settings_new("org.gnome.desktop.interface");
  gchar *monospace_font_name = g_settings_get_string(settings, "monospace-font-name");
  g_object_unref(settings);
  priv->font_description = pango_font_description_from_string(monospace_font_name);
  g_free(monospace_font_name);
  PangoFontMetrics *metrics = pango_context_get_metrics(gtk_widget_get_pango_context(GTK_WIDGET(self)), priv->font_description, NULL);
  double font_size = pango_units_to_double(pango_font_description_get_size(priv->font_description));
  if (!pango_font_description_get_size_is_absolute(priv->font_description)) {
    font_size = font_size / 72.0 * 96.0;
  }
  const double ascent = pango_units_to_double(pango_font_metrics_get_ascent(metrics));
  const double descent = pango_units_to_double(pango_font_metrics_get_descent(metrics));
  priv->line_height = round(font_size * LINE_HEIGHT_FACTOR);
  priv->ascent = round(ascent + (priv->line_height - (ascent + descent)) / 2.0);
  priv->char_width = pango_units_to_double(pango_font_metrics_get_approximate_char_width(metrics));
  pango_font_metrics_unref(metrics);
  priv->layout_cache = new LayoutCache<AtomTextEditorWidget, Layout>();
  priv->style_cache = new StyleCache();
  gtk_widget_set_can_focus(GTK_WIDGET(self), TRUE);
  gtk_widget_add_events(GTK_WIDGET(self), GDK_SCROLL_MASK | GDK_SMOOTH_SCROLL_MASK);
}

static void atom_text_editor_widget_dispose(GObject *object) {
  G_OBJECT_CLASS(atom_text_editor_widget_parent_class)->dispose(object);
}

static void atom_text_editor_widget_finalize(GObject *object) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(object);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  delete priv->style_cache;
  delete priv->layout_cache;
  pango_font_description_free(priv->font_description);
  g_object_unref(priv->drag_gesture);
  g_object_unref(priv->multipress_gesture);
  g_object_unref(priv->im_context);
  delete priv->select_next;
  delete priv->text_editor;
  G_OBJECT_CLASS(atom_text_editor_widget_parent_class)->finalize(object);
}

static void atom_text_editor_widget_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(object);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  switch (property_id) {
  case PROP_HADJUSTMENT:
    priv->hadjustment = GTK_ADJUSTMENT(g_value_get_object(value));
    break;
  case PROP_VADJUSTMENT:
    {
      GtkAdjustment *vadjustment = GTK_ADJUSTMENT(g_value_get_object(value));
      if (vadjustment != priv->vadjustment) {
        priv->vadjustment = vadjustment;
        update(self, false);
      }
    }
    break;
  case PROP_HSCROLL_POLICY:
    priv->hscroll_policy = (GtkScrollablePolicy)g_value_get_enum(value);
    break;
  case PROP_VSCROLL_POLICY:
    priv->vscroll_policy = (GtkScrollablePolicy)g_value_get_enum(value);
    break;
  default:
    break;
  }
}

static void atom_text_editor_widget_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(object);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  switch (property_id) {
  case PROP_HADJUSTMENT:
    g_value_set_object(value, priv->hadjustment);
    break;
  case PROP_VADJUSTMENT:
    g_value_set_object(value, priv->vadjustment);
    break;
  case PROP_HSCROLL_POLICY:
    g_value_set_enum(value, priv->hscroll_policy);
    break;
  case PROP_VSCROLL_POLICY:
    g_value_set_enum(value, priv->vscroll_policy);
    break;
  default:
    break;
  }
}

static void atom_text_editor_widget_realize(GtkWidget *widget) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(widget);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  gtk_widget_set_realized(widget, TRUE);
  GtkAllocation allocation;
  gtk_widget_get_allocation(widget, &allocation);
  GdkWindowAttr attributes;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = allocation.x;
  attributes.y = allocation.y;
  attributes.width = allocation.width;
  attributes.height = allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual(widget);
  attributes.event_mask = gtk_widget_get_events(widget) | GDK_EXPOSURE_MASK;
  GdkWindow *window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL);
  gtk_widget_set_window(widget, window);
  gtk_widget_register_window(widget, window);
  attributes.x = priv->gutter_width;
  attributes.width = allocation.width - priv->gutter_width;
  priv->text_window = gdk_window_new(window, &attributes, GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL);
  gtk_widget_register_window(widget, priv->text_window);
  gdk_window_show(priv->text_window);
  GdkCursor *cursor = gdk_cursor_new_from_name(gdk_window_get_display(priv->text_window), "text");
  gdk_window_set_cursor(priv->text_window, cursor);
  g_object_unref(cursor);
  //GTK_WIDGET_CLASS(atom_text_editor_widget_parent_class)->realize(widget);
}

static void atom_text_editor_widget_unrealize(GtkWidget *widget) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(widget);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  gtk_widget_unregister_window(widget, priv->text_window);
  gdk_window_destroy(priv->text_window);
  GTK_WIDGET_CLASS(atom_text_editor_widget_parent_class)->unrealize(widget);
}

static void atom_text_editor_widget_size_allocate(GtkWidget *widget, GtkAllocation *allocation) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(widget);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  GTK_WIDGET_CLASS(atom_text_editor_widget_parent_class)->size_allocate(widget, allocation);
  if (gtk_widget_get_realized(widget)) {
    gdk_window_move_resize(priv->text_window, allocation->x + priv->gutter_width, allocation->y, allocation->width - priv->gutter_width, allocation->height);
  }
  update(self, false);
}

gboolean atom_text_editor_widget_save(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  if (!priv->text_editor->getPath()) {
    return FALSE;
  }
  priv->text_editor->save();
  return TRUE;
}

void atom_text_editor_widget_save_as(AtomTextEditorWidget *self, GFile *file) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  gchar *path = g_file_get_path(file);
  priv->text_editor->saveAs(path);
  g_free(path);
}

static void get_style_property_for_path(GtkWidget *widget, const std::vector<std::string> &path, const gchar *property, GValue *value) {
  GtkStyleContext *style_context = gtk_style_context_new();
  GtkWidgetPath *widget_path = gtk_widget_path_new();
  gint iter0 = gtk_widget_path_append_type(widget_path, ATOM_TYPE_TEXT_EDITOR_WIDGET);
  gtk_widget_path_iter_set_object_name(widget_path, iter0, "atom-text-editor");
  gtk_style_context_set_path(style_context, widget_path);
  for (const std::string &classes : path) {
    gint iter = gtk_widget_path_append_type(widget_path, G_TYPE_NONE);
    gtk_widget_path_iter_set_object_name(widget_path, iter, "div");
    size_t pos = 0;
    size_t match = classes.find(' ', pos);
    while (match != std::string::npos) {
      gtk_widget_path_iter_add_class(widget_path, iter, classes.substr(pos, match - pos).c_str());
      pos = match + 1;
      match = classes.find(' ', pos);
    }
    gtk_widget_path_iter_add_class(widget_path, iter, classes.substr(pos).c_str());
    GtkStyleContext *new_style_context = gtk_style_context_new();
    gtk_style_context_set_path(new_style_context, widget_path);
    gtk_style_context_set_parent(new_style_context, style_context);
    g_object_unref(style_context);
    style_context = new_style_context;
  }
  gtk_style_context_get_property(style_context, property, GTK_STATE_FLAG_NORMAL, value);
  gtk_widget_path_free(widget_path);
  g_object_unref(style_context);
}

static void emit_attributes(StyleCache *style_cache, GtkWidget *widget, gchar *utf8, PangoAttrList *attrs, int32_t index, int32_t &last_index, const std::vector<std::string> &classes) {
  if (index == last_index) return;

  if (classes.size() > 1) {
    PangoStyle font_style;
    style_cache->get_property(widget, classes, "font-style", &font_style);
    PangoAttribute *attr = pango_attr_style_new(font_style);
    attr->start_index = g_utf8_offset_to_pointer(utf8, last_index) - utf8;
    attr->end_index = g_utf8_offset_to_pointer(utf8, index) - utf8;
    pango_attr_list_insert(attrs, attr);

    PangoWeight font_weight;
    style_cache->get_property(widget, classes, "font-weight", &font_weight);
    attr = pango_attr_weight_new(font_weight);
    attr->start_index = g_utf8_offset_to_pointer(utf8, last_index) - utf8;
    attr->end_index = g_utf8_offset_to_pointer(utf8, index) - utf8;
    pango_attr_list_insert(attrs, attr);

    GdkRGBA text_color;
    style_cache->get_property(widget, classes, "color", &text_color);
    attr = pango_attr_foreground_new(text_color.red * G_MAXUINT16, text_color.green * G_MAXUINT16, text_color.blue * G_MAXUINT16);
    attr->start_index = g_utf8_offset_to_pointer(utf8, last_index) - utf8;
    attr->end_index = g_utf8_offset_to_pointer(utf8, index) - utf8;
    pango_attr_list_insert(attrs, attr);
    attr = pango_attr_foreground_alpha_new(text_color.alpha * G_MAXUINT16);
    attr->start_index = g_utf8_offset_to_pointer(utf8, last_index) - utf8;
    attr->end_index = g_utf8_offset_to_pointer(utf8, index) - utf8;
    pango_attr_list_insert(attrs, attr);
  }

  last_index = index;
}

static PangoLayout *create_layout(AtomTextEditorWidget *self, const DisplayLayer::ScreenLine &screen_line) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  PangoLayout *layout = pango_layout_new(gtk_widget_get_pango_context(GTK_WIDGET(self)));
  pango_layout_set_font_description(layout, priv->font_description);
  DisplayLayer *display_layer = priv->text_editor->displayLayer;
  const std::u16string &text = screen_line.lineText;
  gchar *utf8 = g_utf16_to_utf8((const gunichar2 *)text.c_str(), text.size(), NULL, NULL, NULL);
  pango_layout_set_text(layout, utf8, -1);
  PangoAttrList *attrs = pango_attr_list_new();
  int32_t index = 0;
  int32_t last_index = 0;
  std::vector<std::string> classes;
  for (int32_t tag : screen_line.tags) {
    if (display_layer->isOpenTag(tag)) {
      emit_attributes(priv->style_cache, GTK_WIDGET(self), utf8, attrs, index, last_index, classes);
      classes.push_back(display_layer->classNameForTag(tag));
    } else if (display_layer->isCloseTag(tag)) {
      emit_attributes(priv->style_cache, GTK_WIDGET(self), utf8, attrs, index, last_index, classes);
      classes.pop_back();
    } else {
      index += tag;
    }
  }
  pango_layout_set_attributes(layout, attrs);
  pango_attr_list_unref(attrs);
  g_free(utf8);
  return layout;
}

static PangoLayout *create_layout(AtomTextEditorWidget *self, double row) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  PangoLayout *layout = pango_layout_new(gtk_widget_get_pango_context(GTK_WIDGET(self)));
  pango_layout_set_font_description(layout, priv->font_description);
  if (row == 0) {
    pango_layout_set_text(layout, u8"\u2022", -1);
  } else {
    gchar *text = g_strdup_printf("%d", (gint)row);
    pango_layout_set_text(layout, text, -1);
    g_free(text);
  }
  return layout;
}

static Range constrain_range_to_rows(Range range, double start_row, double end_row) {
  if (range.end.row < start_row || range.start.row >= end_row) return Range();
  if (range.start.row < start_row || range.end.row >= end_row) {
    if (range.start.row < start_row) {
      range.start.row = start_row;
      range.start.column = 0;
    }
    if (range.end.row >= end_row) {
      range.end.row = end_row;
      range.end.column = 0;
    }
  }
  return range;
}

static void add_class(std::string &classes, const char *class_) {
  if (!classes.empty()) {
    classes += ' ';
  }
  classes += class_;
}

static void parse_decoration(
  double start_row,
  double end_row,
  const std::pair<DisplayMarker *, std::vector<Decoration::Properties>> &decoration,
  std::vector<std::string> &line_classes,
  std::vector<std::string> &gutter_classes,
  std::vector<std::pair<Range, const char *>> &highlights,
  std::vector<std::pair<int32_t, int32_t>> &cursors
) {
  DisplayMarker *marker = decoration.first;
  for (const auto &properties : decoration.second) {
    switch(properties.type) {
    case Decoration::Type::line:
    case Decoration::Type::line_number:
      {
        const Range range = marker->getScreenRange();
        const bool reversed = marker->isReversed();

        bool omit_last_row = false;
        if (range.isEmpty()) {
          if (properties.onlyNonEmpty) continue;
        } else {
          if (properties.onlyEmpty) continue;
          if (properties.omitEmptyLastRow) {
            omit_last_row = range.end.column == 0;
          }
        }

        double range_start_row = range.start.row;
        double range_end_row = range.end.row;
        if (properties.onlyHead) {
          if (reversed) {
            range_end_row = range_start_row;
          } else {
            range_start_row = range_end_row;
          }
        }
        range_start_row = std::max(range_start_row, start_row);
        range_end_row = std::min(range_end_row, end_row - 1);
        for (double row = range_start_row; row <= range_end_row; row++) {
          if (omit_last_row && row == range.end.row) break;
          if (properties.type == Decoration::Type::line) {
            add_class(line_classes[row - start_row], properties.class_);
          } else {
            add_class(gutter_classes[row - start_row], properties.class_);
          }
        }
        break;
      }
    case Decoration::Type::highlight:
      {
        const Range range = constrain_range_to_rows(marker->getScreenRange(), start_row, end_row);
        if (range.isEmpty()) continue;
        highlights.push_back({range, properties.class_});
        break;
      }
    case Decoration::Type::cursor:
      {
        const Point position = marker->getHeadScreenPosition();
        if (position.row < start_row || position.row >= end_row) continue;
        cursors.push_back({position.row, position.column});
        break;
      }
    default:
      break;
    }
  }
}

static void draw_gutter(
  GtkWidget *widget,
  cairo_t *cr,
  double padding,
  double allocated_width,
  double start_row,
  double end_row,
  const std::vector<std::string> &gutter_classes
) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(widget);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  cairo_push_group(cr);
  for (double row = start_row; row < end_row; row++) {
    double y = row * priv->line_height;
    if (!gutter_classes[row - start_row].empty()) {
      GdkRGBA background_color;
      priv->style_cache->get_property(widget, {"gutter", "line-number " + gutter_classes[row - start_row]}, "background-color", &background_color);
      gdk_cairo_set_source_rgba(cr, &background_color);
      cairo_rectangle(cr, 0, y, allocated_width, priv->line_height);
      cairo_fill(cr);
    }
    double buffer_row = priv->text_editor->bufferRowForScreenRow(row);
    if (row > 0 && buffer_row == priv->text_editor->bufferRowForScreenRow(row - 1)) {
      buffer_row = 0;
    } else {
      buffer_row = buffer_row + 1;
    }
    Layout layout = priv->layout_cache->get_line_number(self, buffer_row);
    GdkRGBA text_color;
    priv->style_cache->get_property(widget, {"gutter", "line-number " + gutter_classes[row - start_row]}, "color", &text_color);
    gdk_cairo_set_source_rgba(cr, &text_color);
    layout.draw(cr, allocated_width - padding * 2, y + priv->ascent, true);
  }
  cairo_pop_group_to_source(cr);
  cairo_paint_with_alpha(cr, 0.6);
}

static void draw_lines(
  GtkWidget *widget,
  cairo_t *cr,
  double allocated_width,
  double start_row,
  double end_row,
  const std::vector<std::string> &line_classes,
  const std::vector<std::pair<Range, const char *>> &highlights,
  const std::vector<std::pair<int32_t, int32_t>> &cursors,
  const std::vector<Layout> &layouts
) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(widget);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  for (const auto &highlight : highlights) {
    const Range range = highlight.first;
    const char *class_ = highlight.second;
    GdkRGBA highlight_color;
    std::vector<std::string> path;
    path.push_back(std::string("highlight ") + class_);
    path.push_back(std::string("region ") + class_);
    priv->style_cache->get_property(widget, path, "background-color", &highlight_color);
    gdk_cairo_set_source_rgba(cr, &highlight_color);
    double y_start = range.start.row * priv->line_height;
    double y_end = y_start + priv->line_height;
    double x_start = layouts[range.start.row - start_row].index_to_x(range.start.column);
    if (range.start.row == range.end.row) {
      double x_end = layouts[range.start.row - start_row].index_to_x(range.end.column);
      cairo_rectangle(cr, x_start, y_start, x_end - x_start, y_end - y_start);
    } else {
      double x_end = allocated_width;
      cairo_rectangle(cr, x_start, y_start, x_end - x_start, y_end - y_start);
      y_start = y_end;
      x_start = 0;
      if (range.end.row > range.start.row + 1) {
        y_end = range.end.row * priv->line_height;
        cairo_rectangle(cr, x_start, y_start, x_end - x_start, y_end - y_start);
        y_start = y_end;
      }
      if (range.end.column > 0) {
        y_end = y_start + priv->line_height;
        x_end = layouts[range.end.row - start_row].index_to_x(range.end.column);
        cairo_rectangle(cr, x_start, y_start, x_end - x_start, y_end - y_start);
      }
    }
    cairo_fill(cr);
  }
  GdkRGBA text_color;
  priv->style_cache->get_property(widget, {}, "color", &text_color);
  for (double row = start_row; row < end_row; row++) {
    double y = row * priv->line_height;
    if (!line_classes[row - start_row].empty()) {
      GdkRGBA background_color;
      priv->style_cache->get_property(widget, {"line " + line_classes[row - start_row]}, "background-color", &background_color);
      gdk_cairo_set_source_rgba(cr, &background_color);
      cairo_rectangle(cr, 0, y, allocated_width, priv->line_height);
      cairo_fill(cr);
    }
    gdk_cairo_set_source_rgba(cr, &text_color);
    layouts[row - start_row].draw(cr, 0, y + priv->ascent);
  }
  GdkRGBA cursor_color;
  priv->style_cache->get_property(widget, {"cursor"}, "border-left-color", &cursor_color);
  gdk_cairo_set_source_rgba(cr, &cursor_color);
  for (const auto &cursor : cursors) {
    double y = cursor.first * priv->line_height;
    double x = layouts[cursor.first - start_row].index_to_x(cursor.second);
    cairo_rectangle(cr, x, y, 2, priv->line_height);
    cairo_fill(cr);
  }
}

static gboolean atom_text_editor_widget_draw(GtkWidget *widget, cairo_t *cr) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(widget);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);

  priv->layout_cache->increment_generation();
  priv->style_cache->increment_generation();

  const double allocated_width = gtk_widget_get_allocated_width(widget);
  const double allocated_height = gtk_widget_get_allocated_height(widget);
  const double vadjustment = gtk_adjustment_get_value(priv->vadjustment);

  const double start_row = std::min(std::floor(vadjustment / priv->line_height), priv->text_editor->getScreenLineCount());
  const double end_row = std::min(std::ceil((allocated_height + vadjustment) / priv->line_height), priv->text_editor->getScreenLineCount());
  auto screen_lines = priv->text_editor->displayLayer->getScreenLines(start_row, end_row);
  auto decorations = priv->text_editor->decorationManager->decorationPropertiesByMarkerForScreenRowRange(start_row, end_row);

  std::vector<std::string> line_classes(screen_lines.size());
  std::vector<std::string> gutter_classes(screen_lines.size());
  std::vector<std::pair<Range, const char *>> highlights;
  std::vector<std::pair<int32_t, int32_t>> cursors;
  for (const auto &decoration : decorations) {
    parse_decoration(start_row, end_row, decoration, line_classes, gutter_classes, highlights, cursors);
  }

  std::vector<Layout> layouts = priv->layout_cache->get_layouts(self, screen_lines);

  const double padding = round(priv->char_width);
  const double gutter_width = padding * 4 + round(count_digits(priv->text_editor->getScreenLineCount()) * priv->char_width);

  GdkRGBA background_color;
  priv->style_cache->get_property(widget, {}, "background-color", &background_color);
  gdk_cairo_set_source_rgba(cr, &background_color);
  cairo_paint(cr);

  cairo_save(cr);
  cairo_translate(cr, 0, -vadjustment);
  draw_gutter(widget, cr, padding, gutter_width, start_row, end_row, gutter_classes);
  cairo_restore(cr);
  cairo_save(cr);
  cairo_translate(cr, gutter_width, -vadjustment);
  draw_lines(widget, cr, allocated_width - gutter_width, start_row, end_row, line_classes, highlights, cursors, layouts);
  cairo_restore(cr);

  priv->layout_cache->collect_garbage();
  priv->style_cache->collect_garbage();

  return GDK_EVENT_STOP;
}

static gboolean atom_text_editor_widget_key_press_event(GtkWidget *widget, GdkEventKey *event) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(widget);
  if (GTK_WIDGET_CLASS(atom_text_editor_widget_parent_class)->key_press_event(widget, event) || gtk_im_context_filter_keypress(priv->im_context, event)) {
    return GDK_EVENT_STOP;
  }
  return GDK_EVENT_PROPAGATE;
}

static gboolean atom_text_editor_widget_key_release_event(GtkWidget *widget, GdkEventKey *event) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(widget);
  if (GTK_WIDGET_CLASS(atom_text_editor_widget_parent_class)->key_release_event(widget, event) || gtk_im_context_filter_keypress(priv->im_context, event)) {
    return GDK_EVENT_STOP;
  }
  return GDK_EVENT_PROPAGATE;
}

static void atom_text_editor_widget_commit(GtkIMContext *im_context, gchar *text, gpointer user_data) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(user_data);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  gunichar2 *utf16 = g_utf8_to_utf16(text, -1, NULL, NULL, NULL);
  priv->text_editor->insertText((const char16_t *)utf16, true);
  g_free(utf16);
  update(self);
}

static void atom_text_editor_widget_handle_pressed(GtkGestureMultiPress *multipress_gesture, gint n_press, gdouble x, gdouble y, gpointer user_data) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(user_data);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  const double vadjustment = gtk_adjustment_get_value(priv->vadjustment);
  gtk_widget_grab_focus(GTK_WIDGET(self));
  GdkEventSequence *sequence = gtk_gesture_single_get_current_sequence(GTK_GESTURE_SINGLE(multipress_gesture));
  const GdkEvent *event = gtk_gesture_get_last_event(GTK_GESTURE(multipress_gesture), sequence);
  GdkModifierType state;
  gdk_event_get_state(event, &state);
  const bool modify_selection = state & gtk_widget_get_modifier_mask(GTK_WIDGET(self), GDK_MODIFIER_INTENT_MODIFY_SELECTION);
  const bool extend_selection = state & gtk_widget_get_modifier_mask(GTK_WIDGET(self), GDK_MODIFIER_INTENT_EXTEND_SELECTION);
  if (x < priv->gutter_width) {
    const double row = MAX((y + vadjustment) / priv->line_height, 0.0);
    const double start_buffer_row = priv->text_editor->bufferPositionForScreenPosition({row, 0}).row;
    const double end_buffer_row = priv->text_editor->bufferPositionForScreenPosition({row, INFINITY}).row;
    const Range clicked_line_buffer_range(Point(start_buffer_row, 0), Point(end_buffer_row + 1, 0));
    Range initial_buffer_range;
    if (extend_selection) {
      Selection *last_selection = priv->text_editor->getLastSelection();
      initial_buffer_range = last_selection->getBufferRange();
      const bool reversed = row < last_selection->getScreenRange().start.row;
      last_selection->setBufferRange(initial_buffer_range.union_(clicked_line_buffer_range), reversed);
    } else {
      initial_buffer_range = clicked_line_buffer_range;
      if (modify_selection) {
        priv->text_editor->addSelectionForBufferRange(clicked_line_buffer_range);
      } else {
        priv->text_editor->setSelectedBufferRange(clicked_line_buffer_range);
      }
    }
    priv->initial_screen_range = priv->text_editor->screenRangeForBufferRange(initial_buffer_range);
  } else {
    int row, column;
    get_row_and_column(self, x, y, row, column);
    switch (n_press) {
    case 1:
      if (modify_selection) {
        Selection *selection = priv->text_editor->getSelectionAtScreenPosition(Point(row, column));
        if (selection) {
          if (priv->text_editor->hasMultipleCursors()) selection->destroy();
        } else {
          priv->text_editor->addCursorAtScreenPosition(Point(row, column));
        }
      } else {
        if (extend_selection) {
          priv->text_editor->selectToScreenPosition(Point(row, column));
        } else {
          priv->text_editor->setCursorScreenPosition(Point(row, column));
        }
      }
      break;
    case 2:
      if (modify_selection) {
        priv->text_editor->addCursorAtScreenPosition(Point(row, column));
      }
      priv->text_editor->getLastSelection()->selectWord();
      break;
    case 3:
      if (modify_selection) {
        priv->text_editor->addCursorAtScreenPosition(Point(row, column));
      }
      priv->text_editor->getLastSelection()->selectLine();
      break;
    }
  }
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_handle_released(GtkGestureMultiPress *multipress_gesture, gint, gdouble, gdouble, gpointer user_data) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(user_data);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->mergeIntersectingSelections();
  priv->text_editor->finalizeSelections();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_handle_drag_update(GtkGestureDrag *drag_gesture, gdouble offset_x, gdouble offset_y, gpointer user_data) {
  AtomTextEditorWidget *self = ATOM_TEXT_EDITOR_WIDGET(user_data);
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  const double vadjustment = gtk_adjustment_get_value(priv->vadjustment);
  double start_x, start_y;
  gtk_gesture_drag_get_start_point(drag_gesture, &start_x, &start_y);
  if (start_x < priv->gutter_width) {
    const double row = MAX((start_y + offset_y + vadjustment) / priv->line_height, 0.0);
    const Range dragged_line_screen_range(Point(row, 0), Point(row + 1, 0));
    const bool reversed = row < priv->initial_screen_range.start.row;
    priv->text_editor->getLastSelection()->setScreenRange(dragged_line_screen_range.union_(priv->initial_screen_range), reversed);
  } else {
    int row, column;
    get_row_and_column(self, start_x + offset_x, start_y + offset_y, row, column);
    priv->text_editor->selectToScreenPosition(Point(row, column), true);
  }
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void update(AtomTextEditorWidget *self, bool redraw) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  const double padding = round(priv->char_width);
  const double gutter_width = padding * 4 + round(count_digits(priv->text_editor->getScreenLineCount()) * priv->char_width);
  if (gutter_width != priv->gutter_width) {
    priv->gutter_width = gutter_width;
    GtkAllocation allocation;
    gtk_widget_get_allocation(GTK_WIDGET(self), &allocation);
    gdk_window_move_resize(priv->text_window, allocation.x + gutter_width, allocation.y, allocation.width - gutter_width, allocation.height);
  }
  if (priv->vadjustment) {
    const double page_size = gtk_widget_get_allocated_height(GTK_WIDGET(self));
    const double upper = std::max(priv->text_editor->getScreenLineCount() * priv->line_height, page_size);
    const double max_value = std::max(upper - page_size, 0.0);
    g_object_freeze_notify(G_OBJECT(priv->vadjustment));
    gtk_adjustment_set_page_size(priv->vadjustment, page_size);
    gtk_adjustment_set_upper(priv->vadjustment, upper);
    if (gtk_adjustment_get_value(priv->vadjustment) > max_value) {
      gtk_adjustment_set_value(priv->vadjustment, max_value);
    }
    g_object_thaw_notify(G_OBJECT(priv->vadjustment));
  }
  if (redraw) gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void get_row_and_column(AtomTextEditorWidget *self, double x, double y, int &row, int &column) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  const double vadjustment = gtk_adjustment_get_value(priv->vadjustment);
  row = MAX((y + vadjustment) / priv->line_height, 0.0);
  if (row < priv->text_editor->getScreenLineCount()) {
    Layout layout = priv->layout_cache->get_layout(self, priv->text_editor->displayLayer->getScreenLine(row));
    column = layout.x_to_index(x - priv->gutter_width);
  } else {
    column = 0;
  }
}

static void atom_text_editor_widget_move_up(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveUp();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_down(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveDown();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_left(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveLeft();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_right(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveRight();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_to_first_character_of_line(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveToFirstCharacterOfLine();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_to_end_of_line(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveToEndOfLine();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_to_beginning_of_word(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveToBeginningOfWord();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_to_end_of_word(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveToEndOfWord();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_to_previous_subword_boundary(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveToPreviousSubwordBoundary();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_to_next_subword_boundary(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveToNextSubwordBoundary();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_to_top(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveToTop();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_move_to_bottom(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveToBottom();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_all(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectAll();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_up(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectUp();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_down(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectDown();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_left(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectLeft();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_right(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectRight();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_to_first_character_of_line(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectToFirstCharacterOfLine();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_to_end_of_line(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectToEndOfLine();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_to_beginning_of_word(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectToBeginningOfWord();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_to_end_of_word(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectToEndOfWord();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_to_previous_subword_boundary(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectToPreviousSubwordBoundary();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_to_next_subword_boundary(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectToNextSubwordBoundary();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_to_top(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectToTop();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_to_bottom(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectToBottom();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_line(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->selectLinesContainingCursors();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_consolidate_selections(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->consolidateSelections();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_add_selection_above(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->addSelectionAbove();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_add_selection_below(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->addSelectionBelow();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_select_next(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->select_next->findAndSelectNext();
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void atom_text_editor_widget_insert_newline(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->insertNewline();
  update(self);
}

static void atom_text_editor_widget_insert_newline_above(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->insertNewlineAbove();
  update(self);
}

static void atom_text_editor_widget_insert_newline_below(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->insertNewlineBelow();
  update(self);
}

static void atom_text_editor_widget_backspace(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->transact(priv->text_editor->getUndoGroupingInterval(), [&]() {
    priv->text_editor->backspace();
  });
  update(self);
}

static void atom_text_editor_widget_delete(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->transact(priv->text_editor->getUndoGroupingInterval(), [&]() {
    priv->text_editor->delete_();
  });
  update(self);
}

static void atom_text_editor_widget_delete_to_beginning_of_word(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->deleteToBeginningOfWord();
  update(self);
}

static void atom_text_editor_widget_delete_to_end_of_word(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->deleteToEndOfWord();
  update(self);
}

static void atom_text_editor_widget_delete_to_beginning_of_subword(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->deleteToBeginningOfSubword();
  update(self);
}

static void atom_text_editor_widget_delete_to_end_of_subword(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->deleteToEndOfSubword();
  update(self);
}

static void atom_text_editor_widget_indent(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->indent();
  update(self);
}

static void atom_text_editor_widget_outdent_selected_rows(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->outdentSelectedRows();
  update(self);
}

static void atom_text_editor_widget_delete_line(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->deleteLine();
  update(self);
}

static void atom_text_editor_widget_duplicate_lines(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->duplicateLines();
  update(self);
}

static void atom_text_editor_widget_move_line_up(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveLineUp();
  update(self);
}

static void atom_text_editor_widget_move_line_down(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->moveLineDown();
  update(self);
}

static void atom_text_editor_widget_undo(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->undo();
  update(self);
}

static void atom_text_editor_widget_redo(AtomTextEditorWidget *self) {
  AtomTextEditorWidgetPrivate *priv = GET_PRIVATE(self);
  priv->text_editor->redo();
  update(self);
}
