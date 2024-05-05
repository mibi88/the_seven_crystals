the_seven_crystals_sources = [
  'main.c',
  'gfx.c',
  'entity.c',
  'bullet.c',
  'boss.c',
  'mapgen.c',
  'tilemap.c',
  'player.c',
  'mobs.c',
  'data.c',
  'data.c',
]

the_seven_crystals_assets = [
  # Images
  'img/font.png',
  'img/tileset1.png',
  'img/tileset2.png',
  'img/tileset3.png',
  'img/tileset4.png',
  'img/tileset5.png',
  'img/tileset6.png',
  'img/tileset7.png',
  'img/tileset8.png',
  'img/player.png',
  'img/slug.png',
  'img/zombie.png',
  'img/cannon.png',
  'img/boss1.png',
  'img/boss2.png',
  'img/boss3.png',
  'img/boss4.png',
  'img/boss5.png',
  'img/boss6.png',
  'img/boss7.png',
  'img/megaboss.png',
  'img/bullet.png',
  # Audio
  'wav/step.wav',
  'wav/attack.wav',
  'wav/grab.wav',
  'wav/hurts.wav',
  'wav/place.wav',
]

the_seven_crystals_incdirs = include_directories('../inc')

if ['emcc', 'emscripten'].contains(meson.get_compiler('c').get_id())
  extra_args = [
    '-sUSE_SDL=1',
    '-sUSE_SDL_IMAGE=1',
    '-sLEGACY_GL_EMULATION',
    '-sGL_FFP_ONLY',
    '-sGL_UNSAFE_OPTS=0',
    '--use-preload-plugins',
  ]
  foreach asset : the_seven_crystals_assets
    extra_args += '--preload-file'
    extra_args += '@0@@@1@'.format(meson.project_source_root() / asset,
      asset.split('/')[-1])
  endforeach
else
  extra_args = []
endif
foreach asset : the_seven_crystals_assets
  configure_file(input: meson.project_source_root() / asset,
    output: asset.split('/')[-1],
    copy: true,
    install_dir: meson.project_build_root(),
    install: true,
  )
endforeach

if target_machine.system() != 'wasm'
  the_seven_crystals_deps = [
    meson.get_compiler('c').find_library('m', required: true),
    meson.get_compiler('c').find_library('SDL', required: true),
    meson.get_compiler('c').find_library('SDL_image', required: true),
    meson.get_compiler('c').find_library('SDL_mixer', required: true),
    meson.get_compiler('c').find_library('GL', required: true),
    meson.get_compiler('c').find_library('GLU', required: true),
  ]
else
  the_seven_crystals_deps = []
endif

if ['emcc', 'emscripten'].contains(meson.get_compiler('c').get_id())
  executable('index', the_seven_crystals_sources,
    include_directories: the_seven_crystals_incdirs,
    override_options: ['b_lundef=false'],
    link_args: extra_args,
    dependencies: the_seven_crystals_deps,
    name_suffix: 'html',
  )
else
  message('Normal build')
  executable('main', the_seven_crystals_sources,
    include_directories: the_seven_crystals_incdirs,
    link_args: extra_args,
    dependencies: the_seven_crystals_deps,
  )
endif

