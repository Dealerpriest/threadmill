<template>
  <div id="app"></div>
</template>

<script>
export default {
  name: "app",
  mounted: () => {
    console.log("App mounted");

    let wsUrl;
    if (process.env.MODE_ENV == "production") {
      wsUrl = "ws://" + location.host;
    } else {
      wsUrl = process.env.VUE_APP_WS_SERVER_URL;
    }
    const socket = new WebSocket(wsUrl);

    socket.addEventListener("open", function(event) {
      console.log("socket opened", event);
      socket.send("socket open");
    });

    socket.addEventListener("message", function(event) {
      console.log("Message from server ", event.data);
    });

    socket.onclose = event => console.log("socket closed", event);
    socket.onerror = event => console.log("socket error", event);

    console.log("socket state: ", socket.readyState);
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
