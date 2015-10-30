//flags
int traceFlag = 0;
float scale = 3;
float link1      = 436/scale; // all in mm
float link2      = 375/scale;
float tableWidth = 600/scale; 
float tableHeight = 800/scale;
float tableXOffset = 200/scale;
float armStartPointBottom = 200/scale;

float pi = 3.14159265;

float theta1     = 30;
float theta2     = 110;

float x0;
float y0;
float x1;
float y1;
float x2;
float y2;

void setup()
{
  //
  size(333, 266);

}

void draw()
{
  background(255);
  drawTable();
  drawArm();  
}

void drawArm()
{
  strokeWeight(10);
  stroke(0);

  x0 = tableXOffset+tableWidth/2;
  y0 = 0;
  x1 = x0 + link1*cos((pi/180)*(theta1));
  y1 = y0 + link1*sin((pi/180)*(theta1));
  x2 = x1 + link2*cos((pi/180)*(theta1+theta2));
  y2 = y1 + link2*sin((pi/180)*(theta1+theta2));

  line(x0, height-y0, x1, height-y1);
  line(x1, height-y1, x2, height-y2);
  ellipseMode(CENTER);
  ellipse(x2, height-y2, 50, 50);
}


void drawTable(){
  rectMode(CORNERS);
  fill(162); // make table black
  stroke(162);
  rect(tableXOffset, 0, tableXOffset + tableWidth, tableHeight);
}

void setAngles(angle1, angle2){
  theta1 = parseFloat(angle1);
  theta2 = parseFloat(angle2);
  // console.log(theta1+theta2);
  // console.log(cos((pi/180)*(parseFloat(angle1)+parseFloat(angle2))));
}