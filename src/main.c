#include <stdlib.h>     //exit()
#include <string.h>
#include <signal.h>     //signal()
#include <sys/types.h>
#include <sys/stat.h>
#include "EPD_Test.h"   //Examples
#include "EPD_2in13_V2.h"

extern char* get(char *);
extern void curl_init(char *);
extern void EPD_INIT_PART(void);
extern void EPD_INIT_FULL(void);

void Handler(int signo)
{
  //System Exit
  printf("\r\nHandler:exit\r\n");
  EPD_2IN13_V2_Init(EPD_2IN13_V2_FULL);
  EPD_2IN13_V2_Clear();
  EPD_2IN13_V2_Sleep();
  DEV_Module_Exit();
  exit(0);
}

int main(void)
{
  char *values = malloc(sizeof(char) * 300);

  curl_init("http://192.168.178.21:8080");

  signal(SIGINT, Handler);
#if 1
  pid_t pid, sid;
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  umask(0);

  if ((sid = setsid()) < 0) {
    exit(EXIT_FAILURE);
  }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
#endif
  EPD_INIT();
  int epd_part_refresh_init_done = 0;
  int epd_full_init_done = 0;
  char **cpuInfo = malloc(sizeof(void*) * 4);
  for (int i = 0; i < 4; i++) {
    cpuInfo[i] = malloc(sizeof(char) * 100);
  }

  while (1) {
    char *res = get(values);

    if (strcmp(res, "err") == 0) {
      if (!epd_full_init_done) {
        EPD_INIT_FULL();
        paintImage();
        epd_full_init_done = 1;
        epd_part_refresh_init_done = 0;
      }
      sleep(5);
      continue;
    }

    int pos = 0;
    char *token = strtok(values, "\n");

    while (token) {
      snprintf(cpuInfo[pos], strlen(token) + 4, "%sC", token);
      token = strtok(NULL, "\n");
      pos++;
    }

    if (!epd_part_refresh_init_done) {
      EPD_INIT_PART();
      epd_part_refresh_init_done = 1;
      epd_full_init_done = 0;
    }

    paintScreen(cpuInfo);
    sleep(5);
  }
  free(values);
  return 0;
}
