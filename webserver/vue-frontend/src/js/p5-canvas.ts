import p5 from "p5";
interface p5custom extends p5 {
  sensorData: any;
}

export default function(p5: p5custom) {
  let canvas: any;
  // NOTE: Set up is here
  p5.setup = () => {
    console.log(p5.sensorData);
    let canvasSlot = document.querySelector("#p5-canvas-slot");
    console.log(canvasSlot);
    while (canvasSlot && canvasSlot.firstChild) {
      console.log("removing stuff inside canvas holder");
      canvasSlot.removeChild(canvasSlot.firstChild);
    }

    canvas = p5.createCanvas(p5.windowHeight / 1.8, p5.windowHeight - 50);
  };

  // NOTE: Draw is here
  p5.draw = () => {
    p5.background(40, 150, 70);

    p5.push();
    let gravityPos: p5.Vector = centerOfGravity(
      p5.sensorData.loadCellValues[0],
      p5.sensorData.loadCellValues[1],
      p5.sensorData.loadCellValues[2],
      p5.sensorData.loadCellValues[3]
    );
    let totalWeight = p5.sensorData.loadCellValues.reduce(
      (a: number, b: number) => a + b,
      0
    );
    var gradX = gravityPos.x * p5.width; // - p5.width / 2;
    var gradY = gravityPos.y * p5.height; // - p5.height / 2;
    let posX = gradX;
    let posY = gradY;
    var gradient = canvas.drawingContext.createRadialGradient(
      posX,
      posY,
      p5.map(totalWeight, 400, 4000, 5, p5.width, true),
      gradX,
      gradY,
      0
    );
    gradient.addColorStop(0, "transparent");
    gradient.addColorStop(1, "white");
    canvas.drawingContext.fillStyle = gradient;
    p5.rect(0, 0, p5.width, p5.height);

    p5.pop();

    p5.push();

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

    p5.pop();

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

  // center of gravity formula:
  // X = (W1*X1 + W2*X2 + W3*X3 + W4*X4)/(W1+W2+W3+W4)
  // Y = (W1*Y1 + W2*Y2 + W3*Y3 + W4*Y4)/(W1+W2+W3+W4)
  function centerOfGravity(
    w0: number,
    w1: number,
    w2: number,
    w3: number
  ): p5.Vector {
    let centerOfGravity = p5.createVector();
    let startX = 0,
      startY = 0,
      endX = 1,
      endY = 1;
    centerOfGravity.x =
      (w0 * startX + w1 * startX + w2 * endX + w3 * endX) / (w0 + w1 + w2 + w3);
    centerOfGravity.y =
      (w0 * endY + w1 * startY + w2 * startY + w3 * endY) / (w0 + w1 + w2 + w3);
    return centerOfGravity;
  }

  function getDynamicColor(value: number) {
    const r = 60,
      g = 255,
      b = 80;
    return value > 200 ? p5.color(r, g, b, 230) : p5.color(r, g, b, 120);
  }

  function xToPixel(x: number) {
    return x * p5.width;
  }

  function yToPixel(y: number) {
    return y * p5.height;
  }
}
