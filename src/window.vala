class Window : Gtk.ApplicationWindow {
  private Atom.TextEditorWidget text_editor_widget;
  private Gtk.FileChooserNative dialog;

  public Window(Gtk.Application application, File? file = null) {
    Object(application: application);

    var header_bar = new Gtk.HeaderBar();
    header_bar.show_close_button = true;
    header_bar.title = "Atom";
    var open_button = new Gtk.Button.from_icon_name("document-open-symbolic", Gtk.IconSize.BUTTON);
    open_button.tooltip_text = "Open File";
    open_button.clicked.connect(open);
    header_bar.pack_start(open_button);
    var save_button = new Gtk.Button.from_icon_name("document-save-symbolic", Gtk.IconSize.BUTTON);
    save_button.tooltip_text = "Save";
    save_button.clicked.connect(save);
    header_bar.pack_start(save_button);
    set_titlebar(header_bar);

    set_default_size(750, 500);
    var box = new Gtk.Box(Gtk.Orientation.VERTICAL, 0);
    var scrolled_window = new Gtk.ScrolledWindow(null, null);
    text_editor_widget = new Atom.TextEditorWidget(file);
    scrolled_window.add(text_editor_widget);
    box.pack_start(scrolled_window, true);
    var status_bar = new Gtk.Statusbar();
    status_bar.margin = 0;
    status_bar.pack_end(new Gtk.Label("UTF-8"), false);
    box.pack_start(status_bar, false);
    add(box);
  }

  private void open() {
    dialog = new Gtk.FileChooserNative(null, this, Gtk.FileChooserAction.OPEN, null, null);
    dialog.select_multiple = true;
    dialog.response.connect((response) => {
      if (response == Gtk.ResponseType.ACCEPT) {
        foreach (var file in dialog.get_files()) {
          var window = new Window(application, file);
          window.show_all();
          window.present();
        }
      }
      dialog.destroy();
    });
    dialog.show();
  }

  private void save() {
    if (!text_editor_widget.save()) {
      dialog = new Gtk.FileChooserNative(null, this, Gtk.FileChooserAction.SAVE, null, null);
      dialog.do_overwrite_confirmation = true;
      dialog.response.connect((response) => {
        if (response == Gtk.ResponseType.ACCEPT) {
          text_editor_widget.save_as(dialog.get_file());
        }
        dialog.destroy();
      });
      dialog.show();
    }
  }
}
