namespace Atom {

class Notebook : Gtk.Notebook {
  public Notebook() {
    Object(show_border: false);
  }

  public void append_tab(File? file = null) {
    var container = new Atom.TextEditorContainer(file);
    var label = create_tab_label(container);
    label.show_all();
    container.show_all();
    int index = append_page(container, label);
    set_tab_reorderable(container, true);
    child_set_property(container, "tab-expand", true);
    set_current_page(index);
    container.get_text_editor().grab_focus();
  }

  public bool save() {
    return get_current_text_editor().save();
  }

  public void save_as(File file) {
    get_current_text_editor().save_as(file);
  }

  public void save_all() {
    for (int index = 0; index < get_n_pages(); index++) {
      get_text_editor(index).save();
    }
  }

  private Gtk.Widget create_tab_label(Atom.TextEditorContainer container) {
    var tab_label = new Gtk.Box(Gtk.Orientation.HORIZONTAL, 0);

    var center_box = new Gtk.Box(Gtk.Orientation.HORIZONTAL, 4);
    var label = new Gtk.Label(null);
    container.get_text_editor().bind_property("title", label, "label", BindingFlags.SYNC_CREATE);
    center_box.pack_start(label);
    var modified_image = new Gtk.Image.from_icon_name("media-record-symbolic", Gtk.IconSize.MENU);
    modified_image.get_style_context().add_class("modified");
    modified_image.no_show_all = true;
    container.get_text_editor().bind_property("modified", modified_image, "visible", BindingFlags.SYNC_CREATE);
    center_box.pack_start(modified_image);
    tab_label.set_center_widget(center_box);

    var close_button = new Gtk.Button();
    var close_image = new Gtk.Image.from_icon_name("window-close-symbolic", Gtk.IconSize.MENU);
    close_button.add(close_image);
    close_button.relief = Gtk.ReliefStyle.NONE;
    close_button.focus_on_click = false;
    close_button.clicked.connect(() => {
      int index = this.page_num(container);
      this.remove_page(index);
    });
    tab_label.pack_end(close_button, false);

    return tab_label;
  }

  private unowned Atom.TextEditorWidget get_current_text_editor() {
    return get_text_editor(get_current_page());
  }

  private unowned Atom.TextEditorWidget get_text_editor(int index) {
    var container = get_nth_page(index) as unowned Atom.TextEditorContainer;
    return container.get_text_editor();
  }
}

}
