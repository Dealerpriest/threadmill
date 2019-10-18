/// <reference path="./p5.d.ts" />
export default function(p5) {
  // NOTE: Set up is here
  p5.setup = () => {
    console.log(p5.sensorData);
    let canvasSlot = document.querySelector("#p5-canvas-slot");
    console.log(canvasSlot);
    while (canvasSlot.firstChild) {
      console.log("removing stuff inside canvas holder");
      canvasSlot.removeChild(canvasSlot.firstChild);
    }

    p5.createCanvas(500, 500);
  };

  // NOTE: Draw is here
  p5.draw = () => {
    p5.background(0);
    // const degree = p5.frameCount * 3;
    // const y = p5.sin(p5.radians(degree)) * 50;

    // p5.push();
    // p5.translate(0, p5.height / 2);
    // p5.ellipse(
    //   posX,
    //   y,
    //   p5.constrain(p5.sensorData.touchValues[0] / 5, 5, 350),
    //   p5.constrain(p5.sensorData.touchValues[1] / 5, 5, 350)
    // );
    // p5.pop();
    // posX += speed;

    // if (posX > p5.width || posX < 0) {
    //   speed *= -1;
    // }
  };
}
