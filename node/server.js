var ws = require("nodejs-websocket");
var WS_PORT = process.env.PORT || 8080;

var clients = [];

var wsServer = ws.createServer(function (conn) {
    console.log("New connection");

    var sensor = false;
    conn.id = "id_" + Date.now(); 
    clients.push(conn);

    conn.on("text", function (str) {

      var jsonMessage = JSON.parse(str);
      if(jsonMessage['event'] == "update") {
    		this.attitud = jsonMessage["attitud"];
        this.acceleration = jsonMessage["acceleration"];
        this.position = jsonMessage["position"];
        this.recording = jsonMessage["recording"];
    	}

      if(jsonMessage['event'] == "get"){
        var existingClients = [];
        [].forEach.call(clients, function(client) {
          if(conn != client) {
            existingClients.push({ id: client.id, attitud: client.attitud, acceleration: client.acceleration,  position: client.position, recording: client.recording });
          }
        });
        conn.sendText(JSON.stringify(existingClients));
      }
    })

    conn.on("close", function (code, reason) {
        clients.splice(clients.indexOf(this), 1);
        console.log("Connection closed");
    });
}).listen(WS_PORT);

console.log("Server started at " + WS_PORT);
