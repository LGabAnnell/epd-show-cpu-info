#include "EPD_Test.h"
#include "EPD_2in13_V2.h"
#include "myimg.h"
#include "img_fire.h"
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 250
#define SCREEN_HEIGHT 122
void drawSun(void);

static int timeTextStart;
static int timeTextEnd;

void paintTime() {
  time_t t;
  time(&t);
  char s[45];
  struct tm *timeinfo = localtime(&t);
  sprintf(s, "%02d:%02d", timeinfo->tm_hour + 1, timeinfo->tm_min); 
  int slen = strlen(s);

  int xcoord = 250 / 2 - ((Font20.Width * slen) / 2);
  int ycoord = 122 / 2 - Font20.Height / 2;

  timeTextStart = xcoord;
  timeTextEnd = xcoord + Font20.Width * slen;

  Paint_ClearWindows(xcoord, ycoord - 1, xcoord + Font20.Width * slen, ycoord + Font20.Height, WHITE);
  Paint_DrawString_EN(xcoord, ycoord - 1, s, &Font20, WHITE, BLACK);
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

  Paint_ClearWindows(0, displayPos - 1, Font20.Width * strlen(temp), displayPos + Font20.Height, WHITE);
  Paint_DrawString_EN(0, displayPos - 1, temp, &Font20, WHITE, BLACK);
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

  Paint_NewImage(BlackImage, EPD_2IN13_V2_WIDTH, EPD_2IN13_V2_HEIGHT, 270, WHITE);
  Paint_SelectImage(BlackImage);
  Paint_SetMirroring(MIRROR_HORIZONTAL); //
  Paint_Clear(WHITE);
  
  return 0;
}

void EPD_INIT_PART() {
  EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
  Paint_Clear(WHITE);

  EPD_2IN13_V2_DisplayPartBaseImage(BlackImage);
  EPD_2IN13_V2_Init(EPD_2IN13_V2_PART);
  Paint_SelectImage(BlackImage);
}

void EPD_INIT_FULL() {
  EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
  EPD_2IN13_V2_Clear();
}

void drawImageAt(int xSize, const char imgData[][xSize], int xStart, int yStart, int imgXLen, int imgYLen) {
  for (int i = 0; i < imgYLen; i++) {
    for (int j = 0; j < imgXLen; j++) {
      Paint_DrawPoint(j + xStart, i + yStart, imgData[i][j] == '1' ? WHITE : BLACK, 
          DOT_PIXEL_1X1, DOT_FILL_RIGHTUP);
    }
  }
}

void showPID() {
  char text[25];
  sprintf(text, "%d", getpid());
  int len = strlen(text);

  Paint_DrawString_EN(SCREEN_WIDTH / 2 - ((Font16.Width * len) / 2), SCREEN_HEIGHT - Font16.Height, 
      text, &Font16, BLACK, WHITE);
}

void paintScreen(char **cpuInfo) {
  paintTime();
  drawImageAt(
      IMG_FIRE_DIM.width, IMG_FIRE, timeTextStart - 10 - IMG_FIRE_DIM.width, 
      SCREEN_HEIGHT / 2 - IMG_FIRE_DIM.height / 2, 
      IMG_FIRE_DIM.width, IMG_FIRE_DIM.height);
  drawImageAt(
      IMG_FIRE_DIM.width, IMG_FIRE, timeTextEnd + 10, 
      SCREEN_HEIGHT / 2 - IMG_FIRE_DIM.height / 2, 
      IMG_FIRE_DIM.width, IMG_FIRE_DIM.height);
  paintCPU0(cpuInfo[0]);
  paintCPU1(cpuInfo[1]);
  paintCPU2(cpuInfo[2]);
  paintCPU3(cpuInfo[3]);
  showPID();

  EPD_2IN13_V2_DisplayPart(BlackImage);
  DEV_Delay_ms(500);
}

void paintImage() { 
  GUI_ReadBmp("./pic/sun.bmp", 0, 0);
  EPD_2IN13_V2_Display(BlackImage);
  DEV_Delay_ms(2000);
}
