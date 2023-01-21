class Window: Gtk.ApplicationWindow {
  public Window(Gtk.Application application, File? file = null) {
    Object(application: application);
    set_default_size(800, 550);
    var text_editor_widget = new Atom.TextEditorWidget(file);
    text_editor_widget.show_all();
    add(text_editor_widget);
  }
}
