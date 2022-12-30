[CCode(cheader_filename = "atom-glib.h")]
namespace Atom {
  public class TextEditor : GLib.Object {
    public TextEditor();
    public int get_screen_line_count();
    public void get_screen_line(int row, Pango.Layout layout);
    public void get_cursors(int row, GLib.Array<int> cursors);
    public void get_selections(int row, GLib.Array<int> selections);
    public void move_up();
    public void move_down();
    public void move_left();
    public void move_right();
    public void move_to_first_character_of_line();
    public void move_to_end_of_line();
    public void move_to_beginning_of_word();
    public void move_to_end_of_word();
    public void move_to_top();
    public void move_to_bottom();
    public void select_all();
    public void select_up();
    public void select_down();
    public void select_left();
    public void select_right();
    public void select_to_first_character_of_line();
    public void select_to_end_of_line();
    public void select_to_beginning_of_word();
    public void select_to_end_of_word();
    public void select_to_top();
    public void select_to_bottom();
    public void select_lines_containing_cursors();
    public void consolidate_selections();
    public void add_selection_above();
    public void add_selection_below();
    public void insert_text(string text);
    public void insert_newline();
    public void insert_newline_above();
    public void insert_newline_below();
    public void backspace();
    public void delete();
    public void delete_to_beginning_of_word();
    public void delete_to_end_of_word();
    public void indent();
    public void outdent_selected_rows();
    public void delete_line();
    public void duplicate_lines();
    public void move_line_up();
    public void move_line_down();
    public Gdk.RGBA get_background_color();
    public Gdk.RGBA get_text_color();
    public Gdk.RGBA get_cursor_color();
    public Gdk.RGBA get_selection_color();
  }
}
