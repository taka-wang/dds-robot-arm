#include <VarSpeedServo.h> 

typedef struct {
    int pin;
    int min;
    int max;
    int pos;
    int current;
} Tconf;

// declaration
String str;
VarSpeedServo myServo[4];
Tconf conf[4] = {
    {  0,   0,   0,    0, 0 },
    { 10,  50, 120,  500, 0 }, // large arm
    { 11,  45, 100,  300, 0 }, // small arm
    {  9,   5, 155,  500, 0 }, // bottom
};


int delta = 15;     // left, right
int delta2 = 30;    // up, down

int mymap(int val, int mmin, int mmax) {
    return (val / 1000.0) * (mmax - mmin);
}

// move left
void left(int speed, boolean wait) {
    if (conf[3].current > conf[3].min) {
        int v = mymap(delta, conf[3].min, conf[3].max);
        conf[3].current = max(conf[3].min, conf[3].current - v);
        myServo[3].write(conf[3].current, speed, wait);
    } 
}

// move right
void right(int speed, boolean wait) {
    if (conf[3].current < conf[3].max) {
        int v = mymap(delta, conf[3].min, conf[3].max);
        conf[3].current = min(conf[3].max, conf[3].current + v);
        myServo[3].write(conf[3].current, speed, wait);
    } 
}

// move up
void up(int speed, boolean wait) {
    if (conf[1].current < conf[1].max) {
        int v = mymap(delta2, conf[1].min, conf[1].max);
        conf[1].current = min(conf[1].max, conf[1].current + v);
        myServo[1].write(conf[1].current, speed, wait);
    } 
}

// move down
void down(int speed, boolean wait) {
    if (conf[1].current > conf[1].min) {
        int v = mymap(delta2, conf[1].min, conf[1].max);
        conf[1].current = max(conf[1].min, conf[1].current - v);
        myServo[1].write(conf[1].current, speed, wait);
    } 
}

// move
void move(int servo, int val, int speed, boolean wait) {
    val = map(val, 0, 1000, conf[servo].min, conf[servo].max);
    conf[servo].current = val;
    myServo[servo].write(val, speed, wait);
}

// init servo
void initServo() {
    for (int i = 1; i < 4; i++) {
        myServo[i].attach(conf[i].pin);
        delay(100);
        move(i, conf[i].pos, 255, false);
    }
}

// reset to init position
void reset(int speed, boolean wait) {
    for (int i = 1; i < 4; i++) {
        delay(100);
        move(i, conf[i].pos, speed, wait);
    }
}


void setup() {
    Serial.begin(115200);
    initServo();
}

void loop() {
    if (Serial.available() > 0) {
        str = Serial.readStringUntil('\n');
        int angel = str.substring(0,1).toInt();
        switch (angel) {
            case 1: // left
                left(50, false);
                break;
            case 2: // right
                //
                right(50, false);
                break;
            case 3: // down
                //
                down(50, false);
                break;
            case 4: // up
                //
                up(50, false);
                break;
            case 5: // reset
                //
                reset(100, false);
                break;
            default:
                break;
        }
        //delay(10);
    }
}