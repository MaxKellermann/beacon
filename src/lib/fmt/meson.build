# using include_type:system to work around -Wfloat-equal
libfmt = dependency('fmt', version: '>= 7',
                    include_type: 'system',
                    fallback: ['fmt', 'fmt_dep'])

fmt = static_library(
  'fmt',
  'RuntimeError.cxx',
  include_directories: inc,
  dependencies: libfmt,
)

fmt_dep = declare_dependency(
  link_with: fmt,
  dependencies: libfmt,
)
