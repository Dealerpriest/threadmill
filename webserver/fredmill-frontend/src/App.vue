<template>
  <div id="app"></div>
</template>


<script>
// Warning about this library.
// I think it relies on the javascript engine having objects in a certain order.
// Which is actually not part of the standard and hence not guaranteed.
// https://github.com/firejune/struct.js/issues/1
require("./js/struct.js");

import p5 from "p5/lib/p5.min.js";

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
    runP5() {
      const script = p5 => {
        var speed = 2;
        var posX = 0;

        // NOTE: Set up is here
        p5.setup = () => {
          p5.createCanvas(500, 500);
          p5.ellipse(p5.width / 2, p5.height / 2, 500, 500);
        };

        // NOTE: Draw is here
        p5.draw = () => {
          p5.background(0);
          const degree = p5.frameCount * 3;
          const y = p5.sin(p5.radians(degree)) * 50;

          p5.push();
          p5.translate(0, p5.height / 2);
          p5.ellipse(
            posX,
            y,
            p5.constrain(this.inMsg.touchValues[0] / 5, 5, 350),
            p5.constrain(this.inMsg.touchValues[1] / 5, 5, 350)
          );
          p5.pop();
          posX += speed;

          if (posX > p5.width || posX < 0) {
            speed *= -1;
          }
        };
      };
      // // NOTE: Use p5 as an instance mode
      // const P5 = require("p5");
      new p5(script);
    },
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

    this.runP5();
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
