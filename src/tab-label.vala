namespace Atom {

class TabLabel : Gtk.Box {
  private Atom.TextEditorWidget text_editor_widget;

  public TabLabel(Atom.TextEditorWidget text_editor_widget) {
    this.text_editor_widget = text_editor_widget;

    var center_box = new Gtk.Box(Gtk.Orientation.HORIZONTAL, 4);
    var label = new Gtk.Label(null);
    text_editor_widget.bind_property("title", label, "label", BindingFlags.SYNC_CREATE);
    center_box.pack_start(label);
    var modified_image = new Gtk.Image.from_icon_name("media-record-symbolic", Gtk.IconSize.MENU);
    modified_image.get_style_context().add_class("modified");
    modified_image.no_show_all = true;
    text_editor_widget.bind_property("modified", modified_image, "visible", BindingFlags.SYNC_CREATE);
    center_box.pack_start(modified_image);
    set_center_widget(center_box);

    var close_button = new Gtk.Button();
    var close_image = new Gtk.Image.from_icon_name("window-close-symbolic", Gtk.IconSize.MENU);
    close_button.add(close_image);
    close_button.relief = Gtk.ReliefStyle.NONE;
    close_button.focus_on_click = false;
    close_button.clicked.connect(() => {
      var notebook = get_parent() as unowned Gtk.Notebook;
      int index = notebook.page_num(this.text_editor_widget.get_parent().get_parent());
      notebook.remove_page(index);
    });
    pack_end(close_button, false);
  }
}

}
