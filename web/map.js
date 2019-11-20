var fragment_params = new URLSearchParams(window.location.hash.substr(1));
var id = fragment_params.get("id");
if (id === null)
  id = '42';

var gpx_style = new ol.style.Style({
  stroke: new ol.style.Stroke({
    color: 'rgba(20,50,255,0.5)',
    width: 3
  })
});

var locationStyle = new ol.style.Style({
  image: new ol.style.Circle({
    radius: 5,
    fill: null,
    stroke: new ol.style.Stroke({
      color: 'rgba(255,50,50,0.8)',
      width: 2
    })
  })
});

function loadTrackLayer(id) {
  var vectorLayer = new ol.layer.Vector({
    source: new ol.source.Vector({
      url: '/api/gpx/' + id + '.gpx',
      format: new ol.format.GPX(),
    }),

    style: gpx_style
  });

  vectorLayer.on('postrender', function(evt) {
    let currentCoordinate = vectorLayer.getSource().getFeatures()[0].getGeometry().getLastCoordinate();
    let point = new ol.geom.Point(currentCoordinate);
    let vectorContext = ol.render.getVectorContext(evt);
    vectorContext.setStyle(locationStyle);
    vectorContext.drawGeometry(point);
  });

  vectorLayer.getSource().once('addfeature', function(e) {
    onAppend(e.feature.getGeometry());
    scheduleUpdate();
  });

  return vectorLayer;
}

var vectorLayer = loadTrackLayer(id);

var map = new ol.Map({
  target: 'map',
  layers: [
    new ol.layer.Tile({
      source: new ol.source.OSM()
    }),

    vectorLayer,
  ],

  view: new ol.View({
    center: ol.proj.fromLonLat([7.13, 50.95]),
    zoom: 14
  })
});

/**
 * Send a GET request and pass the parsed and projected response body as a
 * "feature" object to the callback.
 */
function getFeatures(url, format, callback) {
  get(url, function(data) {
    callback(format.readFeatures(data, {featureProjection: map.getView().getProjection()}));
  });
}

function onAppend(lineString) {
  // center the map on the current location
  let currentCoordinate = lineString.getLastCoordinate();
  map.getView().setCenter(currentCoordinate);

  let date = new Date(currentCoordinate[2] * 1000);
  document.getElementById('last_time').innerText = date.toUTCString();
}

function appendCoordinates(src) {
  let feature = vectorLayer.getSource().getFeatures()[0]
  let dest = feature.getGeometry();
  let last_time = dest.getLastCoordinate()[2];

  let coordinates = dest.getCoordinates();
  let changed = false;
  src.forEach(function(c) {
    if (c[2] > last_time) {
      coordinates[0].push(c);
      changed = true;
    }
  });

  if (changed) {
    dest.setCoordinates(coordinates);
    onAppend(feature.getGeometry());
  }
}

function updateVectorLayer() {
  let feature = vectorLayer.getSource().getFeatures()[0]
  let currentCoordinate = feature.getGeometry().getLastCoordinate();
  let date = new Date(currentCoordinate[2] * 1000);
  let url = vectorLayer.getSource().getUrl() + "?since=" + date.toJSON();

  getFeatures(url, vectorLayer.getSource().getFormat(), function(newFeatures) {
    appendCoordinates(newFeatures[0].getGeometry().getLineString().getCoordinates())
    document.getElementById('last_update').innerText = new Date().toUTCString();
    scheduleUpdate();
  });
}

function scheduleUpdate() {
  setTimeout(updateVectorLayer, 5000);
}
