if not add_languages('java', required: get_option('javaclient'))
  subdir_done()
endif

jar(
  'BeaconClient',
  'beacon/BeaconClient.java',
  'beacon/BigEndian.java',
  'beacon/CRC16CCITT.java',
  'beacon/Protocol.java',

  java_args: [
    # suppress warnings for missing JavaDoc on private/package members
    '-Xdoclint:-missing/private,-missing/package',
  ],

  install: true,
  install_dir: get_option('datadir') / 'java',
)
