namespace Atom {

class Application : Gtk.Application {
  public Application() {
    Object(application_id: "com.github.eyelash.atom-gtk", flags: ApplicationFlags.HANDLES_OPEN);
  }

  public override void startup() {
    base.startup();
    Gtk.Settings.get_default().gtk_application_prefer_dark_theme = true;
    load_css("/com/github/eyelash/atom-gtk/one-dark.css");
    set_accels_for_action("win.open", {"<Primary>O"});
    set_accels_for_action("win.save", {"<Primary>S"});
    set_accels_for_action("win.save-as", {"<Primary><Shift>S"});
    var window = new Window(this);
    window.show_all();
    window.present();
  }

  public override void activate() {
    var window = get_active_window() as unowned Atom.Window;
    window.append_tab();
  }

  public override void open(File[] files, string hint) {
    var window = get_active_window() as unowned Atom.Window;
    foreach (var file in files) {
      window.append_tab(file);
    }
  }

  private void load_css(string resource_path) {
    var css_provider = new Gtk.CssProvider();
    css_provider.load_from_resource(resource_path);
    Gtk.StyleContext.add_provider_for_screen(Gdk.Screen.get_default(), css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);
  }

  public static int main(string[] args) {
    return new Application().run(args);
  }
}

}
