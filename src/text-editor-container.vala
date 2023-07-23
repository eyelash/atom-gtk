namespace Atom {

class TextEditorContainer : Gtk.Box {
  private Atom.TextEditorWidget text_editor_widget;

  public TextEditorContainer(File? file = null) {
    Object(orientation: Gtk.Orientation.VERTICAL);

    var scrolled_window = new Gtk.ScrolledWindow(null, null);
    text_editor_widget = new Atom.TextEditorWidget(file);
    scrolled_window.add(text_editor_widget);
    pack_start(scrolled_window, true);
    var status_bar = new Gtk.Statusbar();
    status_bar.margin = 0;
    var cursor_position_label = new Gtk.Label(null);
    text_editor_widget.bind_property("cursor-position", cursor_position_label, "label", BindingFlags.SYNC_CREATE);
    status_bar.pack_start(cursor_position_label, false);
    status_bar.reorder_child(cursor_position_label, 0);
    var grammar_label = new Gtk.Label(null);
    text_editor_widget.bind_property("grammar", grammar_label, "label", BindingFlags.SYNC_CREATE);
    status_bar.pack_end(grammar_label, false);
    status_bar.pack_end(new Gtk.Label("UTF-8"), false);
    pack_start(status_bar, false);
  }

  public unowned Atom.TextEditorWidget get_text_editor() {
    return text_editor_widget;
  }
}

}
