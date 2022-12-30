class TextEditorWidget: Gtk.DrawingArea {
  private const double PADDING = 12;
  private Atom.TextEditor text_editor;
  private Gtk.IMContext im_context;
  private Gtk.GestureMultiPress multipress_gesture;
  private Pango.FontDescription font_description;
  private double ascent;
  private double line_height;
  private double char_width;

  static construct {
    set_css_name("atom-text-editor-widget");
  }

  public TextEditorWidget() {
    text_editor = new Atom.TextEditor();
    im_context = new Gtk.IMMulticontext();
    im_context.commit.connect(commit);
    multipress_gesture = new Gtk.GestureMultiPress(this);
    multipress_gesture.pressed.connect(handle_pressed);
    var settings = new Settings("org.gnome.desktop.interface");
    font_description = Pango.FontDescription.from_string(settings.get_string("monospace-font-name"));
    var metrics = get_pango_context().get_metrics(font_description, null);
    ascent = Pango.units_to_double(metrics.get_ascent());
    line_height = Pango.units_to_double(metrics.get_ascent() + metrics.get_descent());
    char_width = Pango.units_to_double(metrics.get_approximate_char_width());
    can_focus = true;
  }

  public override bool draw(Cairo.Context cr) {
    Gdk.cairo_set_source_rgba(cr, text_editor.get_background_color());
    cr.paint();
    for (int row = 0; row < text_editor.get_screen_line_count(); row++) {
      double y = PADDING + row * line_height;
      var layout = new Pango.Layout(get_pango_context());
      layout.set_font_description(font_description);
      text_editor.get_screen_line(row, layout);

      var selections = new Array<int>();
      text_editor.get_selections(row, selections);
      Gdk.cairo_set_source_rgba(cr, text_editor.get_selection_color());
      for (int i = 0; i + 1 < selections.length; i += 2) {
        double x = PADDING + index_to_x(selections.index(i), layout);
        double width = PADDING + index_to_x(selections.index(i + 1), layout) - x;
        cr.rectangle(x, y, width, line_height);
        cr.fill();
      }

      Gdk.cairo_set_source_rgba(cr, text_editor.get_text_color());
      cr.move_to(PADDING, y + ascent);
      Pango.cairo_show_layout_line(cr, layout.get_line_readonly(0));

      var cursors = new Array<int>();
      text_editor.get_cursors(row, cursors);
      Gdk.cairo_set_source_rgba(cr, text_editor.get_cursor_color());
      for (int i = 0; i < cursors.length; i++) {
        double x = PADDING + index_to_x(cursors.index(i), layout);
        cr.rectangle(x - 1, y, 2, line_height);
        cr.fill();
      }
    }
    return Gdk.EVENT_STOP;
  }

  public override bool key_press_event(Gdk.EventKey event) {
    if (base.key_press_event(event) || im_context.filter_keypress(event)) {
      return Gdk.EVENT_STOP;
    }
    return Gdk.EVENT_PROPAGATE;
  }

  public override bool key_release_event(Gdk.EventKey event) {
    if (base.key_release_event(event) || im_context.filter_keypress(event)) {
      return Gdk.EVENT_STOP;
    }
    return Gdk.EVENT_PROPAGATE;
  }

  private void commit(string text) {
    text_editor.insert_text(text);
    queue_draw();
  }

  private void handle_pressed(int n_press, double x, double y) {

  }

  private static double index_to_x(int index, Pango.Layout layout) {
    index = layout.get_text().index_of_nth_char(index);
    int x_pos;
    layout.get_line_readonly(0).index_to_x(index, false, out x_pos);
    return Pango.units_to_double(x_pos);
  }

  [Signal(action = true)]
  public virtual signal void move_up() {
    text_editor.move_up();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_down() {
    text_editor.move_down();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_left() {
    text_editor.move_left();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_right() {
    text_editor.move_right();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_to_first_character_of_line() {
    text_editor.move_to_first_character_of_line();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_to_end_of_line() {
    text_editor.move_to_end_of_line();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_to_beginning_of_word() {
    text_editor.move_to_beginning_of_word();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_to_end_of_word() {
    text_editor.move_to_end_of_word();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_to_top() {
    text_editor.move_to_top();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_to_bottom() {
    text_editor.move_to_bottom();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_all() {
    text_editor.select_all();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_up() {
    text_editor.select_up();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_down() {
    text_editor.select_down();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_left() {
    text_editor.select_left();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_right() {
    text_editor.select_right();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_to_first_character_of_line() {
    text_editor.select_to_first_character_of_line();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_to_end_of_line() {
    text_editor.select_to_end_of_line();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_to_beginning_of_word() {
    text_editor.select_to_beginning_of_word();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_to_end_of_word() {
    text_editor.select_to_end_of_word();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_to_top() {
    text_editor.select_to_top();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_to_bottom() {
    text_editor.select_to_bottom();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void select_line() {
    text_editor.select_lines_containing_cursors();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void consolidate_selections() {
    text_editor.consolidate_selections();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void add_selection_above() {
    text_editor.add_selection_above();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void add_selection_below() {
    text_editor.add_selection_below();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void insert_newline() {
    text_editor.insert_newline();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void insert_newline_above() {
    text_editor.insert_newline_above();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void insert_newline_below() {
    text_editor.insert_newline_below();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void backspace() {
    text_editor.backspace();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void delete() {
    text_editor.delete();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void delete_to_beginning_of_word() {
    text_editor.delete_to_beginning_of_word();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void delete_to_end_of_word() {
    text_editor.delete_to_end_of_word();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void indent() {
    text_editor.indent();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void outdent_selected_rows() {
    text_editor.outdent_selected_rows();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void delete_line() {
    text_editor.delete_line();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void duplicate_lines() {
    text_editor.duplicate_lines();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_line_up() {
    text_editor.move_line_up();
    queue_draw();
  }

  [Signal(action = true)]
  public virtual signal void move_line_down() {
    text_editor.move_line_down();
    queue_draw();
  }
}
