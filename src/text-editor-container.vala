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
    status_bar.pack_end(new Gtk.Label("UTF-8"), false);
    pack_start(status_bar, false);
  }

  public unowned Atom.TextEditorWidget get_text_editor() {
    return text_editor_widget;
  }
}

}
