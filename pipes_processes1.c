#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main() 
{ 
    int fd[2];  // Used to store two ends of the pipe 
    int fd2[2];
  
    char fixed_str[] = "howard.edu"; 
    char fixed_str2[] = "gobison.org";
    char input_str[100]; 
    pid_t p; 
  
    if (pipe(fd)==-1 || pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
  
    printf("Enter a string to concatenate:");
    scanf("%s", input_str); 
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } 
  
    // Parent process (P1)
    else if (p > 0) 
    { 
        close(fd[0]);  // Close reading end of the first pipe 
        close(fd2[1]); // Close writing end of the second pipe
  
        // Write input string and close writing end of the pipe
        write(fd[1], input_str, strlen(input_str)+1); 
  
        // Wait for child (P2) to send a string
        wait(NULL); 

        // Read the concatenated string from P2
        char concat_str[100]; 
        read(fd2[0], concat_str, 100); 
  
        printf("Concatenated string %s\n", concat_str);
  
        // Close both ends of the pipes
        close(fd[1]); 
        close(fd2[0]); 

        // Prompt user for a second input
        printf("Enter another string to concatenate:");
        scanf("%s", input_str);

        // Write the second input string to P2
        write(fd[1], input_str, strlen(input_str)+1); 

        // Wait for P2 to concatenate and send back
        wait(NULL);

        // Read the final concatenated string from P2
        read(fd2[0], concat_str, 100);
        printf("Final Concatenated string %s\n", concat_str);

        // Close both ends of the pipes
        close(fd[1]); 
        close(fd2[0]); 
    } 
  
    // Child process (P2)
    else
    { 
        close(fd[1]);  // Close writing end of the first pipe 
        close(fd2[0]); // Close reading end of the second pipe
      
        // Read a string from P1 using the first pipe 
        char concat_str[100]; 
        read(fd[0], concat_str, 100); 
  
        // Concatenate a fixed string with it 
        int k = strlen(concat_str); 
        int i; 
        for (i=0; i<strlen(fixed_str); i++) 
            concat_str[k++] = fixed_str[i]; 
  
        concat_str[k] = '\0';   // string ends with '\0' 
  
        printf("Concatenated string %s\n", concat_str);
  
        // Write the concatenated string to P1 using the second pipe
        write(fd2[1], concat_str, strlen(concat_str)+1); 

        // Prompt user for a second input
        printf("Enter another string to concatenate:");
        scanf("%s", input_str);

        // Write the second input string to P1 using the second pipe
        write(fd2[1], input_str, strlen(input_str)+1);

        // Close both ends of the pipes
        close(fd[0]); 
        close(fd2[1]); 

        exit(0); 
    } 
}
