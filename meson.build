project('atom-gtk', 'vala', 'c', 'cpp')

executable(
  meson.project_name(),
  'src/one-dark.cc',
  'src/atom-glib.cc',
  'src/atom-glib.vapi',
  'src/application.vala',
  'src/window.vala',
  'src/text-editor-widget.vala',
  import('gnome').compile_resources('data', 'data/gresource.xml', source_dir: 'data'),
  include_directories: include_directories(
    'src'
  ),
  dependencies: [
    subproject('atom-native').get_variable('atom_dependency'),
    dependency('gtk+-3.0')
  ]
)