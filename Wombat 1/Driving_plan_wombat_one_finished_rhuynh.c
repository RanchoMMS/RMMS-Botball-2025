#include <stdio.h>
#include <kipr/wombat.h>
#include <stdlib.h>
#define LEFT 0
#define RIGHT 3
#define compensation_rate 0.2

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
    msleep(2700);
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
    msleep(2500);
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
    else if (port == 1 && (value < 850 || value > 1566)) { // Servo limit for port 1 is 850 to 1566
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

int main() {
    
    square_up_black_line(600); // Square up at the beginning of the game
	ow_turnr_90(); // Turn right 90 degrees using one wheel
    drive(1700, -1000); // Drive backwards to square up against the pvc pipe and fix both axis
    turnl_90(); // Turn left
    drive(1000, -1500); // Drive back a bit to prepare for a square up
    pause(); // Pause after fast driving
    square_up_black_line(600); // Square up again for safety
    drive(3000, 1400); // Drive forward towards the cups
    drive(2000, -1400); // Goes back
    turnr_45(); // Turn right 45 degrees
    drive(3500, 1400); // Drive towards the drink dispenser
    turnl_45(); // Turn left 45 degrees to make the robot face the drink dispenser
   	drive(3500,-1400); // Move back and prepare for square up
    square_up_black_line(500); // Square up against black line
    drive(3500,1400); // Drive towards the drink dispenser
    // Long pause for pretending to grab drinks into the cup
    pause();
    pause();
    pause();
    pause();

    drive(3500,-1400); // Drives back
    turnr_90(); // Turns right 90 degrees
    drive(3000,1400); // Go forward a bit
    turnr_90(); // Turn right 90 degrees again
    square_up_black_line(500); // Squares up on the black line near the beverage station
    drive(300, -1400); // Moves back a little
    turnl_90(); // Turns left
    drive(7000, -1000); // Moves backward towards the PVC pipe to square up
    turnl_90(); // Turns left
    pause(); // Pause after turning and before squaring up towards the black line
    square_up_black_line(500); // Squares up
    drive(3000, 1400); // Drives towards the cup a second time to grab another cup
    drive(2000, -1400); // Moves back
    turnr_45(); // Turns 45 degrees
    drive(3500, 1400); // Drive toward the drink dispenser
    turnl_45(); // Turn left for the robot to face the drink dispenser
   	drive(4100,-1400); // Move back
    square_up_black_line(500); // Squares up facing black line
    drive(3500,1400); // Go towards the drink dispenser
    // Pause for time pretending to grab the remaining drink
    pause();
    pause();

    drive(4100, -1400); // Drive backwards
    square_up_black_line(500); // Square up on the black line
    turnr_90(); // Turn right
    drive(3000,1400); // Go forward a bit
    turnr_90(); // Turn right again
    square_up_black_line(500); // Square up on the black line facing the beverage station
    drive(300, -1400); // Move back a bit
    turnl_90(); // turn left
    drive(7000, -1400); // go back and square up at pvc pipe
    drive(500, 1400); // Drive forward a bit
    turnr_90(); // turn right
    drive(2500, -1400); // move back and prepare for black line square up
    square_up_black_line(500); // square up at black line
    drive(1000, -1400); // move back to grab entrees
    // Pretending to grab entrees
    pause();
    pause();
    
    square_up_black_line(1000); // Squares up on the black line
    ow_turnl_90(); // Turns left
    drive(6000, 1400); // Goes towwards the trays
    turnl_90(); // Turns left
    drive(2500, -1400); // Squares up against the pvc pipe and puts the entree in
    return 0;
}