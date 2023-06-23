// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright Max Kellermann <max.kellermann@gmail.com>

function switchTrack(control) {
  let id = control.options[control.selectedIndex].value;
  if (id == '')
    id = null;

  map.removeLayer(vectorLayer);

  if (id !== null) {
    vectorLayer = loadTrackLayer(id);
    map.addLayer(vectorLayer);
  }
}

function updateTrackList(control) {
  get('/api/list', function(data) {
    list = JSON.parse(data)

    control.innerHTML = '<option value="">none</option>'
    for (let i = 0; i < list.length; ++i) {
      let e = document.createElement("option");
      e.setAttribute("value", list[i]['id'])
      e.innerText = list[i]['id'];
      control.appendChild(e);
    }
  })
}

function reloadTrackList() {
  updateTrackList(document.getElementById('track'));
}

window.onload = reloadTrackList;
