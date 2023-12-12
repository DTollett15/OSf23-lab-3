#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <argument>\n", argv[0]);
    return 1;
  }

  int pipefd1[2];
  int pipefd2[2];
  int pid1, pid2;

  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL};
  char *sort_args[] = {"sort", NULL};

  // create the first pipe
  if (pipe(pipefd1) == -1)
  {
    perror("Pipe1 Failed");
    return 1;
  }

  pid1 = fork();

  if (pid1 == 0)
  {
    // child process (P2)

    // replace standard input with input part of pipe
    dup2(pipefd1[0], 0);

    // close unused half of pipe
    close(pipefd1[1]);

    // execute grep
    execvp("grep", grep_args);
  }
  else
  {
    // parent process (P1)

    // create the second pipe
    if (pipe(pipefd2) == -1)
    {
      perror("Pipe2 Failed");
      return 1;
    }

    pid2 = fork();

    if (pid2 == 0)
    {
      // child's child process (P3)

      // replace standard input with input part of the second pipe
      dup2(pipefd2[0], 0);

      // close unused half of the second pipe
      close(pipefd2[1]);

      // execute sort
      execvp("sort", sort_args);
    }
    else
    {
      // parent process (P1)

      // replace standard output with output part of the first pipe
      dup2(pipefd1[1], 1);

      // close unused input half of the first pipe
      close(pipefd1[0]);

      // close unused output half of the second pipe
      close(pipefd2[1]);

      // execute cat
      execvp("cat", cat_args);
    }
  }

  return 0;
}
