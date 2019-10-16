<template>
  <div id="app"></div>
</template>


<script>
// const Struct = require("struct");

// Warning about this library.
// I think it relies on the javascript engine having objects in a certain order.
// Which is actually not part of the standard and hence not guaranteed.
// https://github.com/firejune/struct.js/issues/1
require("./js/struct.js");
// import "./js/struct.js";

// let struct = new window.Struct({
//   sig: "uint8",
//   mimeType: "uint8",
//   id: "uint16",
//   byteLength: "uint32"
// });

export default {
  name: "app",
  data: function() {
    return {
      // webSocket: undefined,
      inMsg: {},
      // Keep all fields to a size of 4 bytes (float and word32) in order to not fuck up memory alignment
      inMsgRaw: undefined,
      inMsgAsArrayBuffer: undefined,
      structParser: undefined
    };
  },
  methods: {
    initWebSocket() {
      let wsUrl;
      if (process.env.MODE_ENV == "production") {
        wsUrl = "ws://" + location.host;
      } else {
        wsUrl = process.env.VUE_APP_WS_SERVER_URL;
      }
      const webSocket = new WebSocket(wsUrl);

      webSocket.addEventListener("open", function(event) {
        console.log("socket opened", event);
        webSocket.send("socket open");
      });

      webSocket.onmessage = event => {
        this.inMsgRaw = event.data;
      };

      webSocket.onclose = event => {
        console.log("socket closed", event);
        this.initWebSocket();
      };
      webSocket.onerror = event => {
        console.log("socket error", event);
      };

      console.log("socket state: ", webSocket.readyState);
    }
  },
  watch: {
    inMsgRaw: function(newMsg) {
      if (!(newMsg instanceof Blob)) {
        console.log("no binary message: ", newMsg);
        return;
      }
      newMsg.arrayBuffer().then(buffer => {
        this.inMsg = this.structParser.read(buffer);
        console.log("touchValues:", this.inMsg.touchValues);
      });
    }
  },
  mounted: function() {
    console.log("App mounted");
    this.initWebSocket();
    this.structParser = new window.Struct({
      startChar: ["uint8", "####", 4],
      touchValues: ["int32", [-1, -1, -1, -1]],
      loadCellValues: ["int32", [-1, -1, -1, -1]],
      endChar: ["uint8", "!!!!", 4]
    });
  },
  components: {}
};
</script>

<style lang="scss">
#app {
  font-family: "Avenir", Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
}
</style>
