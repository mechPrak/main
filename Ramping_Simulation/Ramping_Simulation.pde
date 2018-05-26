// base speed in steps/s
float v0 = 10;
// slew speed in steps/s
float v = 30;
// acceleration in steps/s^2
float a = 10;
// timer Frequenzy in Hz
float F = 1000;

// acceleration/decelaration distance
float S = (pow(v, 2) - pow(v0, 2)) / (2 * a);

// delay fpr initial steps
float p1 = F / pow(pow(v0, 2) + 2 * a, 0.5);

// delay preiod for slew speed steps
float ps = F / v;

// constant multiplier
float R = a / pow(F, 2);

void setup() {
  size(750, 500);
  background(25);
  println(S);
  println(p1);
  println(ps);
  println(R);
  println();
  
  float p = p1 * ( 1 + (-R) * p1 * p1);
  for(int i = 0; i < S; i++){
    println(p);
    p = p * ( 1 + (-R) * p * p);
  }
}

void draw() {
  
}