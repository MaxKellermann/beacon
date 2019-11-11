systemd_unit_conf = configuration_data()
systemd_unit_conf.set('prefix', get_option('prefix'))

configure_file(
  input: 'beacon-receiver.service.in',
  output: 'beacon-receiver.service',
  configuration: systemd_unit_conf,
  install_dir: 'lib/systemd/system',
)
