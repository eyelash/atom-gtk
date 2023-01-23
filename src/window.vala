class Window: Gtk.ApplicationWindow {
  public Window(Gtk.Application application, File? file = null) {
    Object(application: application);
    set_default_size(800, 550);
    var scrolled_window = new Gtk.ScrolledWindow(null, null);
    var text_editor_widget = new Atom.TextEditorWidget(file);
    scrolled_window.add(text_editor_widget);
    scrolled_window.show_all();
    add(scrolled_window);
  }
}
