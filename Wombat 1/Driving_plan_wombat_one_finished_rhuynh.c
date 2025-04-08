#include <stdio.h>
#include <kipr/wombat.h>
#include <stdlib.h>
#define LEFT 0
#define RIGHT 3
#define compensation_rate 0.2
#define claw_click 0

// Turn 90 degrees right using one wheel
void ow_turnr_90() {
    mav(LEFT, 1000);
    msleep(2500);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

// Turn 90 degrees left using one wheel
void ow_turnl_90() {
    mav(RIGHT, 1000);
    msleep(2500);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

// Turn 45 degrees left using one wheel
void ow_turnl_45() {
    mav(RIGHT, 1000);
    msleep(1350);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

// Turn 90 degrees left using two wheels
void turnl_90() {
    mav(RIGHT, 500);
    mav(LEFT, -500);
    msleep(2500);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

// Turn 45 degrees left using two wheels
void turnl_45() {
    mav(RIGHT, 500);
    mav(LEFT, -500);
    msleep(1350);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

// Turn 90 degrees right using two wheels
void turnr_90() {
    mav(RIGHT, -500);
    mav(LEFT, 500);
    msleep(2600);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

// Turn 45 degrees right using two wheels
void turnr_45() {
    mav(RIGHT, -500);
    mav(LEFT, 500);
    msleep(1300);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

// Turn 45 degrees right using one wheel
void ow_turnr_45() {
    mav(RIGHT, 0);
    mav(LEFT, 1000);
    msleep(1300);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

// Make a servo position function to limit servo stress for the later slow servo function
int Set_servo_position(int port, int value) {
    enable_servos();
    if (port == 0 && (value < 543 || value > 2000)) { // Servo limit for port 0 is 543 to 2000
        printf("Value exceeds limit\n");
        return 0;
    }
    else if (port == 1 && (value < 750 || value > 1850)) { // Servo limit for port 1 is 850 to 1566
        printf("Value exceeds limit\n");
        return 0;
    }
    else if (port == 2 && (value < 430 || value > 2000)) { // Servo limit for port 2 is 430 to 2000
        printf("Value exceeds limit\n");
        return 0;
    }
    else if (port == 3 && (value < 641 || value > 1640)) { // Servo limit for port 3 is 641 to 1640
        printf("Value exceeds limit\n");
        return 0;
    }
    else { // If the input doesn't exceed limit, execute the set_servo_position function
        set_servo_position(port, value);
        return 1;
    }
    disable_servos();
}

// Make a drive function using encoders for precision
void drive(int distance, int speed) {
    cmpc(0);
    cmpc(3);
    while (abs(gmpc(0)) < distance) {

        mav(LEFT, speed*1);
        mav(RIGHT, speed*1);

    }
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

// Make a function to make the servo go up or open the claw
void move_servo_up_or_open_claw(int port, int start_pos, int end_pos, int step) {
    for (int pos = start_pos; pos <= end_pos; pos += step) {
        Set_servo_position(port, pos);
        msleep(100);
    }
}

// Make a function to make the servo go down or close the claw
void move_servo_down_or_close_claw(int port, int start_pos, int end_pos, int step) {
    for (int pos = start_pos; pos >= end_pos; pos -= step) {
        Set_servo_position(port, pos);
        msleep(100);
    }
}

void square_up_black_line(int speed) {
    float go_back = 0.7; //Make a go back variable and adjust it if needed when testing later on
    float grey_value = 3000; //Make a grey value variable to check for white and black surfaces
    while(analog(3) < grey_value || (analog(0) < grey_value)) { //While loop for squaring up
        if (analog(3) < grey_value) { //Check if right sensor is on white surface
            //the right sensor is on white
            mav(3, speed);
        }else { //Check if right sensor is on black surface
            //right sensor is on black
            mav(3, -speed * go_back);
        }
        if (analog(0) < grey_value) { //Check if left sensor is on white surface
            //the left sensor is on white
            mav(0, speed);
        }else { //Check if left sensor is on black surface
            //left sensor is on black
            mav(0, -speed * go_back);
        }
    }
    //Lock the motors
    mav(3, 0);
    mav(0, 0);
    msleep(50);
}

void line_follow(int distance, int speed) { //Declare function for line follow
    //Clear motor position counters
    cmpc(0);
    while (abs(gmpc(0)) < distance) { //Make a while loop to follow line
        int error = analog(3) - 3000; //Declare error variable
        float speed_modifier = error * compensation_rate; //Declare speed_modifier variable
        mav(0, speed - speed_modifier);
        mav(3, speed + speed_modifier);
    }

    //Lock motors
    mav(0, 0);
    mav(3, 0);
    msleep(20);

}

// Make a pause function for pausing during the game
void pause() {
    mav(RIGHT, 0);
    mav(LEFT, 0);
    msleep(500);
}

void grab_drink() {
    while (digital(claw_click) == 0) {
        mav(RIGHT, 500);
        mav(LEFT, -500);
    }
    mav(LEFT, 500);
    mav(RIGHT, -500);
    msleep(375);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
    move_servo_down_or_close_claw(0, 1600, 542, 30);
}

int main() {
    square_up_black_line(600); // Square up at the beginning of the game
    ow_turnr_90(); // Turn right 90 degrees using one wheel
    drive(1700, -1000); // Drive backwards to square up against the pvc pipe and fix both axis
    drive(400, 1000);
    turnl_90(); // Turn left
    drive(1000, -1500); // Drive back a bit to prepare for a square up
    pause(); // Pause after fast driving
    square_up_black_line(600); // Square up again for safety
    move_servo_down_or_close_claw(1, 1840, 1100, 30);
    drive(3850, 1400);
    move_servo_down_or_close_claw(0, 1600, 1000, 30);
    drive(1600, -1400);
    move_servo_up_or_open_claw(1, 1100, 1300, 30);
    turnr_90();
    drive(600, -1400);
    square_up_black_line(500);
    drive(1800, 1400);
    turnr_90();
    square_up_black_line(600);
    turnr_90();
    drive(300, -1000);
    square_up_black_line(500);
    move_servo_up_or_open_claw(0, 1000, 1600, 30);
    move_servo_up_or_open_claw(1, 1300, 1800, 30);
    drive(30, -600);
    turnr_90();
    drive(2500, -1400);
    square_up_black_line(600);
    move_servo_down_or_close_claw(1, 1800, 850, 30);
    drive(4500, 1400);
    grab_drink();
    drive(3000, -1400);
    move_servo_up_or_open_claw(1, 850, 1800, 30);
    ow_turnl_90();
    drive(300, -1000);
    square_up_black_line(600);
    drive(300, 400);
    move_servo_up_or_open_claw(0, 542, 1600, 30);
    return 0;
}
