/*
 * Jason Choi, jawchoi@ucsc.edu
 * Jason Alvarez, jcalvare@ucsc.edu
 * Dennis Tran, dehutran@ucsc.edu
 * Jordan McCall, jmccall@ucsc.edu
 *
 * Purpose: This file implements a simple unix-like shell.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

extern char **get_line();
extern int _argcount;

/*
 * function prototypes
 */
void cd (char **args);
void file_output (char **args, int index);
void file_input (char **args, int index);
void pipeline (char **args, int index);

/***************************************************************
 * cd (args)
 * Description: a function that changes the current directory
 * Input: the list of arguments
 * Output: Nothing
 * Result: the directory has been changed
 ***************************************************************/
void cd (char **args) { 
   if (args[1] == NULL) {
      if (chdir (getenv("HOME")) != 0) {
         perror ("chdir");
      }
   }else {      
      if (chdir (args[1]) != 0) {
         perror ("chdir");
      }
   }
}

/***************************************************************
 * file_output (args, index)
 * Description: a function that redirects stdout to a file
 * Input: the list of arguments, and the index of the position
 *        of ">" in the args array
 * Output: nothing
 * Result: redirects stdout to the specified file
 ***************************************************************/
void file_output (char **args, int index) {
   freopen (args[index + 1], "w", stdout);
}

/***************************************************************
 * file_input (args, index)
 * Description: a function that redirects a file to stdin
 * Input: the list of arguments, and the index of the position
 *        of "<" in the args array
 * Output: nothing
 * Result: redirects a file to stdin
 ***************************************************************/
void file_input (char **args, int index) {
   freopen (args[index + 1], "r", stdin);
}

/***************************************************************
 * pipeline (args, pipe_index)
 * Description: a function that pipes one command to another
 * Input: the list of arguments, and the index of the position
 *        of "|" in the args array
 * Output: the output of the piped command
 * Result: a piped command has been executed or an error occured
 ***************************************************************/
void pipeline (char **args, int pipe_index) {
   pid_t pid;
   int fd[2]; //file descriptor
   char *path = malloc (50);

   args[pipe_index] = NULL;
   pipe (fd);

   if (fork() == 0) {
      dup2 (fd[1], 1);
      strcat (path, args[0]);
      execvp (path, args);
   }else {
      close (fd[1]);
   }

   if (fork() == 0) {
      dup2 (fd[0], 0);
      strcat (path, args[pipe_index+1]);
      execvp (path, &(args[pipe_index+1])); 
   } else {
      waitpid (pid);
   }
}

/***************************************************************
 * Main - Starts the shell
 ***************************************************************/
int main() {
   int background = 0;
   char **args;
   int status;
   char *path = malloc (50); 
   pid_t pid;
   int file_input_index = 0;
   int file_output_index = 0;
   int pipe_index = 0;

   while(1) {
      args = get_line();
      pid = waitpid (-1, &status, WNOHANG);
      background = (strcmp (args[_argcount-1], "&") == 0);

      for (int i = 0; args[i] != NULL; i++) {
         if (strcmp (args[i], "<") == 0) file_input_index = i;
         else if (strcmp (args[i], ">") == 0) file_output_index = i;
         else if (strcmp (args[i], "|") == 0) pipe_index = i;
      }

      if (background) {
         args[_argcount-1] = NULL;
      }

      if (strcmp(args[0], "cd") == 0) {
         cd (args);
      }else if (strcmp (args[0], "exit") == 0) {
         exit(0);
      }else if (pipe_index != 0) {
         pipeline (args, pipe_index);
      }else {
         pid = fork();

         // Child process
         if (pid == 0) {
            //Check if there is file redirection
            if (file_input_index != 0) {
               file_input (args, file_input_index);
               args[file_input_index] = NULL;
            }else if (file_output_index != 0) {
               file_output (args, file_output_index);
               args[file_output_index] = NULL;
            }

            strcat(path, args[0]);
            int ret = execvp (path, args);

            if (ret < 0) {
               perror (args[0]);
               exit(1);
            }
         }else if (!background) {
            pid = waitpid (pid, &status, 0);
         }
      }
      file_input_index = 0;
      file_output_index = 0;
      pipe_index = 0;
   }
}
