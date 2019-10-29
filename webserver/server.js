const http = require("http");
const WebSocket = require("ws");
const express = require("express");
const request = require("request");
// var path = require("path");
const internalIp = require("internal-ip");

const app = express();

const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

let proxyEnabled = process.argv[2] && process.argv[2] == "proxy" ? true : false;

let port = process.env.PORT || 8800;
let serverIp = internalIp.v4.sync();
console.log(port);

console.log(serverIp);

console.log("http://" + serverIp + ":" + port + "/");

console.log("proxyEnabled: " + proxyEnabled);

wss.on("connection", function connection(ws) {
  console.log("client connected");
  ws.on("message", function incoming(message) {
    // console.log("received: %s", message);
    //broadcast data to every client connected
    wss.clients.forEach(function each(client) {
      if (client.readyState === WebSocket.OPEN) {
        client.send(message);
      }
    });
  });

  ws.on("close", () => console.log("client disconnected"));

  ws.send("something");
});

if (proxyEnabled) {
  app.use("/", function(req, res) {
    // console.log("proxy triggered");
    var apiUrl = "https://fredmill.herokuapp.com";
    // console.log(apiUrl);
    // console.log(req.url);
    var url = apiUrl + req.url;
    request(url).pipe(res);
  });
} else {
  app.use(express.static(__dirname + "/public"));
}

server.listen(port);
