#include <stdio.h>
#include <kipr/wombat.h>
#include <stdlib.h>
#define LEFT 0
#define RIGHT 3
#define compensation_rate 0.2

void ow_turnr_90() {
    mav(LEFT, 1000);
    msleep(2500);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

void ow_turnl_90() {
    mav(RIGHT, 1000);
    msleep(2500);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

void ow_turnl_45() {
    mav(RIGHT, 1000);
    msleep(1350);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

void turnl_90() {
    mav(RIGHT, 500);
    mav(LEFT, -500);
    msleep(2700);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

void turnl_45() {
    mav(RIGHT, 500);
    mav(LEFT, -500);
    msleep(1350);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

void turnr_90() {
    mav(RIGHT, -500);
    mav(LEFT, 500);
    msleep(2500);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

void turnr_45() {
    mav(RIGHT, -500);
    mav(LEFT, 500);
    msleep(1300);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

void ow_turnr_45() {
    mav(RIGHT, 0);
    mav(LEFT, 1000);
    msleep(1300);
    mav(0, 0);
    mav(3, 0);
    msleep(20);
}

int Set_servo_position(int port, int value) {
    enable_servos();
    if (port == 0 && (value < 543 || value > 2000)) {
        printf("Value exceeds limit\n");
        return 0;
    }
    else if (port == 1 && (value < 850 || value > 1566)) {
        printf("Value exceeds limit\n");
        return 0;
    }
    else if (port == 2 && (value < 430 || value > 2000)) {
        printf("Value exceeds limit\n");
        return 0;
    }
    else if (port == 3 && (value < 641 || value > 1640)) {
        printf("Value exceeds limit\n");
        return 0;
    }
    else {
        set_servo_position(port, value);
        return 1;
    }
    disable_servos();
}

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

void move_servo_up_or_open_claw(int port, int start_pos, int end_pos, int step) {
    for (int pos = start_pos; pos <= end_pos; pos += step) {
        Set_servo_position(port, pos);
        msleep(100);
    }
}

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

void pause() {
    mav(RIGHT, 0);
    mav(LEFT, 0);
    msleep(500);
}

int main() {
    
    square_up_black_line(600);
	ow_turnr_90();
    drive(1700, -1000);
    turnl_90();
    drive(1000, -1500);
    pause();
    square_up_black_line(600);
    move_servo_down_or_close_claw(1, 1500, 1150, 30);
    move_servo_up_or_open_claw(0, 1100, 1400, 30);
    drive(3750, 1000);
    move_servo_down_or_close_claw(0, 1400, 1100, 30);
    drive(2000, -1000);
    move_servo_up_or_open_claw(1, 1150, 1500, 30);
    return 0;
}
