event_net = static_library(
  'event_net',
  'UdpListener.cxx',
  include_directories: inc,
  dependencies: [
    event_dep,
    net_dep,
    util_dep,
  ],
)

event_net_dep = declare_dependency(
  link_with: event_net,
  dependencies: [
    event_dep,
    net_dep,
    util_dep,
  ],
)
