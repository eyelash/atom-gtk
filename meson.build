project('com.github.eyelash.atom-gtk', 'vala', 'c', 'cpp')

executable(
  meson.project_name(),
  'src/application.vala',
  'src/window.vala',
  'src/notebook.vala',
  'src/tab-label.vala',
  'src/text-editor-container.vala',
  'src/atom.vapi',
  'src/text-editor-widget.cc',
  import('gnome').compile_resources(
    'data',
    'data/gresource.xml',
    dependencies: custom_target(
      'one-dark',
      input: 'data/one-dark.scss',
      output: 'one-dark.css',
      command: [find_program('sassc'), '@INPUT@', '@OUTPUT@']
    ),
    source_dir: 'data',
  ),
  include_directories: include_directories(
    'src'
  ),
  dependencies: [
    subproject('atom-native').get_variable('atom_dep'),
    dependency('gtk+-3.0'),
  ],
  install: true,
)
configuration = {
  'bindir': get_option('prefix') / get_option('bindir'),
}
install_data(
  'data/com.github.eyelash.atom-gtk.desktop',
  install_dir: get_option('datadir') / 'applications',
)
configure_file(
  input: 'data/com.github.eyelash.atom-gtk.service.in',
  output: 'com.github.eyelash.atom-gtk.service',
  configuration: configuration,
  install_dir: get_option('datadir') / 'dbus-1' / 'services',
)
install_data(
  'data/com.github.eyelash.atom-gtk.svg',
  install_dir: get_option('datadir') / 'icons' / 'hicolor' / 'scalable' / 'apps',
)
#install_data(
#  'data/com.github.eyelash.atom-gtk-symbolic.svg',
#  install_dir: get_option('datadir') / 'icons' / 'hicolor' / 'symbolic' / 'apps',
#)
install_data(
  'data/atom-gtk',
  install_dir: get_option('bindir'),
)
