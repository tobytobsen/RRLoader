#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>

#define STR_SIZE 255
#define CMD_SIZE 512

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cmd_line, int cs) {
  char cli_path[STR_SIZE] = { 0 }, 
       cmd_args[STR_SIZE] = "../boot.php ",
       cmd_buff[CMD_SIZE] = { 0 };

  // ---------------------
  
  if (strlen(cmd_line) > 0) 
    strncat(cmd_args, cmd_line, STR_SIZE - strlen(cmd_buff) - 1); 
    
  GetPrivateProfileString("PHP_CLI", "path", NULL, cli_path, STR_SIZE, "../boot.ini");
  
  /* CMD_SIZE = (STR_SIZE * 2) + " " + \0 */
  snprintf(cmd_buff, CMD_SIZE, "%s %s", cli_path, cmd_args);
  system(cmd_buff);
  
  return 0;
}
