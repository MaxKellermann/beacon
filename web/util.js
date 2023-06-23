// SPDX-License-Identifier: BSD-2-Clause
// Copyright Max Kellermann <max.kellermann@gmail.com>

/**
 * Send a GET request and pass the response body to the caller.
 */
function get(url, callback) {
  let client = new XMLHttpRequest();
  client.open('GET', url);
  client.onload = function() {
    callback(client.responseText);
  };
  client.send();
}
