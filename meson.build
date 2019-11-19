project('beacon', 'cpp', version: '0.1',
  meson_version: '>= 0.47',
  default_options: [
    'cpp_std=c++17'
  ],
)

compiler = meson.get_compiler('cpp')

common_flags = [
  '-D_GNU_SOURCE',

  # no header bloat for the iostream library we don't use
  #'-DBOOST_NO_IOSTREAM',

  # avoid the runtime dependency on libboost_system
  '-DBOOST_ERROR_CODE_HEADER_ONLY',

  # disable deprecated boost::system features
  '-DBOOST_SYSTEM_NO_DEPRECATED',

  '-Wall',
  '-Wextra',
  '-Wwrite-strings', '-Wcast-qual', '-Wcast-align', '-Wfloat-equal',
  '-Wshadow', '-Wpointer-arith', '-Wsign-compare',
  '-Wmissing-declarations', '-Wmissing-noreturn', '-Wmissing-format-attribute',
  '-Wredundant-decls', '-Wno-long-long', '-Wundef',
  '-Wunused',
]

test_common_flags = [
  '-fvisibility=hidden',
]

if get_option('buildtype') != 'debug'
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

test_cxxflags = test_common_flags + [
  '-fno-threadsafe-statics',
  '-fmerge-all-constants',

  '-Wno-non-virtual-dtor',
]

add_global_arguments(common_flags, language: 'cpp')
add_global_arguments(compiler.get_supported_arguments(test_cxxflags), language: 'cpp')

conf = configuration_data()

thread_dep = dependency('threads')
boost_dep = dependency('boost', version: '>= 1.67')

libsystemd = dependency('libsystemd', required: false)
conf.set('HAVE_LIBSYSTEMD', libsystemd.found())

libfcgi = compiler.find_library('fcgi')

configure_file(output: 'config.h', configuration: conf)

inc = include_directories(
  'src',

  # for the generated config.h
  '.',
)

subdir('src/pg')

executable('beacon-receiver',
  'src/receiver/Main.cxx',
  'src/receiver/Receiver.cxx',
  'src/receiver/Assemble.cxx',
  'src/util/CRC.cxx',
  'src/util/PrintException.cxx',
  include_directories: inc,
  dependencies: [
    thread_dep,
    boost_dep,
    pg_dep,
    libsystemd,
  ],
  install: true,
)

executable('beacon-gpx',
  'src/web/GetGPX.cxx',
  'src/util/PrintException.cxx',
  'src/util/UriQueryParser.cxx',
  include_directories: inc,
  dependencies: [
    libfcgi,
    pg_dep,
    libsystemd,
  ],
  install: true,
)

subdir('test')

if libsystemd.found()
  subdir('systemd')
endif
