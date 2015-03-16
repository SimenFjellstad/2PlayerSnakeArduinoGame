#include <schematic.h>
#include <TVout.h>
#include <video_gen.h>

/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//  Author: Simen Fjellstad @ Frozen Apps                                      //
//  Email: zaimennor@gmail.com                                                 //
//  Date: 31. May 2014                                                         //
//  Version number: in-dev v0.03                                               //
//                                                                             //
//  This document is licensed under the Creative commons copyrights.           //
//  License document found here: http://creativecommons.org/licenses/by/4.0/   //
//                                                                             //
//  THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     //
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE        //
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A                 //
//  PARTICULAR PURPOSE.                                                        //
//                                                                             //
//  Summary:                                                                   //
//  This document is the basefile for a snakegame especially made for the      //
//  Arduino UNO. This program gets two player input from two 3bit inputs.      //
//  Whereas pin 2-4 and 5-7/8 is used. This program is dependent on the TVOut  //
//  Library.                                                                   //
//                                                                             //
//  Versions:                                                                  //
//  V0.01: Only singleplayer is added. As multiplayersupport will be added     //
//         in near future                                                      //
//                                                                             //
//  v0.02: 2nd player added. Mostly copypasting from 1 player code. Scheduled  //
//         to add collider listeners for each snake.(hitting the other player) //
//         I also need to add a point/growth system (random spawning target).  //
//                                                                             //
//  v0.03: Added a bunch of comments                                           //
/////////////////////////////////////////////////////////////////////////////////

TVout TV;

//Initialize constants
const int max_length = 100;
const int start_length = 10;
const int starting_game_speed = 50; //MS wait time (higher number = slower)
const int target_length_amount = 1;
const int target_size = 1;
//target_size is a square(1 = 1x1px, 2 = 2x2px, ...). I prefer size 1 for competition.

//Other variables
int screen_width, screen_height; //screensizes
int game_speed = starting_game_speed;
int state = 1;

int target_pos_x, target_pos_y;

int p1x, p1y, p1hspeed, p1vspeed, p1length; //p1 variables
int p2x, p2y, p2hspeed, p2vspeed, p2length; //p2 variables
int p1tailx[max_length], p1taily[max_length];
int p2tailx[max_length], p2taily[max_length];


void setup(){
  TV.begin(PAL, 80, 60); //Start streaming a PAL video signal @ 100x80
  //get screen size and add to variables
  screen_width = TV.hres(); 
  screen_height = TV.vres();

  //Assign variables before start
  p1x = 10;
  p1y = 10;
  p2x = screen_width - 10;
  p2y = screen_height - 10;
  target_pos_x = random(1, screen_width-2);
  target_pos_y = random(2, screen_height-2);
  game_speed = starting_game_speed;

  p1hspeed = 1; //Player1 startup direction = RIGHT
  p1length = start_length; //Set length to desired starting length
  p2hspeed = -1; //Player2 startup direction = LEFT
  p2length = start_length; //Set length to desired starting length

  for(int i = 0; i < max_length; i++){
    p1tailx[i] = p1x;
    p1taily[i] = p1y;
    p2tailx[i] = p2x;
    p2taily[i] = p2y;
  }
  delay(200);
}

