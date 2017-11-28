#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

char * get_prompt() {
    char *prompt = (char *)malloc(512);

    // Lengths based off of google searches for the max lengths
    // of username and hostname
    char user[33];
    getlogin_r(user, sizeof(user));

    char hostname[65];
    gethostname(hostname, sizeof(hostname));

    char cwd[256];
    char wd[256];
    getcwd(wd, sizeof(wd));

    char *homedir;
    homedir = getenv("HOME");
    if (strstr(wd, homedir) && strcspn(wd, homedir) == 0) {
        strcpy(cwd, "~");
        strcat(cwd, wd + strlen(homedir));
    } else {
        strcpy(cwd, wd);
    }

    sprintf(prompt, "%s@%s:%s $ ", user, hostname, cwd);
    return prompt;
}

int main() {
  char buffer[256];
  char *cmd = buffer;
  char *restCmd = buffer;
  char *arg;
  char *args[256];

  char * prompt;
  printf("%s", prompt = get_prompt());
  
  while (fgets(buffer, sizeof(buffer), stdin)) {

    cmd = buffer;
    restCmd = buffer;
    buffer[strlen(buffer) - 1] = 0;
    //cmd is the first command, restCmd holds the rest
    // of the commands if there are any
    cmd = strsep(&restCmd, ";");
    //cmd[strlen(cmd) - 1] = 0;
    
    while(cmd){

      int i;
      for (i = 0; cmd; i++) {
	arg = strsep(&cmd, " ");
	args[i] = arg;
      }

      args[i] = 0;

      // If exiting from shell
      if (strcmp(args[0], "exit") == 0) {
	//break;
	//return 0;
	exit(0);
      }

      // If changing directory
      if (strcmp(args[0], "cd") == 0) {
	if (args[1]) {
	  chdir(args[1]);
	} else {
	  chdir(getenv("HOME"));
	}

	//printf("%s", prompt = get_prompt());
	continue;
      }

      // Run through child process otherwise
      int f = fork();
      if (f == 0) {
	execvp(args[0], args);
      }

      int status;
      wait(&status);
      
      cmd = strsep(&restCmd, ";");  
    }
  
    printf("%s", prompt = get_prompt());
  }
  free(prompt);
}
