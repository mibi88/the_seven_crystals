project('the_seven_crystals', 'c',
          version: 'v.1.3',
    meson_version: '>= 0.60.0',
  default_options: [ 'warning_level=2',
                     'c_std=c89',
                   ],
)

config_h = configuration_data()
config_h.set_quoted('VERSION', meson.project_version())
configure_file(
  output: 'config.h',
  configuration: config_h,
)
add_project_arguments([
  '-I' + meson.project_build_root(),
], language: 'c')

subdir('src')
