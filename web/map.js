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

var vectorLayer = new ol.layer.Vector({
  source: new ol.source.Vector({
    url: '/gpx/42.gpx',
    format: new ol.format.GPX(),
  }),

  style: gpx_style
})

vectorLayer.on('postrender', function(evt) {
  let currentCoordinate = vectorLayer.getSource().getFeatures()[0].getGeometry().getLastCoordinate();
  let point = new ol.geom.Point(currentCoordinate);
  let vectorContext = ol.render.getVectorContext(evt);
  vectorContext.setStyle(locationStyle);
  vectorContext.drawGeometry(point);
});

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

function onAppend(lineString) {
  // center the map on the current location
  let currentCoordinate = lineString.getLastCoordinate();
  map.getView().setCenter(currentCoordinate);
}

vectorLayer.getSource().once('addfeature', function(e) {
  onAppend(e.feature.getGeometry());
});
