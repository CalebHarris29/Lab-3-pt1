// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str[] = "howard.edu"; 
    char input_str[100]; 
    /* extra fixed string for parent final append */
    char fixed_str2[] = "gobison.org"; 
    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
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
  
    // Parent process 
    else if (p > 0) 
    { 
  
        close(fd1[0]);  // Close reading end of pipes 
        close(fd2[1]);  // parent will read from fd2[0] only
  
        // Write input string and close writing end of first 
        // pipe. 
        write(fd1[1], input_str, strlen(input_str)+1); 
        close(fd1[1]);  // done writing to child
        
  
        // Wait for child to print the concatenated string 
        wait(NULL); 
  
        // Now read back the string from child via second pipe
        char concat_back[200]; 
        read(fd2[0], concat_back, sizeof(concat_back)); 
        close(fd2[0]); // Close reading end of pipes 

        // Parent now appends "gobison.org"
        int k = strlen(concat_back); 
        int i; 
        for (i = 0; i < (int)strlen(fixed_str2); i++) 
            concat_back[k++] = fixed_str2[i]; 
  
        concat_back[k] = '\0'; 
  
        printf("Final string %s\n", concat_back);
    } 
  
    // child process 
    else
    { 
        close(fd1[1]);  // Close writing end of first pipes 
        close(fd2[0]);  // child will write to fd2[1] 
  
        // Read a string using first pipe 
        char concat_str[200]; 
        read(fd1[0], concat_str, sizeof(concat_str)); 
  
        // Concatenate a fixed string with it 
        int k = strlen(concat_str); 
        int i; 
        for (i=0; i<(int)strlen(fixed_str); i++) 
            concat_str[k++] = fixed_str[i]; 
  
        concat_str[k] = '\0';   // string ends with '\0' 
  
        printf("Concatenated string %s\n", concat_str);

        // Now prompt for the second input in the child
        char second_str[100];
        printf("Enter second string to concatenate:");
        scanf("%s", second_str);

        // Append second string
        for (i = 0; i < (int)strlen(second_str); i++)
            concat_str[k++] = second_str[i];

        concat_str[k] = '\0';

        // Send the result back to parent via second pipe
        write(fd2[1], concat_str, strlen(concat_str) + 1);
        close(fd2[1]); 

        // Close both reading ends 
        close(fd1[0]); 
  
        exit(0); 
    } 
} 
