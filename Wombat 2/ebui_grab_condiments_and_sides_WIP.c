

#include <kipr/wombat.h>
#include <stdlib.h>
#include <stdio.h>
//port (0) 992-375
//port (2) 350-1045
#define RIGHT 0
#define LEFT 3
#define back_RIGHT 1
#define back_LEFT 2
#define grey_value (220+ 3500)/2
#define go_back_R 0.79
#define go_back_L 0.81
//grey value at home base (215+3730)/2
//port 0: parallelgram thing 380 highest point lowest point 1044?

void  mecanum_drive(int distance, int speed){
    cmpc(RIGHT);
    cmpc(LEFT);
    cmpc(back_RIGHT);
    cmpc(back_LEFT);
    while(abs(gmpc(3))< distance){
        mav(RIGHT,speed);
        mav(LEFT,speed);
        mav(back_RIGHT,speed);
        mav(back_LEFT, speed);
        msleep(1);
    }
    mav(RIGHT,0);
    mav(LEFT,0);
    mav(back_LEFT,0);
    mav(back_RIGHT,0);
    msleep(10);
}


void mecanum_drive_right(int distance, int speed){
    cmpc(RIGHT);
    cmpc(LEFT);
    cmpc(back_RIGHT);
    cmpc(back_LEFT);

    while(abs(gmpc(3))< distance){
        mav(RIGHT,-speed);
        mav(LEFT,speed);
        mav(back_RIGHT,speed);
        mav(back_LEFT, -speed);
        msleep(1);
    }
    mav(0,0);
    mav(1,0);
    mav(2,0);
    mav(3,0);
    msleep(10);
}
void mecanum_drive_left(int distance, int speed){
    cmpc(RIGHT);
    cmpc(LEFT);
    cmpc(back_RIGHT);
    cmpc(back_LEFT);

    while(abs(gmpc(3))< distance){
        mav(RIGHT,speed);
        mav(LEFT,-speed);
        mav(back_RIGHT,-speed);
        mav(back_LEFT, speed);
        msleep(1);
    }
    mav(0,0);
    mav(1,0);
    mav(2,0);
    mav(3,0);
    msleep(20);
}
void left(int turn_amount, int speed){
    //1980 turn_amount and 800 speed is 90 degrees
    //980 turn_amount and 800 speed is 45 degrees
    cmpc(RIGHT);
    cmpc(LEFT);
    cmpc(back_RIGHT);
    cmpc(back_LEFT);

    while(abs(gmpc(3))< turn_amount){
        mav(RIGHT,speed);
        mav(LEFT,-speed);
        mav(back_RIGHT,speed);
        mav(back_LEFT, -speed);
        msleep(1);
    }
    mav(0,0);
    mav(1,0);
    mav(2,0);
    mav(3,0);
    msleep(20);
}

void right(int turn_amount, int speed){
    //2100 turn_amount and 800 speed is 90 degrees
    cmpc(RIGHT);
    cmpc(LEFT);
    cmpc(back_RIGHT);
    cmpc(back_LEFT);

    while(abs(gmpc(3))< turn_amount){
        mav(RIGHT,-speed);
        mav(LEFT,speed);
        mav(back_RIGHT,-speed);
        mav(back_LEFT, speed);
        msleep(1);
    }
    mav(0,0);
    mav(1,0);
    mav(2,0);
    mav(3,0);
    msleep(20);
}



void lineFollow(int distance, int speed){
    cmpc(3);
    cmpc(2);
    cmpc(1);
    cmpc(0);
    while(gmpc(3)<distance){
        //error is how much it goes in one sides it will correct 
        //itself to turn to the other side
        int error = analog(1)-3500;//error > 0 = black
        // error <0 = white
        float speed_mod = error*1;
        mav(RIGHT,speed-speed_mod);
        mav(LEFT,speed+speed_mod);
        mav(back_RIGHT,speed-speed_mod);
        mav(back_LEFT,speed+speed_mod);

    }
    mav(RIGHT,0);
    mav(LEFT,0);
    mav(back_RIGHT,0);
    mav(back_LEFT,0);
    msleep(20);
}



