/*
Contributers: 
Nimish Nijhawan - Programer
Roger Huynh - Programer
Vincent Tran - Coach

Program Summary:
This program is our aplication in order to grab cups, and add drinks and ice into them.
It uses smart positioning in order to make sure our robot knows where it is at all times
*/
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

// Make a servo position function to limit servo stress for the later slow servo function
int Set_servo_position(int port, int value) {
    enable_servos();
    if (port == 0 && (value < 320 || value > 2000)) { // Servo limit for port 0 is 543 to 2000
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
    else if (port == 3 && (value < 30 || value > 1940)) { // Servo limit for port 3 is 641 to 1640
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

// Make a square up function for black lines
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
        int error = analog(5) - 3000; //Declare error variable
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
    move_servo_down_or_close_claw(0, 1600, 542, 30);
}

// Function for deciding which cup to grab
int decidecup(int look){
    if (look == 0){
        camera_load_config("redcup");
        camera_update();
        if (get_object_count(0) < 1){
            return 1;
        }
        int x = get_object_center_x(0,0);
        if (x >= 350){
            return 3;
        }else if ( x <= 350 && x >= 200){
            return 2;
        }else{
            return 1;
        }

    }else if (look == 1){
        camera_load_config("greencup");
        camera_update();
        if (get_object_count(0) < 1){
            return 1;
        }
        int x = get_object_center_x(0,0);

        if (x >= 350){
            return 3;
        }else if ( x <= 350 && x >= 200){
            return 2;
        }else{
            return 1;
        }


    }else{
        camera_load_config("bluecup");
        camera_update();
        if (get_object_count(0) < 1){
            return 1;
        }
        int x = get_object_center_x(0,0);
        if (x >= 400){
            return 3;
        }else if ( x <= 400 && x >= 200){
            return 2;
        }else{
            return 1;
        }

    }
}

// Function for turning at different angles
void turn(int angle){
    if (angle < 0){
        mav(RIGHT, 500);
        mav(LEFT, -500);

    }else{
        mav(RIGHT, -500);
        mav(LEFT, 500);
    }

    angle = abs(angle);
    angle = angle * 2500;
    angle = angle/90;

    msleep(angle);

    mav(0,0);
    mav(3,0);

}

int distance_from_cup = 1200;

//Function for knowing the distance to drive towards an object
void drive_until_cup() {
    while (analog(4) < distance_from_cup) {
        mav(0, 1000);
        mav(3, 1000);
    }
    mav(0, 0);
    mav(3, 0);
}

int whatcolor = 1; // 0 for red, 1 for green

//  start pos for port 3 1013, move later to 1375
int main() {
    while (digital(0) == 0 && digital(1)==0){
        msleep(1);
    }
    if (digital(0) == 1){
        whatcolor = 1;
        printf("GO FOR GREEN");
    }else if (digital(1) ==1){
        whatcolor = 0;
        printf("GO FOR RED");
    }
    pause();
    pause();
    pause();
    pause();
    pause();
    pause();
	set_servo_position(3,990);
    wait_for_light(1);
    shut_down_in(119);
 
    camera_open();

    enable_servos();
    move_servo_up_or_open_claw(3, 1013, 1375, 20);
    mav(0,0);
    mav(0,0);
    msleep(3000);
    enable_servos();

    move_servo_down_or_close_claw(0, 1300, 900, 50); // Reset claw
    turn(15);
    drive(1800, -1300); // Drive back to square up

    turn(-15);
    drive(1200, -1300); // Drive back to square up
    pause();
    pause();
    pause();
    pause();  
    pause();
    pause();
    pause();
    
    
    
    


    mav(LEFT,0);
    mav(RIGHT,1300);
    msleep(2000);
  
	turn(-20);



    square_up_black_line(600); // Square up
    move_servo_down_or_close_claw(1, 1600, 1000, 50); //Reset arm
    set_servo_position(2,1744);
    //Deciding which direction to go depending on the cup
    int firstcup = decidecup(whatcolor);
    int blue = decidecup(2);

    printf("firstcup pos is %d and blue cup %d\n", firstcup,blue);
    
    if (firstcup == 2){
        turn(-2);
        drive(3400, 1300);
        drive(200, 400);
        move_servo_down_or_close_claw(0, 1000, 600, 50);
        pause(); // Slight pause
        drive(1000,-1300); // Drive backwards
        move_servo_up_or_open_claw(1, 1000, 1100, 40);
        turn(45); // Turn 45 degrees right
    } else if (firstcup == 3){
        turn(6.5);
        drive(3800, 1300);
        move_servo_down_or_close_claw(0, 1000, 600, 30);
        pause(); // Slight pause
        drive(1000,-1300); // Drive backwards
        move_servo_up_or_open_claw(1, 1000, 1100, 40);
        turn(38.5); // Turn 45 degrees right
    }else{
        turn(-2);
        drive(3400, 1300);
        drive(200, 400);
        move_servo_down_or_close_claw(0, 1000, 600, 50);
        pause(); // Slight pause
        drive(1000,-1300); // Drive backwards
        turn(45); // Turn 45 degrees right
    }
    
    drive(500, 1300); // Drive forward a bit
    turn(45); // Turn another 45 degrees right
    drive(2000, -1450); // Square up against PVC pipe
    square_up_black_line(600); // Squares up against black line
    drive(500, 1300); // Drive forward a bit
    turn(45); // Turn 45 degrees right
    drive(500, 1300); // Drive a bit more forward
    turn(45); // Turn 45 degrees right
    pause(); // Slight pause
    square_up_black_line(600); // Squares up against black line
    pause(); // Slight pause
    move_servo_down_or_close_claw(1, 1100, 1000, 50);
    move_servo_up_or_open_claw(0, 700, 1050, 40); // Open claw to release cup
    move_servo_up_or_open_claw(1, 1000, 1750, 40); // Raise arm up
    drive(500, -1000); // Drive backwards
    turn(-45); // Turns left 45 degrees
    drive(1000, 1000); // Drives forward a bit
    turn(-45); // Turns left 45 degrees
    drive(2800, 1000); // Drive towards the ice
    turn(90); // Turn 90 degrees right
    drive(1500, -1000); // Squares up against pvc pipe in front of ice
    drive(300, 1000); // Moves forward a bit
    turn(2);
    set_servo_position(2,1270);
    msleep(500);
    move_servo_down_or_close_claw(3, 1300, 50, 50); // Turn arm down
    move_servo_down_or_close_claw(2, 1270, 430, 50); // Close claw and grab ice
    pause();
    move_servo_up_or_open_claw(3, 50, 1300, 30); // Turn arm up
    pause();
    pause();
    pause();
    pause();
    turn(-90); // Turn left 45 degrees
    drive(2130, -1300); // Drive backwards towards the cup
    turn(-96);
    drive(2100, -1300);
    square_up_black_line(600);
    move_servo_down_or_close_claw(1, 1750, 700, 50);
    move_servo_down_or_close_claw(0, 1050, 950, 50);
    drive(4250, 1300);
    move_servo_down_or_close_claw(0, 820, 400, 50);
    drive(2550, -1300);
    move_servo_up_or_open_claw(1, 820, 1750, 50);
    turn(90);
    drive(1470, -1300);
    turn(-45); // Turn 45 degrees
    drive(300, 1300); // drives forward a bit
    turn(-47); // Turn 45 degrees again
   // move_servo_down_or_close_claw(3, 1900, 1500, 40); // Lower arm down a bit to ensure that ice don't fall out of cup
    turn(-6);
    drive(150, 1300);
    move_servo_up_or_open_claw(2, 430, 1750, 40); // Release claw to put poms in
   // move_servo_up_or_open_claw(3, 1500, 1900, 40); // Raise arm up again
    drive(500, 1300); // Drive forward a bit
    turn(180);
    square_up_black_line(600);
    drive(700, 1300);
    move_servo_down_or_close_claw(1, 1750, 1650, 50);
    move_servo_up_or_open_claw(0, 450, 1300, 50);
    move_servo_up_or_open_claw(1, 1650, 1750, 50);
    drive(1100, -1300);
    move_servo_down_or_close_claw(1, 1750, 1000, 50);
    square_up_black_line(600);
    drive(70, 500);
    move_servo_down_or_close_claw(0, 1300, 600, 50);
    turn(-45);
    /*drive(200,1300);
    turn(-65);
    drive(3800, 1300);
    move_servo_up_or_open_claw(1, 1000, 1300, 40);
    turn(45);
    square_up_black_line(600);
    move_servo_down_or_close_claw(1, 1300, 1000, 40);*/
    move_servo_up_or_open_claw(0, 600, 1000, 40);
    return 0;
}
