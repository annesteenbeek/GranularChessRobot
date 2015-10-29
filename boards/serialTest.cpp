#include "mbed.h"
#include <string>
#include "MODSERIAL.h"
#define PI 3.1459165

MODSERIAL pc(USBTX, USBRX);
const int baudrate = 9600;
char recieved;
bool onStatus = true;

int i = 0;
int j = 0;


Ticker dataSend;
DigitalOut ledRed(LED_RED);
DigitalOut ledGreen(LED_GREEN);
DigitalOut ledBlue(LED_BLUE);

void sendData(){
        i = (i+1) % 70;
        j = (j+2) % 50;
        float theta1 = 20+i;
        float theta2 = 10+j;

        pc.printf("theta1 %.2lf\n", theta1);
        pc.printf("theta2 %.2lf\n", theta2);
        pc.printf("DOF %s\n", "X");

}

int main()
{
    dataSend.attach(&sendData, 0.1f);
    ledRed.write(1);
    ledBlue.write(1);
    ledGreen.write(1);
    pc.baud(baudrate);
    char buf[128];
while(true){
    pc.scanf("%s", buf);
    if(strcmp(buf,"ledRed")==0){
        ledRed.write(!ledRed.read());
    }
    if(strcmp(buf,"ledBlue")==0){
        ledBlue.write(!ledBlue.read());
    }
    if(strcmp(buf,"ledGreen")==0){
        ledGreen.write(!ledGreen.read());
    }
    pc.printf("recv: %s\n", buf);
}
}