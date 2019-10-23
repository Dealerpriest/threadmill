export default class {
  device!: USBDevice;
  msg: string = "";
  constructor() {
    this.init().then(this.readLoop);
  }

  init = async () => {
    this.device = await navigator.usb.requestDevice({
      filters: [{ vendorId: 0x0d28 }]
    });
    await this.device.open();
    //Don't now why we call this with 1 as argument, but everyone else seems to do that...
    await this.device.selectConfiguration(1);
    //the com port is apparently the 4th interface on the uBit...
    await this.device.claimInterface(4);

    let controlResult = await this.device.controlTransferOut(
      {
        requestType: "class",
        recipient: "interface",
        request: 9,
        value: 512,
        index: 4
      },
      new Uint8Array([130, 0, 194, 1, 0])
    );
    console.log(controlResult);
    return controlResult;
    // 0: 130
    // 1: 0
    // 2: 194
    // 3: 1
    // 4: 0
  };

  readLoop = async () => {
    // console.log("readLoop entered");
    let outResult: USBOutTransferResult = await this.device.controlTransferOut(
      {
        requestType: "class",
        recipient: "interface",
        request: 9,
        value: 512,
        index: 4
      },
      new Uint8Array([131])
    );
    if (outResult.status == "ok") {
      let inResult: USBInTransferResult = await this.device.controlTransferIn(
        {
          requestType: "class",
          recipient: "interface",
          request: 0x01,
          value: 0x100,
          index: 4
        },
        64
      );
      if (inResult.status == "ok" && inResult.data != undefined) {
        // console.log("status ok on controlTransferIn & inresult.data !null");
        let byteArray: Uint8Array = new Uint8Array(inResult.data.buffer);
        var str = "";
        let len = byteArray[1];
        console.log("len is ", len);
        for (var i = 2; i < len + 2; ++i) {
          str += String.fromCharCode(byteArray[i]);
        }
        if (str.length > 0) {
          this.msg = str;
          console.log(str);

          // console.log("data:", byteArray);
          this.readLoop();
        } else {
          // console.log("nothing received");
          setTimeout(() => {
            this.readLoop();
          }, 25);
        }
      } else {
        console.log("read failed");
        setTimeout(() => {
          this.readLoop();
        }, 1000);
      }
    }
  };
}
