var gpx_style = new ol.style.Style({
  stroke: new ol.style.Stroke({
    color: 'rgba(20,50,255,0.5)',
    width: 3
  })
});

var vectorLayer = new ol.layer.Vector({
  source: new ol.source.Vector({
    url: '/gpx/42.gpx',
    format: new ol.format.GPX(),
  }),

  style: gpx_style
})

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
