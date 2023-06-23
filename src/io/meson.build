io = static_library(
  'io',
  'FileDescriptor.cxx',
  include_directories: inc,
  dependencies: [
    fmt_dep,
  ],
)

io_dep = declare_dependency(
  link_with: io,
  dependencies: [
    fmt_dep,
    util_dep,
  ],
)
