#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define STR_SIZE 255
#define CMD_SIZE 512

int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cmd_line, int cs) {
  char cli_path[STR_SIZE] = { 0 }, 
       cmd_args[STR_SIZE] = "../boot.php ",
       cmd_buff[CMD_SIZE] = { 0 };

  // ---------------------
  
  if (strlen(cmd_line) > 0) 
    strncat(cmd_buff, cmd_line, STR_SIZE - strlen(cmd_buff) - 1); 
    
  GetPrivateProfileString("PHP_CLI", "path", NULL, cli_path, STR_SIZE, "../boot.ini");
  
  strncpy(cmd_buff, cli_path, STR_SIZE);  
  strncat(cmd_buff, " ", 1);
  strncat(cmd_buff, cmd_args, STR_SIZE);
  
  system(cmd_buff);
  return 0;
}
