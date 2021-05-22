#include "EPD_Test.h"
#include "EPD_2in13_V2.h"
#include "myimg.h"
#include "img_fire.h"
#include "img_temperature.h"
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 250
#define SCREEN_HEIGHT 122
#define FONT_USED Font24
void drawSun(void);
void paintOnlyTime(void);

static int timeTextStart;
static int timeTextEnd;

void paintTime() {
  time_t t;
  time(&t);
  char s[45];
  struct tm *timeinfo = localtime(&t);

  int hour = timeinfo->tm_hour + 1;
  if (hour == 24) {
    hour = 0;
  }

  sprintf(s, "%02d:%02d", hour, timeinfo->tm_min); 
  int slen = strlen(s);

  int xcoord = SCREEN_WIDTH / 2 - ((FONT_USED.Width * slen) / 2);
  int ycoord = SCREEN_HEIGHT / 2 - FONT_USED.Height / 2;

  timeTextStart = xcoord;
  timeTextEnd = xcoord + FONT_USED.Width * slen;

  Paint_ClearWindows(xcoord, ycoord - 1, xcoord + FONT_USED.Width * slen, ycoord + FONT_USED.Height, WHITE);
  Paint_DrawString_EN(xcoord, ycoord - 1, s, &FONT_USED, WHITE, BLACK);
}

int getDisplayPosX(int len) {
  int pixelLen = FONT_USED.Width * len;

  return SCREEN_WIDTH - pixelLen;
}

static int len0 = -1;
void paintCPU0(char *temp) {
  if (len0 == -1) len0 = strlen(temp);
  Paint_ClearWindows(0, 0, FONT_USED.Width * len0, FONT_USED.Height, WHITE);
  Paint_DrawString_EN(0, 0, temp, &FONT_USED, WHITE, BLACK);
  len0 = strlen(temp);
}

static int len1 = -1;
void paintCPU1(char *temp) {
  if (len1 == -1) len1 = strlen(temp);

  Paint_ClearWindows(getDisplayPosX(len1), 0, getDisplayPosX(len1) + FONT_USED.Width * len1, FONT_USED.Height, WHITE);
  len1 = strlen(temp);
  Paint_DrawString_EN(getDisplayPosX(len1), 0, temp, &FONT_USED, WHITE, BLACK);
}

static int len2 = -1;
void paintCPU2(char *temp) {
  if (len2 == -1) len2 = strlen(temp);
  int displayPos = SCREEN_HEIGHT - FONT_USED.Height;

  Paint_ClearWindows(0, displayPos - 1, FONT_USED.Width * len2, displayPos + FONT_USED.Height, WHITE);

  Paint_DrawString_EN(0, displayPos - 1, temp, &FONT_USED, WHITE, BLACK);
  len2 = strlen(temp);
}

static int len3 = -1;
void paintCPU3(char *temp) {
  int displayPosY = SCREEN_HEIGHT - FONT_USED.Height;
  if (len3 == -1) len3 = strlen(temp);

  Paint_ClearWindows(getDisplayPosX(len3), displayPosY, getDisplayPosX(len3) + FONT_USED.Width * len3, 
      displayPosY + FONT_USED.Height, WHITE);

  len3 = strlen(temp);
  Paint_DrawString_EN(getDisplayPosX(len3), displayPosY, temp, &FONT_USED, WHITE, BLACK);
}

static UBYTE *BlackImage;
static int isBlackImageInitialized = 0;

void initBlackImage() {
  if (isBlackImageInitialized) return;
  printf("init black image");
  UWORD Imagesize = ((EPD_2IN13_V2_WIDTH % 8 == 0) ? (EPD_2IN13_V2_WIDTH / 8 ) : 
      (EPD_2IN13_V2_WIDTH / 8 + 1)) * EPD_2IN13_V2_HEIGHT;
  if((BlackImage = (UBYTE *) malloc(Imagesize)) == NULL) {
      printf("Failed to apply for black memory...\r\n");
      exit(-1); 
  }
  isBlackImageInitialized = 1;
}

int EPD_INIT() {
  if (DEV_Module_Init() != 0) {
      return -1;
  }

  EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
  EPD_2IN13_V2_Clear();

  initBlackImage();

  Paint_NewImage(BlackImage, EPD_2IN13_V2_WIDTH, EPD_2IN13_V2_HEIGHT, 270, WHITE);
  Paint_SelectImage(BlackImage);
  Paint_SetMirroring(MIRROR_HORIZONTAL);
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

void drawImageAt(int xSize, char imgData[][xSize], int xStart, int yStart, int imgXLen, int imgYLen) {
  for (int i = 0; i < imgYLen; i++) {
    for (int j = 0; j < imgXLen; j++) {
      Paint_DrawPoint(j + xStart, i + yStart, imgData[i][j] == '1' ? WHITE : BLACK, 
          DOT_PIXEL_1X1, DOT_FILL_RIGHTUP);
    }
  }
}

void paintOnlyTime() {
  paintTime();
  drawImageAt(
      IMG_FIRE_DIM.width, IMG_FIRE, timeTextStart - 10 - IMG_FIRE_DIM.width, 
      SCREEN_HEIGHT / 2 - IMG_FIRE_DIM.height / 2, 
      IMG_FIRE_DIM.width, IMG_FIRE_DIM.height);
  drawImageAt(
      IMG_TEMPERATURE_DIM.width, IMG_TEMPERATURE, timeTextEnd + 10, 
      SCREEN_HEIGHT / 2 - IMG_TEMPERATURE_DIM.height / 2, 
      IMG_TEMPERATURE_DIM.width, IMG_TEMPERATURE_DIM.height);

  EPD_2IN13_V2_DisplayPart(BlackImage);
  DEV_Delay_ms(500);
}

void paintScreen(char **cpuInfo) {
  paintTime();
  drawImageAt(
      IMG_FIRE_DIM.width, IMG_FIRE, timeTextStart - 10 - IMG_FIRE_DIM.width, 
      SCREEN_HEIGHT / 2 - IMG_FIRE_DIM.height / 2, 
      IMG_FIRE_DIM.width, IMG_FIRE_DIM.height);
  drawImageAt(
      IMG_TEMPERATURE_DIM.width, IMG_TEMPERATURE, timeTextEnd + 10, 
      SCREEN_HEIGHT / 2 - IMG_TEMPERATURE_DIM.height / 2, 
      IMG_TEMPERATURE_DIM.width, IMG_TEMPERATURE_DIM.height);
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
