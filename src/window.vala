namespace Atom {

class Window : Gtk.ApplicationWindow {
  private Gtk.FileChooserNative dialog;

  public Window(Gtk.Application application) {
    Object(application: application);

    var new_file_action = new SimpleAction("new-file", null);
    new_file_action.activate.connect(new_file);
    add_action(new_file_action);
    var open_action = new SimpleAction("open", null);
    open_action.activate.connect(open);
    add_action(open_action);
    var save_action = new SimpleAction("save", null);
    save_action.activate.connect(save);
    add_action(save_action);
    var save_as_action = new SimpleAction("save-as", null);
    save_as_action.activate.connect(save_as);
    add_action(save_as_action);
    var save_all_action = new SimpleAction("save-all", null);
    save_all_action.activate.connect(save_all);
    add_action(save_all_action);

    var header_bar = new Gtk.HeaderBar();
    header_bar.show_close_button = true;
    header_bar.title = "Atom";
    var new_file = new Gtk.Button.from_icon_name("document-new-symbolic", Gtk.IconSize.BUTTON);
    new_file.tooltip_text = "New File";
    new_file.action_name = "win.new-file";
    header_bar.pack_start(new_file);
    var open_button = new Gtk.Button.from_icon_name("document-open-symbolic", Gtk.IconSize.BUTTON);
    open_button.tooltip_text = "Open File";
    open_button.action_name = "win.open";
    header_bar.pack_start(open_button);
    var save_button = new Gtk.Button.from_icon_name("document-save-symbolic", Gtk.IconSize.BUTTON);
    save_button.tooltip_text = "Save";
    save_button.action_name = "win.save";
    var save_menu_button = new Gtk.MenuButton();
    {
      var save_menu = new Gtk.Menu();
      var save_as_item = new Gtk.MenuItem.with_label("Save As");
      save_as_item.action_name = "win.save-as";
      save_menu.append(save_as_item);
      var save_all_item = new Gtk.MenuItem.with_label("Save All");
      save_all_item.action_name = "win.save-all";
      save_menu.append(save_all_item);
      save_menu.show_all();
      save_menu_button.set_popup(save_menu);
    }
    header_bar.pack_start(linked(save_button, save_menu_button));
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

  private Gtk.Widget linked(Gtk.Widget first, Gtk.Widget second, Gtk.Orientation orientation = Gtk.Orientation.HORIZONTAL) {
    var box = new Gtk.Box(orientation, 0);
    box.get_style_context().add_class(Gtk.STYLE_CLASS_LINKED);
    box.pack_start(first);
    box.pack_start(second);
    return box;
  }

  private void new_file() {
    append_tab();
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

  private void save_all() {
    var notebook = get_child() as unowned Atom.Notebook;
    for (int i = 0; i < notebook.get_n_pages(); i++) {
      var container = notebook.get_nth_page(i) as unowned Atom.TextEditorContainer;
      container.get_text_editor().save();
    }
  }
}

}