void square_up(int speed){
    //USE 800 AS THE SPEED
    while(analog(RIGHT)<grey_value || analog(LEFT)<grey_value){

        if(analog(RIGHT) < grey_value){
            //right sensor on white
            mav(RIGHT,speed);
            mav(back_RIGHT,speed);
        }else{
            //right sensor on black
            //this will make the botball go backwards to allow it to recheck which if it's alined
            //same on the left side but on the left
            mav(RIGHT,-speed*go_back_R);
            mav(back_RIGHT,-speed*go_back_R);
        }

        if(analog(LEFT)<grey_value){
            //left sensor on white
            mav(LEFT,speed);
            mav(back_LEFT,speed);
        }else{
            //left sensor on black
            mav(LEFT,-speed*go_back_L);
            mav(back_LEFT,-speed*go_back_L);
        }
    }
    //to make sure that the motors stop when the function is done
    mav(RIGHT,0);
    mav(LEFT,0);
    mav(back_LEFT,0);
    mav(back_RIGHT,0);
    msleep(50);

}








void servo(int port, int end_pos, int step){
    enable_servos();
    int servo_pos = get_servo_position(port);


    if(servo_pos<= end_pos){
        for (int pos = servo_pos; pos <= end_pos; pos += step) {
            set_servo_position(port, pos);
            msleep(30);
        }
    }else{
        for (int pos = servo_pos; pos >= end_pos; pos -= step) {
            set_servo_position(port, pos);
            msleep(30);
        }

    }
    mav(0,0);
    mav(1,0);
    mav(2,0);
    mav(3,0);
    msleep(300);
}


//competition functions
void potato_grab(){
    //    mecanum_drive_left(1,588,);
    
    // msleep(2000);


    //servo(0,584,10);

    //  servo(0, 849,5);
    //servo(1,764,10);
}
void stop(int time){
    mav(0,0);
    mav(1,0);
    mav(2,0);
    mav(3,0);
}


/*
void tomato_grab(){
    //after it drops the potato, it line follows to aline itself with the tomato
    servo(0,875,10);
    servo(1,537,10);
    stop(1);
    left(1980,800);
    stop(1);
    mecanum_drive_left(200,1200);
    stop(1);
    lineFollow(1000,1200);
    stop(1);
    left(1980,800);
    stop(1);
    mecanum_drive(1000,1200);
    stop(1);
    mecanum_drive_left(750,1200);
    stop(9);

    //trys to grab the tomato
    stop(1);
    servo(0,1085,7);
    servo(1,925,5);
    servo(0,918,10);
    mecanum_drive(1500,-1200);

    //  servo(0,554,10);
    left(1980,800);
    servo(0,824,10);
    //    
    //line follows to drop it off at the second tray
    stop(1);
    lineFollow(2300,1200);
    stop(1);

    left(1980,800);

    stop(1);
    mecanum_drive_left(50,1200);
    stop(1);

    //drops the tomato to the tray
    mecanum_drive(700,800);
    stop(1);
    servo(0,896,10);
    servo(1,689,10);   
    }
*/


void condiments_grab_3(){
    servo(1,630,15);
    servo(0,2047,15);
    servo(1,865,10);
    mecanum_drive(900,1000);
    mecanum_drive_right(200,800);
    lineFollow(2150,800);
    mecanum_drive(100,-800);
    stop(1);
    servo(1,1140,10);
    servo(0,1780,10);
}

