class Window : Gtk.ApplicationWindow {
  private Atom.TextEditorWidget text_editor_widget;
  private Gtk.FileChooserNative dialog;

  public Window(Gtk.Application application) {
    Object(application: application);

    var open_action = new SimpleAction("open", null);
    open_action.activate.connect(open);
    add_action(open_action);
    var save_action = new SimpleAction("save", null);
    save_action.activate.connect(save);
    add_action(save_action);
    var save_as_action = new SimpleAction("save-as", null);
    save_as_action.activate.connect(save_as);
    add_action(save_as_action);

    var header_bar = new Gtk.HeaderBar();
    header_bar.show_close_button = true;
    header_bar.title = "Atom";
    var open_button = new Gtk.Button.from_icon_name("document-open-symbolic", Gtk.IconSize.BUTTON);
    open_button.tooltip_text = "Open File";
    open_button.action_name = "win.open";
    header_bar.pack_start(open_button);
    var save_button = new Gtk.Button.from_icon_name("document-save-symbolic", Gtk.IconSize.BUTTON);
    save_button.tooltip_text = "Save";
    save_button.action_name = "win.save";
    header_bar.pack_start(save_button);
    set_titlebar(header_bar);

    set_default_size(750, 500);
    var notebook = new Gtk.Notebook();
    notebook.show_border = false;
    add(notebook);
  }

  public void append_tab(File? file = null) {
    var notebook = get_child() as unowned Gtk.Notebook;
    var box = new Gtk.Box(Gtk.Orientation.VERTICAL, 0);
    var scrolled_window = new Gtk.ScrolledWindow(null, null);
    text_editor_widget = new Atom.TextEditorWidget(file);
    scrolled_window.add(text_editor_widget);
    box.pack_start(scrolled_window, true);
    var status_bar = new Gtk.Statusbar();
    status_bar.margin = 0;
    status_bar.pack_end(new Gtk.Label("UTF-8"), false);
    box.pack_start(status_bar, false);
    var label = create_tab_label(text_editor_widget);
    label.show_all();
    box.show_all();
    int index = notebook.append_page(box, label);
    notebook.set_tab_reorderable(box, true);
    notebook.child_set_property(box, "tab-expand", true);
    notebook.set_current_page(index);
    text_editor_widget.grab_focus();
  }

  private Gtk.Widget create_tab_label(Atom.TextEditorWidget text_editor_widget) {
    var box = new Gtk.Box(Gtk.Orientation.HORIZONTAL, 0);

    var label = new Gtk.Label(null);
    text_editor_widget.bind_property("title", label, "label", BindingFlags.SYNC_CREATE);
    box.set_center_widget(label);

    var close_button = new Gtk.Button();
    // use a Gtk.Arrow so that the icon can be set in CSS using -gtk-icon-source
    var close_image = new Gtk.Arrow(Gtk.ArrowType.DOWN, Gtk.ShadowType.NONE);
    close_button.add(close_image);
    close_button.relief = Gtk.ReliefStyle.NONE;
    close_button.focus_on_click = false;
    close_button.clicked.connect(() => {
      var notebook = box.get_parent() as unowned Gtk.Notebook;
      int index = notebook.page_num(text_editor_widget.get_parent().get_parent());
      notebook.remove_page(index);
    });
    box.pack_end(close_button, false, true);

    text_editor_widget.notify["modified"].connect(() => {
      if (text_editor_widget.modified) {
        box.get_style_context().add_class("modified");
      } else {
        box.get_style_context().remove_class("modified");
      }
    });

    return box;
  }

  private void open() {
    dialog = new Gtk.FileChooserNative(null, this, Gtk.FileChooserAction.OPEN, null, null);
    dialog.select_multiple = true;
    dialog.response.connect((response) => {
      if (response == Gtk.ResponseType.ACCEPT) {
        foreach (var file in dialog.get_files()) {
          append_tab(file);
        }
      }
      dialog.destroy();
    });
    dialog.show();
  }

  private void save() {
    if (!text_editor_widget.save()) {
      save_as();
    }
  }

  private void save_as() {
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
