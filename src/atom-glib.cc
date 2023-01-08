#include "atom-glib.h"
#include <text-editor.h>
#include <display-marker.h>
#include <decoration-manager.h>
#include <selection.h>
#include <select-next.h>
#include "one-dark.h"

struct _AtomTextEditor {
  GObject parent_instance;
  TextEditor *text_editor;
  SelectNext *select_next;
};
G_DEFINE_TYPE(AtomTextEditor, atom_text_editor, G_TYPE_OBJECT)

static void atom_text_editor_dispose(GObject *gobject) {
  AtomTextEditor *self = ATOM_TEXT_EDITOR(gobject);
  delete self->select_next;
  delete self->text_editor;
  G_OBJECT_CLASS(atom_text_editor_parent_class)->dispose(gobject);
}

static void atom_text_editor_class_init(AtomTextEditorClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->dispose = atom_text_editor_dispose;
}

static void atom_text_editor_init(AtomTextEditor *self) {
  self->text_editor = new TextEditor();
  self->select_next = new SelectNext(self->text_editor);
}

AtomTextEditor *atom_text_editor_new() {
  return ATOM_TEXT_EDITOR(g_object_new(ATOM_TYPE_TEXT_EDITOR, NULL));
}

gint atom_text_editor_get_screen_line_count(AtomTextEditor *self) {
  return self->text_editor->getScreenLineCount();
}

void atom_text_editor_get_screen_line(AtomTextEditor *self, gint row, PangoLayout *layout) {
  DisplayLayer *display_layer = self->text_editor->displayLayer;
  auto screen_line = display_layer->getScreenLine(row);
  const std::u16string &text = screen_line.lineText;
  gchar *utf8 = g_utf16_to_utf8((const gunichar2 *)text.c_str(), text.size(), NULL, NULL, NULL);
  pango_layout_set_text(layout, utf8, -1);
  PangoAttrList *attrs = pango_attr_list_new();
  int32_t index = 0;
  int32_t start;
  for (int32_t tag : screen_line.tags) {
    if (display_layer->isOpenTag(tag)) {
      start = index;
    } else if (display_layer->isCloseTag(tag)) {
      PangoAttribute *attr = pango_attr_foreground_new(
        red(one_dark::syntax_invisible_character_color) / 255.0 * G_MAXUINT16,
        green(one_dark::syntax_invisible_character_color) / 255.0 * G_MAXUINT16,
        blue(one_dark::syntax_invisible_character_color) / 255.0 * G_MAXUINT16
      );
      attr->start_index = g_utf8_offset_to_pointer(utf8, start) - utf8;
      attr->end_index = g_utf8_offset_to_pointer(utf8, index) - utf8;
      pango_attr_list_insert(attrs, attr);
      attr = pango_attr_foreground_alpha_new(
        alpha(one_dark::syntax_invisible_character_color) * G_MAXUINT16
      );
      attr->start_index = g_utf8_offset_to_pointer(utf8, start) - utf8;
      attr->end_index = g_utf8_offset_to_pointer(utf8, index) - utf8;
      pango_attr_list_insert(attrs, attr);
    } else {
      index += tag;
    }
  }
  pango_layout_set_attributes(layout, attrs);
  pango_attr_list_unref(attrs);
  g_free(utf8);
}

static bool is_cursor(const std::pair<DisplayMarker *, std::vector<Decoration::Properties>> &decoration) {
  for (const auto &property: decoration.second) {
    if (property.type == Decoration::Type::cursor) {
      return true;
    }
  }
  return false;
}

void atom_text_editor_get_cursors(AtomTextEditor *self, gint row, GArray *cursors) {
  auto decorations_by_marker = self->text_editor->decorationManager->decorationPropertiesByMarkerForScreenRowRange(row, row + 1);
  for (const auto &decoration: decorations_by_marker) {
    if (is_cursor(decoration)) {
      Point head = decoration.first->getHeadScreenPosition();
      if (head.row == row) {
        gint column = head.column;
        g_array_append_val(cursors, column);
      }
    }
  }
}

