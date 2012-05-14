#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>

#define CHR_SIZE 255
#define CMD_SIZE 511

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR args, int cs) {
  char cli[CHR_SIZE] = { 0 }, 
       cmd[CHR_SIZE] = "../boot.php ",
       exe[CMD_SIZE] = { 0 };

  // ---------------------
  
  if (strlen(args) > 0) 
    strncat(cmd, args, sizeof(cmd) - strlen(cmd) - 1); 
    
  GetPrivateProfileString("PHP_CLI", "path", NULL, cli, CHR_SIZE, "../boot.ini");

  strncpy(exe, cli, CMD_SIZE - 1);
  strncat(exe, " ", CMD_SIZE - CHR_SIZE - 1);
  strncat(exe, cmd, CHR_SIZE);
  
  system(exe);  
  return 0;
}