void first_pom_set(){
    condiments_grab_3();

    //goes roughly towards the closests tray
    mecanum_drive(750,1000);
    right(1980,800);

    mecanum_drive_left(200,800);
    mecanum_drive(200,800);
    //goes to the pipe to aline closely to the tray
    while(digital(0)==0){
        mav(RIGHT,1000);
        mav(LEFT,1000);
        mav(back_LEFT,1000);
        mav(back_RIGHT,1000);
        msleep(10);    
    }
    mecanum_drive(200,800);
    mecanum_drive(150,-800);
    mecanum_drive_right(150,800);

    //drops the condiments
    servo(1,700,15);
    mecanum_drive_left(100,1200);
    mecanum_drive_right(100,1200);


}
void second_pom_set(){
    //aline's itself with the next pom set
    left(1980,800);
    mecanum_drive(100,-1000);
    mecanum_drive_left(980,1000);
    //condiments_grab_3 function but adjusticed to acount for the bump on the table
    servo(1,630,10);
    servo(0,2047,10);
    servo(1,920,10);
    msleep(20);
    //roughly alines itself with the trays
    mecanum_drive_left(200,1200);
    mecanum_drive(2000,750);
    mecanum_drive_right(100,1200);
    lineFollow(2000,800);
    stop(1);
    msleep(200);
    servo(1,1140,10);
    servo(0,1780,10);
    right(1980,800);
    mecanum_drive_right(500,1200);
    //goes to the pipe to aline the tray
    while(digital(0)==0){
        mav(RIGHT,1000);
        mav(LEFT,1000);
        mav(back_LEFT,1000);
        mav(back_RIGHT,1000);
        msleep(10);    
    }

    mecanum_drive(200,800);
    mecanum_drive(150,-800);
    mecanum_drive_left(50,1000);
    //drops the condiments
    servo(1,700,15);

    mecanum_drive_left(100,1200);
    mecanum_drive_right(200,1200);
}
void potato(){
    msleep(500);
        servo(1,475,15);

    	servo(0,1485,10);

    mecanum_drive(500,-1200);
    mecanum_drive_left(500,1200);
    left(1980,800);
    mecanum_drive_left(500,1200);
    lineFollow(4750,1200);
    stop(1);
   	left(2000,800);
     mecanum_drive(2250,-1200);
    mecanum_drive_right(400,1200);
    lineFollow(2600,1200);
    mecanum_drive_right(50,1200);

    servo(0,1790,10);
    servo(1,1043,20);
    servo(0,1485,10);
    stop(1);
    //alines the bot to be on the black line
    left(1980,800);


    stop(1000);
    //follows the line until it's at the tray
    lineFollow(2700,1300);
    stop(1);
    left(2010,800);
        stop(1);
    mecanum_drive_right(100,1000);
    while(digital(0)==0){
        mav(RIGHT,1000);
        mav(LEFT,1000);
        mav(back_LEFT,1000);
        mav(back_RIGHT,1000);
        msleep(10);    
    }
	    mecanum_drive(100,1200);

    mecanum_drive(200,-1200);
    mecanum_drive_left(50,1200);
    //drops of the potato
    stop(1000);
//    servo(0,820,10);
    servo(1,486,15);
}

void poms(){
	mecanum_drive(1000,-1200);
    right(1980,800);
    // mecanum_drive_left(200,1200);
    lineFollow(2200,1200);
    mecanum_drive_right(750,1200);
    servo(0,2047,15);
    servo(1,1043,10);
    servo(0,1675,10);
    mecanum_drive(1600,-1200);
    mecanum_drive_left(100,1200);
    left(2010,800);
        while(digital(0)==0){
        mav(RIGHT,1000);
        mav(LEFT,1000);
        mav(back_LEFT,1000);
        mav(back_RIGHT,1000);
        msleep(10);    
    }
    mecanum_drive(100,1200);

    mecanum_drive(200,-1200);
    mecanum_drive_left(50,1200);
    //drops of the potato
    stop(1000);
  //  servo(0,820,10);
    servo(1,486,15);
}
int main(){
   // wait_for_light(5);
       // lineFollow(20000,1500);
	msleep(1000);
    servo(0,2047,10);
    servo(1,1080,10);
//gets out of the start box

    mecanum_drive(2000,-1300);
    //becomes perpendicular to the pipe (squaring up)
    mecanum_drive_left(250,1200);
   	left(1980,900);
        mecanum_drive_right(750,1300);

    mecanum_drive(800,-1300);

    servo(0,1650,20);
    //gets to first condiment
    mecanum_drive(2700,1300);
    
    first_pom_set();
    second_pom_set();
    potato();
    poms();
    disable_servos();

    return 0;
}
