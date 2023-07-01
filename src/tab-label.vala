namespace Atom {

class TabLabel : Gtk.Box {
  public TabLabel(Atom.TextEditorWidget text_editor_widget) {
    var label = new Gtk.Label(null);
    text_editor_widget.bind_property("title", label, "label", BindingFlags.SYNC_CREATE);
    set_center_widget(label);

    var close_button = new Gtk.Button();
    // use a Gtk.Arrow so that the icon can be set in CSS using -gtk-icon-source
    var close_image = new Gtk.Arrow(Gtk.ArrowType.DOWN, Gtk.ShadowType.NONE);
    close_button.add(close_image);
    close_button.relief = Gtk.ReliefStyle.NONE;
    close_button.focus_on_click = false;
    close_button.clicked.connect(() => {
      var notebook = get_parent() as unowned Gtk.Notebook;
      int index = notebook.page_num(text_editor_widget.get_parent().get_parent());
      notebook.remove_page(index);
    });
    pack_end(close_button, false, true);

    text_editor_widget.notify["modified"].connect(() => {
      if (text_editor_widget.modified) {
        get_style_context().add_class("modified");
      } else {
        get_style_context().remove_class("modified");
      }
    });
  }
}

}