void loop(){
  TV.draw_line( 2, 2, 5, 5, WHITE);
  switch(1){
    case 0:
    { //Start (menu?)
      TV.clear_screen();
      int input1 = getP1Input();
      TV.print(screen_width / 2, screen_height / 2, "Press Action to start");
      if(input1 == 5){
        TV.clear_screen();
        state = 1;
      }
      delay(100);
      break;
    }

    case 1: 
    { //Playing (in-game)
      if (p1length > max_length) p1length = max_length;
      if (p2length > max_length) p2length = max_length;
      //Overlap checks
      
      
      int input1 = getP1Input();
      int input2 = getP2Input();
      TV.clear_screen();


      if(input1 > 0){ //Check if input is made
        switch(input1){ //What input was made?
        case 1: //Move Up
          if(p1vspeed <= 0){
            p1vspeed = -1;
            p1hspeed = 0;
          }
          break;
        case 2: //Move Down
          if(p1vspeed >= 0){
            p1vspeed = 1;
            p1hspeed = 0;
          }
          break;
        case 3: //Move Left
          if(p1hspeed <= 0){
            p1vspeed = 0;
            p1hspeed = -1;
          }
          break;
        case 4: //Move Right
          if(p1hspeed >= 0){
            p1vspeed = 0;
            p1hspeed = 1;
          } 
          break;
        case 5: //Action (Stop moving)
          p1vspeed = 0;
          p1hspeed = 0;
          break;
        }
      } 

      if(input2 > 0){ //Check if input is made
        switch(input2){ //What input was made?
        case 1: //Move Up
          if(p2vspeed <= 0){
            p2vspeed = -1;
            p2hspeed = 0;
          }
          break;
        case 2: //Move Down
          if(p2vspeed >= 0){
            p2vspeed = 1;
            p2hspeed = 0;
          }
          break;
        case 3: //Move Left
          if(p2hspeed <= 0){
            p2vspeed = 0;
            p2hspeed = -1;
          }
          break;
        case 4: //Move Right
          if(p2hspeed >= 0){
            p2vspeed = 0;
            p2hspeed = 1;
          } 
          break;
        case 5: //Action (Stop moving)
          p2vspeed = 0;
          p2hspeed = 0;
          break;
        }
      }
      //Move the tail after our snakehead
      for (int i = max_length-1; i >= -1; i--){
        p1tailx[i] = p1tailx[i-1];
        p1taily[i] = p1taily[i-1]; 
        p2tailx[i] = p2tailx[i-1];
        p2taily[i] = p2taily[i-1];
      }

      //Update the tail attaching to the head (get heads last position)
      p1tailx[0] = p1x; 
      p1taily[0] = p1y;  
      p2tailx[0] = p2x; 
      p2taily[0] = p2y;  

      //Update our heads' positions
      p1x += p1hspeed;
      p1y += p1vspeed;
      p2x += p2hspeed;
      p2y += p2vspeed;

      //check for collision with own tail
      for(int i = 0; i < p1length; i++){
        if(p1x == p1tailx[i] && p1y == p1taily[i]){
          state = 3;
        }
        if(p2x == p2tailx[i] && p2y == p2taily[i]){
          state = 3;
        }
      }

      //check for collision with target (addLength)
      if(p1x == target_pos_x && p1y == target_pos_y){
        p1length += target_length_amount;
        moveTarget();
      }
      if(p2x == target_pos_x && p2y == target_pos_y){
        p2length += target_length_amount;
        moveTarget();
      }
      
      //Edge wrapping for player 1
      if(p1x > screen_width) p1x = 1;
      if(p1x < 0) p1x = screen_width-1;
      if(p1y > screen_height) p1y = 1;
      if(p1y < 0) p1y = screen_height-1;

      //Edge wrapping for player 2  
      if(p2x > screen_width) p2x = 1;
      if(p2x < 0) p2x = screen_width-1;
      if(p2y > screen_height) p2y = 1;
      if(p2y < 0) p2y = screen_height-1;

      //Draws snakeheads (current positions)
      TV.draw_line(p1x, p1y, p1x, p1y, WHITE); //Player 1
      TV.draw_line(p2x, p2y, p2x, p2y, WHITE); //Player 2

      //Draws snaketails
      for(int i = 0; i < p1length-1; i++){
        TV.draw_line(p1tailx[i], p1taily[i], p1tailx[i], p1taily[i], WHITE);
      }
      for(int i = 0; i < p2length-1; i++){
        TV.draw_line(p2tailx[i], p2taily[i], p2tailx[i], p2taily[i], WHITE);
      }
      
      //Draw target ball
      TV.draw_rect(target_pos_x, target_pos_y, target_size-1, target_size-1, WHITE);
      
      TV.delay(game_speed); //Wait's the game speed amount
      break;
    }

    case 2:
    { //Paused
      break;
    }

    case 3:
    { //Stopped (Death / loss)
      TV.print(20, 10, "Game over!");
      TV.print(18, 20, "Mwahahahah!");
      break;
    }
  }
}

int getP1Input(){
  int bit0 = digitalRead(2);
  int bit1 = digitalRead(3);
  int bit2 = digitalRead(4);
  return 1*bit0 + 2*bit1 + 4*bit2; //Reverse calculate the binary code inserted
}

int getP2Input(){
  int bit0 = digitalRead(5);
  int bit1 = digitalRead(6);
  int bit2 = digitalRead(8);
  return 1*bit0 + 2*bit1 + 4*bit2;  
}

void moveTarget(){
  target_pos_x = random(1, screen_width-2);
  target_pos_y = random(2, screen_height-2);
}
