// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright 2019 Max Kellermann <max.kellermann@gmail.com>,
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
