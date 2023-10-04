net = static_library(
  'net',
  'AddressInfo.cxx',
  'FormatAddress.cxx',
  'HostParser.cxx',
  'IPv4Address.cxx',
  'IPv6Address.cxx',
  'Resolver.cxx',
  'SocketAddress.cxx',
  'SocketDescriptor.cxx',
  'SocketError.cxx',
  'UniqueSocketDescriptor.cxx',
  include_directories: inc,
  dependencies: [
    fmt_dep,
    system_dep,
    io_dep,
  ],
)

net_dep = declare_dependency(
  link_with: net,
  dependencies: [
    system_dep,
    io_dep,
  ],
)
