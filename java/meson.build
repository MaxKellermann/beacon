if not add_languages('java', required: get_option('javaclient'))
  subdir_done()
endif

jar(
  'BeaconClient',
  'beacon/BeaconClient.java',
  'beacon/BigEndian.java',
  'beacon/CRC16CCITT.java',
  'beacon/Protocol.java',
)
