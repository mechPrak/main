float cn;
float max;

void setup() {
  size(750, 500);
  background(25);
  
  max = 200;
  cn = 1000;
  for (int n = 1; n < 255; n++) {
    println(cn);
    stroke(n, 100, 100);
    line(cn, 10, cn, 100);
    if (cn > max) {
      cn = cn - (2*cn / (4 * n + 1));
    }
  }
}

void draw() {
}