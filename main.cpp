#include "mbed.h"
#include "bbcar.h"

BufferedSerial pc(USBTX, USBRX);

Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D10), pin6(D11);
BusInOut qti(D4, D5, D6, D7);
DigitalIn encoder(D13);
BBCar car(pin5, pin6, servo_ticker);

int _pattern;
volatile int steps;
volatile int last;
bool _end = 0;

void drive() {
    qti.output();
    qti = 0b1111;
    wait_us(230);
    qti.input();
    wait_us(230);
    _pattern = qti.read();

    if(qti == 0b1111) {
        car.stop();
        printf("distance: %f\n",steps*6.5*3.14/32);
        ThisThread::sleep_for(1s);
        steps = 0;
        car.goStraight(90);
        ThisThread::sleep_for(500ms);
    }
    
    else if(_pattern == 0b0001) {car.turn(90, -0.4); ThisThread::sleep_for(60ms);}    // printf("sharp left\n");
    else if(_pattern == 0b0011) {car.turn(80, -0.4); ThisThread::sleep_for(60ms);}    // printf("medium left\n");
    else if(_pattern == 0b0010) {car.turn(50, -0.4); ThisThread::sleep_for(60ms);}    // printf("gentle left\n");
    else if(_pattern == 0b0110) {car.goStraight(90); ThisThread::sleep_for(60ms);}  // printf("straight\n");
    else if(_pattern == 0b0100) {car.turn(50, 0.4); ThisThread::sleep_for(60ms);}   // printf("gentle right\n");
    else if(_pattern == 0b1100) {car.turn(80, 0.4); ThisThread::sleep_for(60ms);}   // printf("medium right\n");
    else if(_pattern == 0b1000) {car.turn(90, 0.4); ThisThread::sleep_for(60ms);}  // printf("sharp right\n");
    else if(_pattern == 0b1111) {
        car.stop();
        printf("encoder = %d\r\n", steps);
        ThisThread::sleep_for(60ms);
        car.goStraight(90);
        ThisThread::sleep_for(100ms);
        } 
    else if(_pattern == 0b0000) {car.stop(); ThisThread::sleep_for(60ms); _end = 1;} 
    else                        {car.goStraight(90); ThisThread::sleep_for(60ms);}   // printf("straight\n");

}

void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
}

int main(){
    encoder_ticker.attach(&encoder_control, 1ms);
    while(!_end){
        drive();
    }
}

