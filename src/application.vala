class Application: Gtk.Application {
  public Application() {
    Object(application_id: "com.github.eyelash.atom-gtk", flags: ApplicationFlags.HANDLES_OPEN);
  }

  public override void startup() {
    base.startup();
    Gtk.Settings.get_default().gtk_application_prefer_dark_theme = true;
    load_css("/com/github/eyelash/atom-gtk/key-bindings.css");
    load_css("/com/github/eyelash/atom-gtk/one-dark.css");
  }

  public override void activate() {
    var window = new Window(this);
    window.present();
  }

  public override void open(File[] files, string hint) {
    foreach (var file in files) {
      var window = new Window(this, file);
      window.present();
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
