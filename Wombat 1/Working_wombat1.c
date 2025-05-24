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
void Set_servo_position(int port, int value) {
    enable_servos(); // Enable servos to make sure they activate
    if (port == 0 && (value < 320 || value > 2000)) { // Servo limit for port 0 is 543 to 2000
        printf("Value exceeds limit\n");
    }
    else if (port == 1 && (value < 750 || value > 1850)) { // Servo limit for port 1 is 850 to 1566
        printf("Value exceeds limit\n");
    }
    else if (port == 2 && (value < 430 || value > 2000)) { // Servo limit for port 2 is 430 to 2000
        printf("Value exceeds limit\n");
    }
    else if (port == 3 && (value < 30 || value > 1940)) { // Servo limit for port 3 is 641 to 1640
        printf("Value exceeds limit\n");
    }
    else { // If the input doesn't exceed limit, execute the set_servo_position function
        set_servo_position(port, value);
    }
    disable_servos(); // Turn off servos to save battery
}

// Make a drive function using encoders for precision
void drive(int distance, int speed) {
    cmpc(0); // Reset left wheel counter position
    cmpc(3); // Reset right wheel counter position
    while (abs(gmpc(0)) < distance) { // Drive until given distance

	// Drive both wheels forward
        mav(LEFT, speed);
        mav(RIGHT, speed);

    }

    // Lock motors after driving given distance
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
    float go_back = 0.7; //Make a go back variable to adjust speed during square up
    float grey_value = 3000; //Make a grey value variable to check for white and black surfaces
    while(analog(3) < grey_value || (analog(0) < grey_value)) { //While loop for squaring up
        if (analog(3) < grey_value) { //Check if right sensor is on white surface
            //the right sensor is on white
            mav(3, speed); // Right motor drive forward
        }else { //Check if right sensor is on black surface
            //right sensor is on black
            mav(3, -speed * go_back); // Turns to align sensors
        }
        if (analog(0) < grey_value) { //Check if left sensor is on white surface
            //the left sensor is on white
            mav(0, speed); // Left motor drive forward
        }else { //Check if left sensor is on black surface
            //left sensor is on black
            mav(0, -speed * go_back); // Turns to align sensors
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
    // If specified angle is less than 0, turn left
    if (angle < 0){
        mav(RIGHT, 500);
        mav(LEFT, -500);

    }else{ // If specified angle is larger than or equal to 0, turn right
        mav(RIGHT, -500);
        mav(LEFT, 500);
    }

    // Use this formula to figure out how long to turn
    angle = abs(angle);
    angle = angle * 2500;
    angle = angle/90;

    msleep(angle);

    // Lock the motors after turning
    mav(0,0);
    mav(3,0);
    msleep(10);
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


int main() {
    // This while loop makes the robot unable to move until we press the green or red button
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
    // Long pause to get ready
    pause();
    pause();
    pause();
    pause();
    pause();
    pause();

    // Reset the back arm (port 3) to fit in the height limit
    set_servo_position(3,990);

    // Game officially begins. No touching the robot
    wait_for_light(1);
    shut_down_in(119); // Automatically turns off after 119 seconds
 
    camera_open(); // Turns on camera

    enable_servos(); // Enable servos

    move_servo_up_or_open_claw(3, 1013, 1375, 20); // Raise the back arm up

    // Long pause for the other robot to get out of the starting box
    mav(0,0);
    mav(0,0);
    msleep(3000);

    move_servo_down_or_close_claw(0, 1300, 900, 50); // Reset claw
    turn(15); // Turns a bit so that later it doesn't hit pvc while turning
    drive(1800, -1300); // Drive back towards the pvc pipe
    turn(-15);
    drive(1200, -1300); // Drive back to square up

    // Long pause to wait for the other robot
    pause();
    pause();
    pause();
    pause();  
    pause();
    pause();
    pause();

    // Turns to prepare to square up in front of the black line
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

    // If the cup is in the middle or right, go for it. Else, just go for the middle
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
    drive(300, 1000); // Moves forward a bit to prepare grabbing ice
    turn(2); // Turns to align itself

    // Preparing claw to grab ice
    set_servo_position(2,1270);
    msleep(500);

    move_servo_down_or_close_claw(3, 1300, 50, 50); // Turn arm down
    move_servo_down_or_close_claw(2, 1270, 430, 50); // Close claw and grab ice
    pause();
    move_servo_up_or_open_claw(3, 50, 1300, 30); // Turn arm up
    // Long pause to prevent servo lag
    pause();
    pause();
    pause();
    pause();
    turn(-90); // Turn left 45 degrees
    drive(2130, -1300); // Drive backwards towards the cup
    turn(-96); // Turns towards the drinks with a bit of buffer
    drive(2100, -1300); // Moves back to square up against black line
    square_up_black_line(600); // Squares up

    // Preparing front arm and claw for grabbing the drink
    move_servo_down_or_close_claw(1, 1750, 700, 50);
    move_servo_down_or_close_claw(0, 1050, 950, 50);

    // Drive towards the drink
    drive(4250, 1300);
    move_servo_down_or_close_claw(0, 820, 400, 50); // Close claw and grab drink
    drive(2550, -1300); // Drive backwards to the cup
    move_servo_up_or_open_claw(1, 700, 1750, 50); // Turn front arm up
    turn(90); // Turn 90 degrees
    drive(1470, -1300); // Move backwards to align against cup
    turn(-45); // Turn 45 degrees
    drive(300, 1300); // drives forward a bit
    turn(-47); // Turn 45 degrees again
    turn(-6); // Add a buffer to make sure back claw is aligned with cup
    drive(150, 1300); // Drive forward
    move_servo_up_or_open_claw(2, 430, 1750, 40); // Release claw to put poms in
    drive(500, 1300); // Drive forward a bit
    turn(180); // Makes a 180 degree turn to put the drink in
    square_up_black_line(600); // Squares up against a convienient black line
    drive(700, 1300); // Drives forward to align claw with the cup

    // Release drink into cup
    move_servo_down_or_close_claw(1, 1750, 1650, 50);
    move_servo_up_or_open_claw(0, 450, 1300, 50);
    move_servo_up_or_open_claw(1, 1650, 1750, 50);
    // Moves back and prepares to grab the cup
    drive(1100, -1300);
    move_servo_down_or_close_claw(1, 1750, 1000, 50); // Move front arm down
    square_up_black_line(600); // Squares up against previous black line
    drive(70, 500); // Add a buffer to make sure we grab the cup
    move_servo_down_or_close_claw(0, 1300, 600, 50); // Close claw and grab cup
    turn(-45); // Turns 45 degrees so that the cup is not on the black line
    move_servo_up_or_open_claw(0, 600, 1000, 40); // Release cup
    return 0;
}
