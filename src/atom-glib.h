#ifndef ATOM_GLIB_H_
#define ATOM_GLIB_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ATOM_TYPE_TEXT_EDITOR atom_text_editor_get_type()
G_DECLARE_FINAL_TYPE(AtomTextEditor, atom_text_editor, ATOM, TEXT_EDITOR, GObject)

AtomTextEditor *atom_text_editor_new(void);
gint atom_text_editor_get_screen_line_count(AtomTextEditor *);
void atom_text_editor_get_screen_line(AtomTextEditor *, gint, PangoLayout *);
void atom_text_editor_get_cursors(AtomTextEditor *, gint, GArray *);
void atom_text_editor_get_selections(AtomTextEditor *, gint, GArray *);
void atom_text_editor_move_up(AtomTextEditor *);
void atom_text_editor_move_down(AtomTextEditor *);
void atom_text_editor_move_left(AtomTextEditor *);
void atom_text_editor_move_right(AtomTextEditor *);
void atom_text_editor_move_to_first_character_of_line(AtomTextEditor *);
void atom_text_editor_move_to_end_of_line(AtomTextEditor *);
void atom_text_editor_move_to_beginning_of_word(AtomTextEditor *);
void atom_text_editor_move_to_end_of_word(AtomTextEditor *);
void atom_text_editor_move_to_top(AtomTextEditor *);
void atom_text_editor_move_to_bottom(AtomTextEditor *);
void atom_text_editor_select_all(AtomTextEditor *);
void atom_text_editor_select_up(AtomTextEditor *);
void atom_text_editor_select_down(AtomTextEditor *);
void atom_text_editor_select_left(AtomTextEditor *);
void atom_text_editor_select_right(AtomTextEditor *);
void atom_text_editor_select_to_first_character_of_line(AtomTextEditor *);
void atom_text_editor_select_to_end_of_line(AtomTextEditor *);
void atom_text_editor_select_to_beginning_of_word(AtomTextEditor *);
void atom_text_editor_select_to_end_of_word(AtomTextEditor *);
void atom_text_editor_select_to_top(AtomTextEditor *);
void atom_text_editor_select_to_bottom(AtomTextEditor *);
void atom_text_editor_select_lines_containing_cursors(AtomTextEditor *);
void atom_text_editor_consolidate_selections(AtomTextEditor *);
void atom_text_editor_add_selection_above(AtomTextEditor *);
void atom_text_editor_add_selection_below(AtomTextEditor *);
void atom_text_editor_insert_text(AtomTextEditor *, const gchar *);
void atom_text_editor_insert_newline(AtomTextEditor *);
void atom_text_editor_insert_newline_above(AtomTextEditor *);
void atom_text_editor_insert_newline_below(AtomTextEditor *);
void atom_text_editor_backspace(AtomTextEditor *);
void atom_text_editor_delete(AtomTextEditor *);
void atom_text_editor_delete_to_beginning_of_word(AtomTextEditor *);
void atom_text_editor_delete_to_end_of_word(AtomTextEditor *);
void atom_text_editor_indent(AtomTextEditor *);
void atom_text_editor_outdent_selected_rows(AtomTextEditor *);
void atom_text_editor_delete_line(AtomTextEditor *);
void atom_text_editor_duplicate_lines(AtomTextEditor *);
void atom_text_editor_move_line_up(AtomTextEditor *);
void atom_text_editor_move_line_down(AtomTextEditor *);
void atom_text_editor_get_background_color(AtomTextEditor *, GdkRGBA *);
void atom_text_editor_get_text_color(AtomTextEditor *, GdkRGBA *);
void atom_text_editor_get_cursor_color(AtomTextEditor *, GdkRGBA *);
void atom_text_editor_get_selection_color(AtomTextEditor *, GdkRGBA *);

G_END_DECLS

#endif  // ATOM_GLIB_H_
