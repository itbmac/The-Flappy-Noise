/**
 robot taken from http://wiki.processing.org/index.php/Robot_class
 @author Ira Greenberg
 */

/* WARNING: This sketch takes over your mouse
 Press escape to close running sketch */

import java.awt.AWTException;
import java.awt.Robot;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import ddf.minim.*;

int[] flapArray = {
  1
    , 54
    , 99
    , 143
    , 185
    , 221
    , 267
    , 307
    , 343
    , 383
    , 429
    , 474
    , 504
    , 540
    , 583
    , 623
    , 658
    , 710
    , 747
    , 788
    , 825
    , 878
    , 922
    , 954
    , 982
    , 1015
    , 1063
    , 1094
    , 1143
    , 1188
    , 1226
    , 1263
    , 1295
    , 1332
    , 1378
    , 1413
    , 1460
    , 1503
    , 1539
    , 1588
    , 1634
    , 1670
    , 1707
    , 1752
    , 1796
    , 1829
    , 1869
    , 1899
    , 1942
    , 1975
    , 2020
    , 2070
    , 2107
    , 2145
    , 2188
    , 2230
    , 2268
    , 2310
    , 2357
    , 2399
    , 2436
    , 2454
    , 2493
    , 2534
    , 2571
    , 2621
    , 2662
    , 2696
    , 2748
    , 2793
    , 2829
    , 2869
    , 2883
    , 2900
    , 2946
    , 2977
    , 3019
    , 3079
    , 3114
    , 3155
    , 3168
    , 3207
    , 3251
    , 3285
    , 3331
    , 3376
    , 3411
    , 3446
    , 3482
    , 3527
    , 3565
    , 3598
    , 3648
    , 3682
    , 3718
    , 3766
    , 3808
    , 3846
    , 3889
    , 3926
    , 3970
    , 4008
    , 4047
    , 4087
    , 4126
    , 4171
    , 4213
    , 4252
    , 4295
    , 4337
    , 4378
    , 4416
    , 4459
    , 4496
    , 4539
    , 4574
    , 4619
    , 4662
    , 4701
    , 4744
    , 4781
    , 4817
    , 4866
    , 4907
    , 4946
    , 4985
    , 5028
    , 5068
    , 5111
    , 5150
    , 5191
    , 5233
    , 5271
    , 5314
    , 5359
    , 5399
    , 5437
    , 5477
    , 5516
    , 5556
    , 5596
    , 5635
    , 5675
    , 5716
    , 5767
    , 5806
    , 5841
    , 5874
    , 5918
    , 5960
    , 5999
    , 6043
    , 6081
    , 6127
    , 6160
};

int[] flapArray2 = {
  0
    , 58
    , 99
    , 140
    , 180
    , 218
    , 263
    , 305
    , 343
    , 382
    , 436
    , 477
    , 514
    , 553
    , 592
    , 631
    , 668
    , 712
    , 756
    , 794
    , 832
    , 882
    , 923
    , 960
    , 1000
    , 1049
    , 1089
    , 1127
    , 1165
    , 1203
    , 1239
    , 1277
    , 1330
    , 1371
    , 1409
    , 1447
    , 1483
    , 1521
    , 1558
    , 1597
    , 1638
    , 1676
    , 1715
    , 1767
    , 1807
    , 1844
    , 1885
    , 1926
    , 1966
    , 2003
    , 2041
    , 2079
    , 2116
    , 2154
    , 2207
    , 2247
    , 2286
    , 2324
    , 2369
    , 2406
};

int[] flapDist = {
  42
    , 39
    , 38
    , 36
    , 35
    , 42
    , 40
    , 35
    , 37
    , 48
    , 40
    , 35
    , 36
    , 35
    , 37
    , 35
    , 41
    , 41
    , 36
    , 36
    , 44
    , 41
    , 36
    , 40
    , 43
    , 39
    , 37
    , 34
    , 35
    , 35
    , 37
    , 45
    , 41
    , 37
    , 36
    , 31
    , 35
    , 34
    , 40
    , 42
    , 38
    , 37
    , 43
    , 41
    , 38
    , 39
    , 39
    , 38
    , 37
    , 37
    , 36
    , 35
    , 37
    , 44
    , 41
    , 37
    , 41
    , 42
    , 38
};

AudioPlayer player;
Minim minim;
Robot robot;
PImage flap;
int curFrame = 0;
int curFrameIndex = 0;
boolean playMode = false;
boolean countdownMode = false;
int yPos = 0;
int goalOff = 38;

void setup() {
  size(400, 400);
  noStroke();
  frameRate(60);

  minim = new Minim(this);
  player = minim.loadFile("flap.mp3");

  flap = loadImage("flap.png");

  try { 
    robot = new Robot();
  } 
  catch (AWTException e) {
    e.printStackTrace();
  }
  robot.mouseMove(600, 400);
}

void draw() {
  println(frameRate);
  fill(200, 40);
  rect(-2, -2, width+4, height+4);

  if (playMode)
  {
    image(flap, -50, -50 - yPos);

    if (yPos > 0)
      yPos--;

    /* else if (curFrameIndex% 16 == 1)
     goalOff = 2;
     else if (curFrameIndex% 16 == 2)
     goalOff = 1;
     else if (curFrameIndex% 16 == 3)
     goalOff = 2;
     else if (curFrameIndex% 16 == 4)
     goalOff = 0;
     else if (curFrameIndex% 16 == 5)
     goalOff = 2;
     else if (curFrameIndex% 16 == 6)
     goalOff = 1;
     else if (curFrameIndex% 16 == 7)
   
     goalOff = 2; */

    if (curFrame >= goalOff) //flapArray[curFrameIndex])
    {
      if (curFrameIndex% 3 == 0)
        goalOff = flapDist[curFrameIndex];
      else if (curFrameIndex% 22 == 0)
        goalOff = 45; 
      else
        goalOff = 38;

      robot.keyPress(KeyEvent.VK_LEFT);
      //robot.mousePress(InputEvent.BUTTON1_MASK);
      player.play();
      player.rewind();
      curFrameIndex++;
      curFrame = 0;
      yPos = 25;

      if (curFrameIndex >= flapDist.length)
      {
        curFrameIndex = 0;
        curFrame = 0;
        playMode = true;
      }

      fill(209);
      rect(-2, -2, width+4, height+4);
    }
    else
    {
    }
    curFrame++;
  }

  if (countdownMode)
  {
    curFrame++;

    textSize(100);
    fill(0);

    if (curFrame < 60)
    {
      text("3", 170, 200);
    }
    else if (curFrame < 120)
    {
      text("2", 170, 200);
    }
    else if (curFrame < 180)
    {
      text("1", 170, 200);
    }
    else
    {
      text("0", 170, 200);
      robot.keyPress(KeyEvent.VK_LEFT);

      countdownMode = false;
      playMode = true; 
      curFrame = 0;
      curFrameIndex = 0;
    }
  }
}

void keyPressed() {
  countdownMode = true;
  curFrameIndex = 0;
  curFrame = 0;
  playMode = false;
}