void atom_text_editor_get_selections(AtomTextEditor *self, gint row, GArray *selections) {
  const double line_length = self->text_editor->lineLengthForScreenRow(row);
  auto decorations_by_marker = self->text_editor->decorationManager->decorationPropertiesByMarkerForScreenRowRange(row, row + 1);
  for (const auto &decoration: decorations_by_marker) {
    if (is_cursor(decoration)) {
      Range range = decoration.first->getScreenRange();
      if (range.start.row <= row && range.end.row >= row) {
        gint start_column = range.start.row < row ? 0 : range.start.column;
        gint end_column = range.end.row > row ? line_length : range.end.column;
        g_array_append_val(selections, start_column);
        g_array_append_val(selections, end_column);
      }
    }
  }
}

void atom_text_editor_set_cursor_screen_position(AtomTextEditor *self, gint row, gint column) {
  self->text_editor->setCursorScreenPosition(Point(row, column));
}

void atom_text_editor_select_to_screen_position(AtomTextEditor *self, gint row, gint column, gboolean suppress_selection_merge) {
  self->text_editor->selectToScreenPosition(Point(row, column), suppress_selection_merge);
}

void atom_text_editor_select_word(AtomTextEditor *self) {
  self->text_editor->getLastSelection()->selectWord();
}

void atom_text_editor_select_line(AtomTextEditor *self) {
  self->text_editor->getLastSelection()->selectLine();
}

void atom_text_editor_add_cursor_at_screen_position(AtomTextEditor *self, gint row, gint column) {
  self->text_editor->addCursorAtScreenPosition(Point(row, column));
}

void atom_text_editor_toggle_cursor_at_screen_position(AtomTextEditor *self, gint row, gint column) {
  Selection *selection = self->text_editor->getSelectionAtScreenPosition(Point(row, column));
  if (selection) {
    if (self->text_editor->hasMultipleCursors()) selection->destroy();
  } else {
    self->text_editor->addCursorAtScreenPosition(Point(row, column));
  }
}

void atom_text_editor_merge_intersecting_selections(AtomTextEditor *self) {
  self->text_editor->mergeIntersectingSelections();
}

void atom_text_editor_finalize_selections(AtomTextEditor *self) {
  self->text_editor->finalizeSelections();
}

void atom_text_editor_move_up(AtomTextEditor *self) {
  self->text_editor->moveUp();
}

void atom_text_editor_move_down(AtomTextEditor *self) {
  self->text_editor->moveDown();
}

void atom_text_editor_move_left(AtomTextEditor *self) {
  self->text_editor->moveLeft();
}

void atom_text_editor_move_right(AtomTextEditor *self) {
  self->text_editor->moveRight();
}

void atom_text_editor_move_to_first_character_of_line(AtomTextEditor *self) {
  self->text_editor->moveToFirstCharacterOfLine();
}

void atom_text_editor_move_to_end_of_line(AtomTextEditor *self) {
  self->text_editor->moveToEndOfLine();
}

void atom_text_editor_move_to_beginning_of_word(AtomTextEditor *self) {
  self->text_editor->moveToBeginningOfWord();
}

void atom_text_editor_move_to_end_of_word(AtomTextEditor *self) {
  self->text_editor->moveToEndOfWord();
}

void atom_text_editor_move_to_top(AtomTextEditor *self) {
  self->text_editor->moveToTop();
}

void atom_text_editor_move_to_bottom(AtomTextEditor *self) {
  self->text_editor->moveToBottom();
}

void atom_text_editor_select_all(AtomTextEditor *self) {
  self->text_editor->selectAll();
}

void atom_text_editor_select_up(AtomTextEditor *self) {
  self->text_editor->selectUp();
}

void atom_text_editor_select_down(AtomTextEditor *self) {
  self->text_editor->selectDown();
}

void atom_text_editor_select_left(AtomTextEditor *self) {
  self->text_editor->selectLeft();
}

void atom_text_editor_select_right(AtomTextEditor *self) {
  self->text_editor->selectRight();
}

void atom_text_editor_select_to_first_character_of_line(AtomTextEditor *self) {
  self->text_editor->selectToFirstCharacterOfLine();
}

