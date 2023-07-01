namespace Atom {

class Window : Gtk.ApplicationWindow {
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
    var notebook = new Atom.Notebook();
    add(notebook);
  }

  public void append_tab(File? file = null) {
    var notebook = get_child() as unowned Atom.Notebook;
    var container = new Atom.TextEditorContainer(file);
    var label = new Atom.TabLabel(container.get_text_editor());
    label.show_all();
    container.show_all();
    int index = notebook.append_page(container, label);
    notebook.set_tab_reorderable(container, true);
    notebook.child_set_property(container, "tab-expand", true);
    notebook.set_current_page(index);
    container.get_text_editor().grab_focus();
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
    var notebook = get_child() as unowned Atom.Notebook;
    var text_editor_widget = notebook.get_current_text_editor();
    if (!text_editor_widget.save()) {
      save_as();
    }
  }

  private void save_as() {
    dialog = new Gtk.FileChooserNative(null, this, Gtk.FileChooserAction.SAVE, null, null);
    dialog.do_overwrite_confirmation = true;
    dialog.response.connect((response) => {
      if (response == Gtk.ResponseType.ACCEPT) {
        var notebook = get_child() as unowned Atom.Notebook;
        var text_editor_widget = notebook.get_current_text_editor();
        text_editor_widget.save_as(dialog.get_file());
      }
      dialog.destroy();
    });
    dialog.show();
  }
}

}
