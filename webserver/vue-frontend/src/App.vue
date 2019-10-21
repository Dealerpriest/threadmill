<template>
  <div id="app">
    <div style>
      <div id="p5-canvas-slot"></div>
      <div>
      <ul>
        <li v-for="(value, index) in sensorData.touchValues" :key="index">
          touchSensor {{ index }}: {{ value }}
        </li>
      </ul>
      <ul>
        <li v-for="(value, index) in sensorData.loadCellValues" :key="index">
          loadCell {{ index }}: {{ value }}
        </li>
      </ul>
      </div>
    </div>
  </div>
</template>


<script>
// Warning about this library.
// I think it relies on the javascript engine having objects in a certain order.
// Which is actually not part of the standard and hence not guaranteed.
// https://github.com/firejune/struct.js/issues/1
require("./js/struct.js");
import p5init from "./js/p5-canvas.ts";

import p5 from "p5/lib/p5.min.js";

export default {
  name: "app",
  data: function() {
    return {
      // webSocket: undefined,
      sensorData: {
        touchValues: [0, 0, 0, 0, 0, 0],
        loadCellValues: [0, 0, 0, 0]
      },
      // Keep all fields to a size of 4 bytes (float and word32) in order to not fuck up memory alignment
      inMsg: {},
      inMsgRaw: undefined,
      // inMsgAsArrayBuffer: undefined,
      structParser: undefined
    };
  },
  methods: {
    startP5() {
      // // NOTE: Use p5 as an instance mode
      let sketch = new p5(p5init, document.querySelector("#p5-canvas-slot"));
      sketch.sensorData = this.sensorData;
    },
    initWebSocket() {
      let wsUrl;
      console.log("NODE_ENV: ", process.env.NODE_ENV);
      if (process.env.NODE_ENV == "production") {
        wsUrl = "wss://" + location.host;
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
        this.sensorData.touchValues = this.inMsg.touchValues;
        this.sensorData.loadCellValues = this.inMsg.loadCellValues;
        // console.log("touchValues:", this.inMsg.touchValues);
      });
    }
  },
  mounted: function() {
    console.log("App mounted");
    this.structParser = new window.Struct({
      startChar: ["uint8", "####", 4],
      touchValues: ["int32", [-1, -1, -1, -1, -1, -1]],
      loadCellValues: ["int32", [-1, -1, -1, -1]],
      endChar: ["uint8", "!!!!", 4]
    });
    this.initWebSocket();
    this.startP5();
  },
  components: {}
};
</script>

<style lang="scss">
#app {
  font-family: "Avenir", Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  // color: #2c3e50;
  // margin-top: 60px;
}
</style>
