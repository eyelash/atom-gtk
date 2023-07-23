#ifndef ATOM_TEXT_EDITOR_WIDGET_H_
#define ATOM_TEXT_EDITOR_WIDGET_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define ATOM_TYPE_TEXT_EDITOR_WIDGET atom_text_editor_widget_get_type()
G_DECLARE_DERIVABLE_TYPE(AtomTextEditorWidget, atom_text_editor_widget, ATOM, TEXT_EDITOR_WIDGET, GtkWidget)
struct _AtomTextEditorWidgetClass {
  GtkWidgetClass parent_class;
  void (*move_up)(AtomTextEditorWidget *);
  void (*move_down)(AtomTextEditorWidget *);
  void (*move_left)(AtomTextEditorWidget *);
  void (*move_right)(AtomTextEditorWidget *);
  void (*move_to_first_character_of_line)(AtomTextEditorWidget *);
  void (*move_to_end_of_line)(AtomTextEditorWidget *);
  void (*move_to_beginning_of_word)(AtomTextEditorWidget *);
  void (*move_to_end_of_word)(AtomTextEditorWidget *);
  void (*move_to_previous_subword_boundary)(AtomTextEditorWidget *);
  void (*move_to_next_subword_boundary)(AtomTextEditorWidget *);
  void (*page_up)(AtomTextEditorWidget *);
  void (*page_down)(AtomTextEditorWidget *);
  void (*move_to_top)(AtomTextEditorWidget *);
  void (*move_to_bottom)(AtomTextEditorWidget *);
  void (*select_all)(AtomTextEditorWidget *);
  void (*select_up)(AtomTextEditorWidget *);
  void (*select_down)(AtomTextEditorWidget *);
  void (*select_left)(AtomTextEditorWidget *);
  void (*select_right)(AtomTextEditorWidget *);
  void (*select_to_first_character_of_line)(AtomTextEditorWidget *);
  void (*select_to_end_of_line)(AtomTextEditorWidget *);
  void (*select_to_beginning_of_word)(AtomTextEditorWidget *);
  void (*select_to_end_of_word)(AtomTextEditorWidget *);
  void (*select_to_previous_subword_boundary)(AtomTextEditorWidget *);
  void (*select_to_next_subword_boundary)(AtomTextEditorWidget *);
  void (*select_page_up)(AtomTextEditorWidget *);
  void (*select_page_down)(AtomTextEditorWidget *);
  void (*select_to_top)(AtomTextEditorWidget *);
  void (*select_to_bottom)(AtomTextEditorWidget *);
  void (*select_line)(AtomTextEditorWidget *);
  void (*select_larger_syntax_node)(AtomTextEditorWidget *);
  void (*select_smaller_syntax_node)(AtomTextEditorWidget *);
  void (*consolidate_selections)(AtomTextEditorWidget *);
  void (*add_selection_above)(AtomTextEditorWidget *);
  void (*add_selection_below)(AtomTextEditorWidget *);
  void (*select_next)(AtomTextEditorWidget *);
  void (*insert_newline)(AtomTextEditorWidget *);
  void (*insert_newline_above)(AtomTextEditorWidget *);
  void (*insert_newline_below)(AtomTextEditorWidget *);
  void (*backspace)(AtomTextEditorWidget *);
  void (*delete_)(AtomTextEditorWidget *);
  void (*delete_to_beginning_of_word)(AtomTextEditorWidget *);
  void (*delete_to_end_of_word)(AtomTextEditorWidget *);
  void (*delete_to_beginning_of_subword)(AtomTextEditorWidget *);
  void (*delete_to_end_of_subword)(AtomTextEditorWidget *);
  void (*indent)(AtomTextEditorWidget *);
  void (*outdent_selected_rows)(AtomTextEditorWidget *);
  void (*delete_line)(AtomTextEditorWidget *);
  void (*duplicate_lines)(AtomTextEditorWidget *);
  void (*move_line_up)(AtomTextEditorWidget *);
  void (*move_line_down)(AtomTextEditorWidget *);
  void (*undo)(AtomTextEditorWidget *);
  void (*redo)(AtomTextEditorWidget *);
  void (*copy)(AtomTextEditorWidget *);
  void (*cut)(AtomTextEditorWidget *);
  void (*paste)(AtomTextEditorWidget *);
};

AtomTextEditorWidget *atom_text_editor_widget_new(GFile *);
gchar *atom_text_editor_widget_get_title(AtomTextEditorWidget *);
gboolean atom_text_editor_widget_get_modified(AtomTextEditorWidget *);
gchar *atom_text_editor_widget_get_cursor_position(AtomTextEditorWidget *);
gchar *atom_text_editor_widget_get_selection_count(AtomTextEditorWidget *);
const gchar *atom_text_editor_widget_get_grammar(AtomTextEditorWidget *);
gboolean atom_text_editor_widget_save(AtomTextEditorWidget *);
void atom_text_editor_widget_save_as(AtomTextEditorWidget *, GFile *);

G_END_DECLS

#endif  // ATOM_TEXT_EDITOR_WIDGET_H_
