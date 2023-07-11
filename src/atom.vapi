namespace Atom {
  [CCode(cheader_filename = "text-editor-widget.h")]
  public class TextEditorWidget : Gtk.Widget {
    public string title { owned get; }
    public bool modified { get; }
    public string cursor_position { owned get; }
    public TextEditorWidget(GLib.File? file);
    public bool save();
    public void save_as(GLib.File file);
  }
}
