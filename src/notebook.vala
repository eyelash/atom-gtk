namespace Atom {

class Notebook : Gtk.Notebook {
  public Notebook() {
    Object(show_border: false);
  }

  public unowned Atom.TextEditorWidget get_current_text_editor() {
    int index = get_current_page();
    var container = get_nth_page(index) as unowned Atom.TextEditorContainer;
    return container.get_text_editor();
  }
}

}
