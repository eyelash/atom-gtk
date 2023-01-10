class Window: Gtk.ApplicationWindow {
  public Window(Gtk.Application application) {
    Object(application: application);
    set_default_size(800, 550);
    var text_editor_widget = new Atom.TextEditorWidget();
    text_editor_widget.show_all();
    add(text_editor_widget);
  }
}
