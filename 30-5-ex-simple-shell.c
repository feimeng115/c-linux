#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAXLINE 1000

// command structure: 
// cmd arguments, < >, |

int is_blank(char c);
char * trim(char *s);
void exec_cmd(char *line);

int is_blank(char c) {
  return c == 0xA || c == 0x20;
}

void exec_cmd(char *line) {
  char *cmd;
  char * arguments[100];
  char out_redirect[MAXLINE];
  char in_redirect[MAXLINE];
  int i;
  char *pos;
  int fd;
  memset(out_redirect, 0, MAXLINE);
  memset(in_redirect, 0, MAXLINE);

  cmd = trim(line);

  // pipe TODO

  // in redirect
  pos = strchr(cmd, '<');
  if (pos != NULL) {
    *pos = ' ';
    for (pos++; is_blank(*pos) && *pos != '\0'; pos++);
    for (i = 0; !is_blank(*pos) && *pos != '\0'; pos++, i++) {
      in_redirect[i] = *pos;
      *pos = ' ';
    }
    if (strlen(in_redirect) > 0) {
      fd = open(in_redirect, O_RDONLY);
      if (fd < 0) {
        perror("open");
        exit(1);
      }
      dup2(fd, STDIN_FILENO);
      close(fd);
    }
  }

  // out redirect
  pos = strchr(cmd, '>');
  if (pos != NULL) {
    *pos = ' ';
    for (pos++; is_blank(*pos) && *pos != '\0'; pos++);
    for (i = 0; !is_blank(*pos) && *pos != '\0'; pos++, i++) {
      out_redirect[i] = *pos;
      *pos = ' ';
    }
    if (strlen(out_redirect) > 0) {
      fd = open(out_redirect, O_WRONLY);
      if (fd < 0) {
        perror("open");
        exit(1);
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
  }

  // arguments
  arguments[0] = strtok(cmd, " ");
  for (i = 1; (arguments[i] = strtok(NULL, " ")) != NULL; i++);

  // run
  execvp(arguments[0], arguments);
  perror("exec error");
}

char * trim(char *s)
{
  int n;
  int i;
  for (; is_blank(*s); s++);
  n = strlen(s);
  for (i = n-1; is_blank(s[i]) && i >= 0; i--) {
    s[i] = 0x0;
  }
  return s;
}

int main(void)
{
  char *promote_symbol = "my@ruff shell > ";
  char line[MAXLINE];
  int n;
  pid_t pid;
  int fd[2];

    while (1) {
      if (pipe(fd) < 0) {
        perror("pipe");
        exit(1);
      }

      fputs("my@ruff shell > ", stdout);
      fflush(stdout);
      n = read(STDIN_FILENO, line, MAXLINE-1);

      pid = fork();
      if (pid < 0) {
        perror("fork");
        exit(1);
      }
      if (pid > 0) {
        close(fd[0]);
        write(fd[1], line, n);
        int stat_val;
        waitpid(pid, &stat_val, 0);
      } else {
        close(fd[1]);
        n = read(fd[0], line, MAXLINE-1);
        line[n] = '\0';
        exec_cmd(line);
        exit(0);
      }

      
    }
    

  // promote symbol
  // cmd recognize, and "exec"
  // redirectory < >
  // pipe |
  // string parse: | < > \s 
}
