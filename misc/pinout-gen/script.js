var connectorYaml = `
###DATA###
`;

function addRow(table, info) {
  var template = document.getElementById("table-template");
  var clone = template.content.cloneNode(true);
  var pdata = clone.querySelector(".pin-data");
  var idata = clone.querySelector(".id-data");
  var tdata = clone.querySelector(".type-data");
  var fdata = clone.querySelector(".function-data");
  var cdata = clone.querySelector(".color-data");
  pdata.textContent = info.pin;
  idata.textContent = info.id;
  tdata.textContent = info.type
  fdata.textContent = info.function;
  cdata.textContent = info.color
  table.appendChild(clone);
}

function adjustMarkers() {
  var cdiv = document.getElementById("connector-div");
  cdiv.style.width = document.getElementById("connector-img").clientWidth;
  var pins = document.querySelectorAll(".pin-marker");
  for (var i = 0; i < pins.length; i++) {
    var height = cdiv.clientHeight * 0.05;
    pins[i].style.height = height + "px";
    pins[i].style.width = height + "px";
    pins[i].style.marginTop = "-" + (height * 0.5) + "px";
    pins[i].style.marginLeft = "-" + (height * 0.5) + "px";
  }
}

window.addEventListener('load', function() {
  var connector = YAML.parse(connectorYaml);
  document.getElementById("connector-img").addEventListener('load', function() {
    adjustMarkers();
  });
  document.getElementById("connector-img").src = connector.info.image.file;

  for (var i = 0; i < connector.pins.length; i++) {
    var pin = connector.pins[i];
    var pinfo;
    for (var ii = 0; ii < connector.info.pins.length; ii++) {
      if (connector.info.pins[ii].pin == pin.pin) {
        pinfo = connector.info.pins[ii];
        break;
      }
    }
    var cdiv = document.getElementById("connector-div");
    var template = document.getElementById("pin-template");
    var clone = template.content.cloneNode(true);
    var pdiv = clone.querySelector("div");
    var img = document.getElementById("connector-img");
    var imgHeight = img.naturalHeight;
    var imgWidth = img.naturalWidth;
    pdiv.style.top = ((pinfo.y / imgHeight) * 100) + "%";
    pdiv.style.left = ((pinfo.x / imgWidth) * 100) + "%";
    pdiv.dataset.type = pin.type;
    pdiv.addEventListener("click", function(pin, div) {
      var table = document.getElementById("info-table").querySelector("tbody");
      table.innerHTML = "";
      if (Array.isArray(pin.id)) {
        var pinIds = pin.id.filter((value, index) => {
          return pin.id.indexOf(value) === index;
        });
        for (var i = 0; i < pinIds.length; i++) {
          addRow(table, {pin: pin.pin, id: pinIds[i], function: pin.function});
        }
      } else {
        addRow(table, pin);
      }
      var pins = document.querySelectorAll(".pin-marker");
      for (var i = 0; i < pins.length; i++) {
        if (pins[i].dataset.type == pin.type) {
          pins[i].classList.add("highlight");
        } else {
          pins[i].classList.remove("highlight");
        }
        pins[i].classList.remove("selected");
      }
      div.classList.add("selected");
    }.bind(null, pin, pdiv));
    cdiv.appendChild(pdiv);
  }

  for (var i = 0; i < connector.pins.length; i++) {
    var table = document.getElementById("pinout-table").querySelector("tbody");
    addRow(table, connector.pins[i]);
  }
});

window.addEventListener('resize', function() {
  adjustMarkers();
});
