libpq = dependency('libpq', version: '>= 11')

pg = static_library(
  'pg',
  'Serial.cxx',
  'Array.cxx',
  'Connection.cxx',
  'Result.cxx',
  'Error.cxx',
  include_directories: inc,
  dependencies: [
    libpq,
  ],
)

pg_dep = declare_dependency(
  link_with: pg,
  dependencies: [
    libpq,
  ],
)
