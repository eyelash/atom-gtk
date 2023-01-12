project('atom-gtk', 'vala', 'c', 'cpp')

executable(
  meson.project_name(),
  'src/application.vala',
  'src/window.vala',
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
    source_dir: 'data'
  ),
  include_directories: include_directories(
    'src'
  ),
  dependencies: [
    subproject('atom-native').get_variable('atom_dependency'),
    dependency('gtk+-3.0')
  ]
)
