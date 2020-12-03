/*****************************************************************************
* | File      	:   EPD_2IN13_V2_test.c
* | Author      :   Waveshare team
* | Function    :   2.13inch e-paper(V2) test demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-13
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_Test.h"
#include "EPD_2in13_V2.h"
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 250
#define SCREEN_HEIGHT 122

void paintTime() {
  time_t t;
  time(&t);
  char s[45];
  struct tm *timeinfo = localtime(&t);
  sprintf(s, "%02d:%02d:%02d", timeinfo->tm_hour + 1, timeinfo->tm_min, timeinfo->tm_sec); 
  int slen = strlen(s);

  int xcoord = 250 / 2 - ((Font20.Width * slen) / 2);
  int ycoord = 122 / 2 - Font20.Height / 2;
  Paint_ClearWindows(xcoord, ycoord, xcoord + Font20.Width * slen, ycoord + Font20.Height, WHITE);
  Paint_DrawString_EN(xcoord, ycoord, s, &Font20, WHITE, BLACK);
}

void paintCPU0(char *temp) {
  Paint_ClearWindows(0, 0, Font20.Width * strlen(temp), Font20.Height, WHITE);
  Paint_DrawString_EN(0, 0, temp, &Font20, WHITE, BLACK);
}

void paintCPU1(char *temp) {
  int len = strlen(temp);
  int pixelLen = Font20.Width * len;
  int displayPos = SCREEN_WIDTH - pixelLen;

  Paint_ClearWindows(displayPos, 0, displayPos + Font20.Width * len, Font20.Height, WHITE);
  Paint_DrawString_EN(displayPos, 0, temp, &Font20, WHITE, BLACK);
}

void paintCPU2(char *temp) {
  int displayPos = SCREEN_HEIGHT - Font20.Height;

  Paint_ClearWindows(0, displayPos, Font20.Width * strlen(temp), displayPos + Font20.Height, WHITE);
  Paint_DrawString_EN(0, displayPos, temp, &Font20, WHITE, BLACK);
}

void paintCPU3(char *temp) {
  int len = strlen(temp);
  int pixelLen = Font20.Width * len;
  int displayPosX = SCREEN_WIDTH - pixelLen;
  int displayPosY = SCREEN_HEIGHT - Font20.Height;

  Paint_ClearWindows(displayPosX, displayPosY, displayPosX + Font20.Width * len, displayPosY + Font20.Height, WHITE);
  Paint_DrawString_EN(displayPosX, displayPosY, temp, &Font20, WHITE, BLACK);
}

static UBYTE *BlackImage;
static int isBlackImageInitialized = 0;

void initBlackImage() {
  if (isBlackImageInitialized) return;
  printf("init black image");
  UWORD Imagesize = ((EPD_2IN13_V2_WIDTH % 8 == 0)? (EPD_2IN13_V2_WIDTH / 8 ): (EPD_2IN13_V2_WIDTH / 8 + 1)) * EPD_2IN13_V2_HEIGHT;
  if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
      printf("Failed to apply for black memory...\r\n");
      exit(-1); 
  }
  isBlackImageInitialized = 1;
}


int EPD_INIT()
{
  if(DEV_Module_Init()!=0){
      return -1;
  }

  EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
  EPD_2IN13_V2_Clear();

  initBlackImage();
  /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */

  Paint_NewImage(BlackImage, EPD_2IN13_V2_WIDTH, EPD_2IN13_V2_HEIGHT, 270, WHITE);
  Paint_SelectImage(BlackImage);
  Paint_SetMirroring(MIRROR_HORIZONTAL); //
  Paint_Clear(WHITE);


  Paint_SelectImage(BlackImage);
  
  return 0;
}

void EPD_INIT_PART() {
  EPD_2IN13_V2_Clear();
  EPD_2IN13_V2_DisplayPartBaseImage(BlackImage);
  EPD_2IN13_V2_Init(EPD_2IN13_V2_PART);
  Paint_SelectImage(BlackImage);
}

void EPD_INIT_FULL() {
  EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
  EPD_2IN13_V2_Clear();
}

void paintScreen(char **cpuInfo) {
  paintTime();
  paintCPU0(cpuInfo[0]);
  paintCPU1(cpuInfo[1]);
  paintCPU2(cpuInfo[2]);
  paintCPU3(cpuInfo[3]);

  EPD_2IN13_V2_DisplayPart(BlackImage);
  DEV_Delay_ms(500);
}

void paintImage() { 
  GUI_ReadBmp("./pic/sun.bmp", 0, 0);
  EPD_2IN13_V2_Display(BlackImage);
  DEV_Delay_ms(2000);
}
