void setup() {
  size(1500, 500);
  background(50);

  color c0 = color(255, 255, 255);
  color c1 = color(255, 100, 100);
  color c2 = color(100, 255, 100);
  color c3 = color(100, 100, 255);
  textSize(25);

  float line_pos = 0;
  float line_mul = 200;

  float frq;
  float acc;
  float r;

  float v_base;
  float v_target;
  float steps;
  float del_init;
  float del_target;
  float del;


  acc = 10000000;
  frq = 160000;
  r = acc / pow(frq, 2);
  
  fill(c0);
  text("acc :  " + acc, 10, 200);
  text("frq :  " + frq, 10, 225);
  text("r :  " + r, 10, 250); 

  v_base = 0;
  v_target = 20000;
  steps = (pow(v_target, 2) - pow(v_base, 2)) / (2 * acc);
  del_init = frq / pow(pow(v_base, 2) + 2 * acc, 0.5);
  del_target = frq / v_target;  

  fill(c1);
  text("v_base: " + v_base, 260, 200);
  text("v_target: " + v_target, 260, 225);
  text("steps: " + steps, 260, 250);
  text("del_init: " + del_init, 260, 275);
  text("del_target: " + del_target, 260, 300);

  stroke(c1);
  del = del_init * ( 1 + (-r) * del_init * del_init);
  for (int i = 0; i < steps; i++) {
    println(del);
    line_pos += del;
    line(line_pos/line_mul, 10, line_pos/line_mul, 150);
    del = del * ( 1 + (-r) * del * del);
  }
  println();


  stroke(c2);
  for (int i = 0; i < 50; i++) {
    line_pos += del;
    line(line_pos/line_mul, 10, line_pos/line_mul, 150);
  }
  println();

/*
  acc = 10;
  frq = 160000;
  r = acc / pow(frq, 2);

  v_base = 100;
  v_target = 1;
  steps = (pow(v_base, 2) - pow(v_target, 2)) / (2 * acc);
  del_init = frq / pow(pow(v_base, 2) + 2 * acc, 0.5);
  del_target = frq / v_target;  

  fill(c3);
  text("v_base: " + v_base, 510, 200);
  text("v_target: " + v_target, 510, 225);
  text("steps: " + steps, 510, 250);
  text("del_init: " + del_init, 510, 275);
  text("del_target: " + del_target, 510, 300);

  stroke(c3);
  //del = del_init * ( 1 + (r) * del_init * del_init);
  for (int i = 0; i < steps; i++) {
    println(del);
    line_pos += del;
    line(line_pos/line_mul, 10, line_pos/line_mul, 150);
    del = del * ( 1 + (r) * del * del);
  }
  println();
  */
}

void draw() {
}