// interface p5 {
//   sensorData: any;
// }
import p5, { Color } from "p5";
interface p5custom extends p5 {
  sensorData: any;
}

export default function(p5: p5custom) {
  // NOTE: Set up is here
  p5.setup = () => {
    console.log(p5.sensorData);
    let canvasSlot = document.querySelector("#p5-canvas-slot");
    console.log(canvasSlot);
    while (canvasSlot && canvasSlot.firstChild) {
      console.log("removing stuff inside canvas holder");
      canvasSlot.removeChild(canvasSlot.firstChild);
    }

    p5.createCanvas(p5.windowHeight / 1.8, p5.windowHeight - 50);
  };

  // NOTE: Draw is here
  p5.draw = () => {
    p5.background(50);

    p5.fill(getDynamicColor(p5.sensorData.touchValues[0]));
    p5.rect(xToPixel(0.0), yToPixel(0.5), xToPixel(0.1), yToPixel(0.5));

    p5.fill(getDynamicColor(p5.sensorData.touchValues[1]));
    p5.beginShape();
    p5.vertex(0, 0);
    p5.vertex(xToPixel(0.1), xToPixel(0.1));
    p5.vertex(xToPixel(0.1), yToPixel(0.5));
    p5.vertex(0, yToPixel(0.5));
    p5.endShape(p5.CLOSE);

    p5.fill(getDynamicColor(p5.sensorData.touchValues[2]));
    p5.beginShape();
    p5.vertex(0, 0);
    p5.vertex(xToPixel(0.1), xToPixel(0.1));
    p5.vertex(xToPixel(0.5), xToPixel(0.1));
    p5.vertex(xToPixel(0.5), 0);
    p5.endShape(p5.CLOSE);
    // p5.rect(xToPixel(0.0), yToPixel(0.33333), xToPixel(0.1), yToPixel(0.33333));

    p5.translate(p5.width / 2, p5.height / 2);
    p5.scale(-1, 1);
    p5.translate(-(p5.width / 2), -(p5.height / 2));

    p5.fill(getDynamicColor(p5.sensorData.touchValues[3]));
    p5.rect(xToPixel(0.0), yToPixel(0.5), xToPixel(0.1), yToPixel(0.5));

    p5.fill(getDynamicColor(p5.sensorData.touchValues[4]));
    p5.beginShape();
    p5.vertex(0, 0);
    p5.vertex(xToPixel(0.1), xToPixel(0.1));
    p5.vertex(xToPixel(0.1), yToPixel(0.5));
    p5.vertex(0, yToPixel(0.5));
    p5.endShape(p5.CLOSE);

    p5.fill(getDynamicColor(p5.sensorData.touchValues[5]));
    p5.beginShape();
    p5.vertex(0, 0);
    p5.vertex(xToPixel(0.1), xToPixel(0.1));
    p5.vertex(xToPixel(0.5), xToPixel(0.1));
    p5.vertex(xToPixel(0.5), 0);
    p5.endShape(p5.CLOSE);

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

  p5.windowResized = () => {
    p5.resizeCanvas(p5.windowHeight / 1.8, p5.windowHeight - 50);
  };

  function getDynamicColor(value: number) {
    const r = 0,
      b = 130,
      g = 20;
    return value > 200 ? p5.color(r, b, g, 255) : p5.color(r, b, g, 100);
  }

  function xToPixel(x: number) {
    return x * p5.width;
  }

  function yToPixel(y: number) {
    return y * p5.height;
  }
}
