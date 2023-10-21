namespace Atom {

class Statusbar : Gtk.Box {
  static construct {
    set_css_name("statusbar");
  }

  public Statusbar(Atom.TextEditorWidget text_editor_widget) {
    Object(orientation: Gtk.Orientation.HORIZONTAL, spacing: 2);

    var path_label = new Gtk.Label(null);
    text_editor_widget.bind_property("path", path_label, "label", BindingFlags.SYNC_CREATE);
    pack_start(pack(path_label), false);

    var cursor_position_label = new Gtk.Label(null);
    text_editor_widget.bind_property("cursor-position", cursor_position_label, "label", BindingFlags.SYNC_CREATE);
    pack_start(pack(cursor_position_label), false);

    var selection_count_label = new Gtk.Label(null);
    text_editor_widget.bind_property("selection-count", selection_count_label, "label", BindingFlags.SYNC_CREATE);
    pack_start(pack(selection_count_label), false);

    var grammar_label = new Gtk.Label(null);
    text_editor_widget.bind_property("grammar", grammar_label, "label", BindingFlags.SYNC_CREATE);
    pack_end(pack(grammar_label), false);

    var encoding_label = new Gtk.Label("UTF-8");
    encoding_label.tooltip_text = "This file uses UTF-8 encoding";
    pack_end(pack(encoding_label), false);
  }

  private static Gtk.Widget pack(Gtk.Widget child) {
    var frame = new Gtk.Frame(null);
    frame.shadow_type = Gtk.ShadowType.NONE;
    var box = new Gtk.Box(Gtk.Orientation.HORIZONTAL, 4);
    box.margin_top = 4;
    box.margin_bottom = 4;
    box.margin_start = 11;
    box.margin_end = 11;
    box.add(child);
    frame.add(box);
    return frame;
  }
}

}
