project('beacon', 'cpp', version: '0.1',
  meson_version: '>= 1.2',
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',

    # This is only here to build subprojects as static libraries
    'default_library=static',
  ],
)

debug = get_option('b_ndebug') != 'true'

compiler = meson.get_compiler('cpp')

common_flags = [
  '-D_GNU_SOURCE',
]

test_common_flags = [
  '-Wcast-align',
  '-Wcast-qual',
  '-Wdouble-promotion',
  '-Wfloat-equal',
  '-Wmissing-declarations',
  '-Wmissing-noreturn',
  '-Wmissing-format-attribute',
  '-Wredundant-decls',
  '-Wshadow',
  '-Wundef',
  '-Wunused',
  '-Wvla',
  '-Wwrite-strings',

  # clang specific warning options:
  '-Wunreachable-code-aggressive',
  '-Wused-but-marked-unused',
]

test_global_common_flags = [
  '-fvisibility=hidden',
]

if not debug
  test_common_flags += [
    '-ffunction-sections',
    '-fdata-sections',
  ]

  add_global_link_arguments(
    compiler.get_supported_link_arguments(
      '-Wl,--gc-sections',
      '-Wl,--icf=all',
    ),
    language: 'cpp'
  )
endif

test_global_cxxflags = test_global_common_flags + [
]

test_cxxflags = test_common_flags + [
  '-fno-threadsafe-statics',
  '-fmerge-all-constants',

  '-Wcomma-subscript',
  '-Wextra-semi',
  '-Wmismatched-tags',
  '-Woverloaded-virtual',
  '-Wsign-promo',
  '-Wvolatile',
  '-Wvirtual-inheritance',

  '-Wno-missing-field-initializers',

  # a vtable without a dtor is just fine
  '-Wno-non-virtual-dtor',

  # clang specific warning options:
  '-Wcomma',
  '-Wheader-hygiene',
  '-Winconsistent-missing-destructor-override',
]

add_global_arguments(common_flags, language: 'cpp')
add_global_arguments(compiler.get_supported_arguments(test_global_cxxflags), language: 'cpp')
add_project_arguments(compiler.get_supported_arguments(test_cxxflags), language: 'cpp')

conf = configuration_data()

thread_dep = dependency('threads')

libsystemd = dependency('libsystemd', required: get_option('systemd'))
conf.set('HAVE_LIBSYSTEMD', libsystemd.found())

libfcgi = compiler.find_library('fcgi')
nlohmann_json = dependency('nlohmann_json')

configure_file(output: 'config.h', configuration: conf)

inc = include_directories(
  'src',

  # for the generated config.h
  '.',
)

subdir('src/util')
subdir('src/lib/fmt')
subdir('src/io')
subdir('src/system')
subdir('src/net')
subdir('src/event')
subdir('src/event/net')
subdir('src/pg')

executable('beacon-receiver',
  'src/receiver/Main.cxx',
  'src/receiver/Receiver.cxx',
  'src/receiver/Assemble.cxx',
  'src/receiver/Database.cxx',
  include_directories: inc,
  dependencies: [
    util_dep,
    event_dep,
    event_net_dep,
    thread_dep,
    pg_dep,
    libsystemd,
    fmt_dep,
  ],
  install: true,
)

executable('beacon-api',
  'src/api/Main.cxx',
  'src/api/Handler.cxx',
  'src/api/GetGPX.cxx',
  'src/api/Response.cxx',
  include_directories: inc,
  dependencies: [
    util_dep,
    libfcgi,
    nlohmann_json,
    pg_dep,
    libsystemd,
  ],
  install: true,
)

subdir('java')
subdir('test')

if libsystemd.found()
  subdir('systemd')
endif
