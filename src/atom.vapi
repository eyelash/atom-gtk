namespace Atom {
  [CCode(cheader_filename = "text-editor-widget.h")]
  public class TextEditorWidget : Gtk.DrawingArea {
    public string title { owned get; }
    public bool modified { get; }
    public TextEditorWidget(GLib.File? file);
    public bool save();
    public void save_as(GLib.File file);
  }
}