void atom_text_editor_select_to_end_of_line(AtomTextEditor *self) {
  self->text_editor->selectToEndOfLine();
}

void atom_text_editor_select_to_beginning_of_word(AtomTextEditor *self) {
  self->text_editor->selectToBeginningOfWord();
}

void atom_text_editor_select_to_end_of_word(AtomTextEditor *self) {
  self->text_editor->selectToEndOfWord();
}

void atom_text_editor_select_to_top(AtomTextEditor *self) {
  self->text_editor->selectToTop();
}

void atom_text_editor_select_to_bottom(AtomTextEditor *self) {
  self->text_editor->selectToBottom();
}

void atom_text_editor_select_lines_containing_cursors(AtomTextEditor *self) {
  self->text_editor->selectLinesContainingCursors();
}

void atom_text_editor_consolidate_selections(AtomTextEditor *self) {
  self->text_editor->consolidateSelections();
}

void atom_text_editor_add_selection_above(AtomTextEditor *self) {
  self->text_editor->addSelectionAbove();
}

void atom_text_editor_add_selection_below(AtomTextEditor *self) {
  self->text_editor->addSelectionBelow();
}

void atom_text_editor_find_and_select_next(AtomTextEditor *self) {
  self->select_next->findAndSelectNext();
}

void atom_text_editor_insert_text(AtomTextEditor *self, const gchar *text) {
  gunichar2 *utf16 = g_utf8_to_utf16(text, -1, NULL, NULL, NULL);
  self->text_editor->insertText((const char16_t *)utf16);
  g_free(utf16);
}

void atom_text_editor_insert_newline(AtomTextEditor *self) {
  self->text_editor->insertNewline();
}

void atom_text_editor_insert_newline_above(AtomTextEditor *self) {
  self->text_editor->insertNewlineAbove();
}

void atom_text_editor_insert_newline_below(AtomTextEditor *self) {
  self->text_editor->insertNewlineBelow();
}

void atom_text_editor_backspace(AtomTextEditor *self) {
  self->text_editor->backspace();
}

void atom_text_editor_delete(AtomTextEditor *self) {
  self->text_editor->delete_();
}

void atom_text_editor_delete_to_beginning_of_word(AtomTextEditor *self) {
  self->text_editor->deleteToBeginningOfWord();
}

void atom_text_editor_delete_to_end_of_word(AtomTextEditor *self) {
  self->text_editor->deleteToEndOfWord();
}

void atom_text_editor_indent(AtomTextEditor *self) {
  self->text_editor->indent();
}

void atom_text_editor_outdent_selected_rows(AtomTextEditor *self) {
  self->text_editor->outdentSelectedRows();
}

void atom_text_editor_delete_line(AtomTextEditor *self) {
  self->text_editor->deleteLine();
}

void atom_text_editor_duplicate_lines(AtomTextEditor *self) {
  self->text_editor->duplicateLines();
}

void atom_text_editor_move_line_up(AtomTextEditor *self) {
  self->text_editor->moveLineUp();
}

void atom_text_editor_move_line_down(AtomTextEditor *self) {
  self->text_editor->moveLineDown();
}

static void to_rgba(const less::Color &color, GdkRGBA *rgba) {
  rgba->red = red(color) / 255.0;
  rgba->green = green(color) / 255.0;
  rgba->blue = blue(color) / 255.0;
  rgba->alpha = alpha(color);
}

void atom_text_editor_get_background_color(AtomTextEditor *self, GdkRGBA *color) {
  to_rgba(one_dark::syntax_background_color, color);
}

void atom_text_editor_get_text_color(AtomTextEditor *self, GdkRGBA *color) {
  to_rgba(one_dark::syntax_text_color, color);
}

void atom_text_editor_get_cursor_color(AtomTextEditor *self, GdkRGBA *color) {
  to_rgba(one_dark::syntax_cursor_color, color);
}

void atom_text_editor_get_selection_color(AtomTextEditor *self, GdkRGBA *color) {
  to_rgba(one_dark::syntax_selection_color, color);
}
