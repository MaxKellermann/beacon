var map = new ol.Map({
  target: 'map',
  layers: [
    new ol.layer.Tile({
      source: new ol.source.OSM()
    }),

    new ol.layer.Vector({
      source: new ol.source.Vector({
        url: '/gpx/42.gpx',
        format: new ol.format.GPX(),
      }),
    }),
  ],

  view: new ol.View({
    center: ol.proj.fromLonLat([7.13, 50.95]),
    zoom: 14
  })
});
