namespace Atom {

class Statusbar : Gtk.Box {
  static construct {
    set_css_name("statusbar");
  }

  public Statusbar(Atom.TextEditorWidget text_editor_widget) {
    Object(orientation: Gtk.Orientation.HORIZONTAL, spacing: 2);

    var path_label = new Gtk.Label(null);
    path_label.ellipsize = Pango.EllipsizeMode.END;
    text_editor_widget.bind_property("path", path_label, "label", BindingFlags.SYNC_CREATE);
    pack_start(pack(path_label), false);

    var cursor_position_label = new Gtk.Label(null);
    text_editor_widget.bind_property("cursor-position", cursor_position_label, "label", BindingFlags.SYNC_CREATE);
    text_editor_widget.bind_property("cursor-position", cursor_position_label, "tooltip-text", BindingFlags.SYNC_CREATE, (binding, from_value, ref to_value) => {
      var from_values = ((string)from_value).split(":");
      int row = int.parse(from_values[0]);
      int column = int.parse(from_values[1]);
      to_value = "Line %d, Column %d".printf(row, column);
      return true;
    });
    pack_start(pack(cursor_position_label), false);

    var selection_count_label = new Gtk.Label(null);
    text_editor_widget.bind_property("selection-count", selection_count_label, "visible", BindingFlags.SYNC_CREATE, (binding, from_value, ref to_value) => {
      to_value = ((string)from_value).length > 0;
      return true;
    });
    text_editor_widget.bind_property("selection-count", selection_count_label, "label", BindingFlags.SYNC_CREATE, (binding, from_value, ref to_value) => {
      if (((string)from_value).length == 0) {
        to_value = "";
      } else {
        var from_values = ((string)from_value).split(":");
        int line_count = int.parse(from_values[0]);
        int count = int.parse(from_values[1]);
        to_value = "(%d, %d)".printf(line_count, count);
      }
      return true;
    });
    text_editor_widget.bind_property("selection-count", selection_count_label, "tooltip-text", BindingFlags.SYNC_CREATE, (binding, from_value, ref to_value) => {
      if (((string)from_value).length == 0) {
        to_value = "";
      } else {
        var from_values = ((string)from_value).split(":");
        int line_count = int.parse(from_values[0]);
        int count = int.parse(from_values[1]);
        to_value = "%s, %s selected".printf(pluralize(line_count, "line"), pluralize(count, "character"));
      }
      return true;
    });
    pack_start(pack(selection_count_label), false);

    var grammar_label = new Gtk.Label(null);
    text_editor_widget.bind_property("grammar", grammar_label, "label", BindingFlags.SYNC_CREATE);
    text_editor_widget.bind_property("grammar", grammar_label, "tooltip-text", BindingFlags.SYNC_CREATE, (binding, from_value, ref to_value) => {
      to_value = "File uses the %s grammar".printf((string)from_value);
      return true;
    });
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

  private static string pluralize(int count, string singular) {
    if (count == 1) {
      return "%d %s".printf(count, singular);
    } else {
      return "%d %ss".printf(count, singular);
    }
  }
}

}
