<template>
  <div id="app">
    <div style>
      <div id="p5-canvas-slot"></div>
      <!-- <button @click="pairUbit">Connect uBit</button>
      <p>serial: {{ uBitSerial ? uBitSerial.msg : "" }}</p> -->
      <div v-if="showDebugControls">
        <button @click="showDebugControls = false">X</button>
        <button @click="setReferenceValues">set calibration offsets</button>
        <table>
          <tr>
            <th>
              Raw values
            </th>
            <th>
              Calibrated values
            </th>
          </tr>
          <tr>
            <td>
              <ul>
                <li
                  v-for="(value, index) in sensorData.touchValues"
                  :key="index"
                >
                  touchSensor {{ index }}: {{ value }}
                </li>
              </ul>
              <ul>
                <li
                  v-for="(value, index) in sensorData.loadCellValues"
                  :key="index"
                >
                  loadCell {{ index }}: {{ value }}
                </li>
              </ul>
            </td>
            <td>
              <ul>
                <li
                  v-for="(value, index) in calibratedSensorData.touchValues"
                  :key="index"
                >
                  touchSensor {{ index }}: {{ value }}
                </li>
              </ul>
              <ul>
                <li
                  v-for="(value, index) in calibratedSensorData.loadCellValues"
                  :key="index"
                >
                  loadCell {{ index }}: {{ value }}
                </li>
              </ul>
            </td>
          </tr>
        </table>
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

// Keep all fields to a size of 4 bytes in order to not fuck up memory alignment
let structParser = new window.Struct({
  startChar: ["uint8", "####", 4],
  touchValues: ["int32", [-1, -1, -1, -1, -1, -1]],
  loadCellValues: ["int32", [-1, -1, -1, -1]],
  endChar: ["uint8", "!!!!", 4]
});

import p5init from "./js/p5-canvas.ts";

import WebSerial from "./js/uBitSerial.ts";

import p5 from "p5/lib/p5.min.js";

// // NOTE: Use p5 as an instance mode
let sketch = new p5(p5init, document.querySelector("#p5-canvas-slot"));
sketch.sensorData = undefined;

// import NoSleep from "nosleep.js";
// let noSleep = new NoSleep();

export default {
  name: "app",
  data: function() {
    return {
      debugTriggerCounter: 0,
      showDebugControls: process.env.NODE_ENV == "development" ? true : false,
      debugTriggerTimeoutId: undefined,
      // uBitSerial: undefined,
      // webSocket: undefined,
      sensorData: {
        touchValues: [0, 0, 0, 0, 0, 0],
        loadCellValues: [0, 0, 0, 0]
      },
      referenceSensorData: {
        touchValues: [0, 0, 0, 0, 0, 0],
        loadCellValues: [0, 0, 0, 0]
      },
      calibratedSensorData: {
        touchValues: [0, 0, 0, 0, 0, 0],
        loadCellValues: [0, 0, 0, 0]
      },
      inMsg: {},
      inMsgRaw: undefined
      // inMsgAsArrayBuffer: undefined,
      // structParser: undefined
    };
  },
  computed: {
    // calibratedSensorData() {
    //   let calibratedSensorData = { touchValues: [], loadCellValues: [] };
    //   for (let i = 0; i < this.sensorData.touchValues.length; i++) {
    //     calibratedSensorData.touchValues[i] =
    //       this.sensorData.touchValues[i] -
    //       this.referenceSensorData.touchValues[i];
    //   }
    //   for (let i = 0; i < this.sensorData.loadCellValues.length; i++) {
    //     calibratedSensorData.loadCellValues[i] =
    //       this.sensorData.loadCellValues[i] -
    //       this.referenceSensorData.loadCellValues[i];
    //   }
    //   return calibratedSensorData;
    // }
  },
  methods: {
    setReferenceValues() {
      for (let i = 0; i < this.sensorData.touchValues.length; i++) {
        this.referenceSensorData.touchValues[i] = this.sensorData.touchValues[
          i
        ];
      }
      for (let i = 0; i < this.sensorData.loadCellValues.length; i++) {
        this.referenceSensorData.loadCellValues[
          i
        ] = this.sensorData.loadCellValues[i];
      }
    },
    debugTrigger() {
      console.log("debug triggered");
      this.debugTriggerCounter++;
      if (this.debugTriggerCounter >= 7) {
        this.showDebugControls = true;
      }
      if (this.debugTriggerTimeoutId) {
        clearTimeout(this.debugTriggerTimeoutId);
      }
      this.debugTriggerTimeoutId = setTimeout(() => {
        this.debugTriggerCounter = 0;
        console.log(this.debugTriggerCounter);
      }, 350);
    },
    // pairUbit() {
    //   this.uBitSerial = new WebSerial();
    // },
    attachSensorDataToP5() {
      sketch.sensorData = this.calibratedSensorData;
    },
    initWebSocket() {
      let wsUrl;
      console.log("NODE_ENV: ", process.env.NODE_ENV);
      if (process.env.NODE_ENV == "production") {
        let protocol = location.protocol == "https:"?"wss:":"ws:"
        wsUrl = protocol + "//" + location.host;
      } else {
        wsUrl = process.env.VUE_APP_WS_SERVER_URL;
      }

      const webSocket = new WebSocket(wsUrl);
      webSocket.addEventListener("open", event => {
        console.log("socket opened", event);
        webSocket.send("socket open");
        setTimeout(() => {
          this.setReferenceValues();
        }, 2500);
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
        this.inMsg = structParser.read(buffer);
        this.sensorData.touchValues = this.inMsg.touchValues;
        this.sensorData.loadCellValues = this.inMsg.loadCellValues;

        for (let i = 0; i < this.inMsg.touchValues.length; i++) {
          this.calibratedSensorData.touchValues[i] =
            this.inMsg.touchValues[i] - this.referenceSensorData.touchValues[i];
        }
        for (let i = 0; i < this.inMsg.loadCellValues.length; i++) {
          this.calibratedSensorData.loadCellValues[i] =
            this.inMsg.loadCellValues[i] -
            this.referenceSensorData.loadCellValues[i];
        }
        // console.log("touchValues:", this.inMsg.touchValues);
      });
    }
  },
  mounted: function() {
    console.log("App mounted");

    document.addEventListener("touchend", () => this.debugTrigger());
    document.addEventListener("click", () => this.debugTrigger());

    // document.addEventListener(
    //   "touchstart",
    //   function enableNoSleep() {
    //     console.log("enabling NoSleep library");
    //     noSleep.enable();
    //     document.body.style.backgroundColor = "green";
    //     document.removeEventListener("touchstart", enableNoSleep);
    //   },
    //   false
    // );
    // this.structParser = new window.Struct({
    //   startChar: ["uint8", "####", 4],
    //   touchValues: ["int32", [-1, -1, -1, -1, -1, -1]],
    //   loadCellValues: ["int32", [-1, -1, -1, -1]],
    //   endChar: ["uint8", "!!!!", 4]
    // });
    this.initWebSocket();
    this.attachSensorDataToP5();
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
