//flags
int traceFlag = 0;

float link1      = 300;
float link2      = 200;
float link3      = 100;
float theta1     = 45;
float theta2     = 30;
float theta3     = 0;

void setup()
{
  size(1280, 720);
  strokeWeight(10);
  stroke(255, 160);
}

void draw()
{
  background(0);
  drawArm();  
}

void drawLink(float x_m, float y_m, float segLength, float angle_deg){
  float x = x_m;
  float y = height - y_m;
  float angle = radians(angle_deg);
  line(x, y, x + segLength*cos(angle),
        y - segLength*sin(angle));
}

void drawArm()
{
  drawLink(0, 0, link1, theta1);
  drawLink(link1*cos(radians(theta1)), 
            link1*sin(radians(theta1)), 
            link2, theta1 - theta2);
  ellipseMode(CENTER);
  ellipse(link1*cos(radians(theta1))+link2*cos(radians(theta1-theta2)),height- (link1*sin(radians(theta1))+link2*sin(radians(theta1-theta2))), 50, 50);
}