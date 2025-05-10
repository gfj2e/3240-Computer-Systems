 /*
Name: Garrett Johnson
CSCI-3240 Project 4
Dr. Sainju
Date: 04/14/2025
*/

#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 1000
#define MAXSTRUCT 100
#define MAX_LENGTH 25
#define MAX_BUFFER MAX_SIZE * sizeof(char)

typedef struct {                                             // Struct for storing information of employee
    char firstName[MAX_LENGTH];
    char lastName[MAX_LENGTH];
    char zipCode[MAX_LENGTH];
    char department[MAX_LENGTH];
    int salary;
} Struct_Employee_Info;

Struct_Employee_Info employees[MAX_SIZE];                   // Made struct array global so all clients have access to the same struct
int readCount= 0;                                          // Read count for locking and unlocking
sem_t mutex, w;

void serverFunction(int connfd);
char *searchByName(Struct_Employee_Info employees[], const char *firstName, const char *lastName);
char *searchByZipCode(Struct_Employee_Info employees[], const char *zipCode);
char *searchBySalary(Struct_Employee_Info employees[], int salary, const char *comparisonOperator);
void *thread(void *vargp);

int main(int argc, char **argv) {
    int listenfd;
    int *connfd;                         // file descriptor to communicate with the client
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */
    pthread_t tid;
    sem_init(&mutex, 0, 1);
    sem_init(&w, 0, 1);
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]);  //wrapper function that calls getadderinfo, socket, bind, and listen functions in the server side

     //Server runs in the infinite loop.
     //To stop the server process, it needs to be killed using the Ctrl+C key.
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);

         // wait for the connection from the client.
        connfd = Malloc(sizeof(int));                                       // Malloc necessary to avoid race 
        *connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *)&clientaddr, clientlen, client_hostname,
                   MAXLINE, client_port, MAXLINE, 0);
            
        
        printf("Connected to (%s, %s)\n", client_hostname, client_port);

         // Pthead to do threading for multiple servers
        Pthread_create(&tid, NULL, thread, connfd);
    }
    exit(0);
}

void serverFunction(int connfd) {
    char buffer[MAXLINE];
    char firstName[MAXLINE], lastName[MAXLINE], zipCode[MAXLINE], department[MAXLINE];
    int salary;
    size_t n;
    char successMessage[MAXLINE] = "Server Request Completed\n\0";
    char errorMessage[MAXLINE] = "Server Request Error\n\0";
    int count = 0;
    // Load employee data
    FILE *file = fopen("records.csv", "r");
   
    char line[MAX_SIZE];
    

    if (file) {                                                             // Read in the file info into the array of structs
        while (fgets(line, sizeof(line), file) && count < MAX_SIZE) {
            sscanf(line, "%[^,],%[^,],%[^,],%[^,],%d",
                  employees[count].firstName, employees[count].lastName,
                  employees[count].zipCode, employees[count].department, 
                  &employees[count].salary);
            count++;
        }
        fclose(file);
    }

    bzero(buffer, MAXLINE); 
    while (1) {                                                 // Infinite loop for menu implentation and user

        bzero(buffer, MAXLINE);                                 // interaction with the server
        n = read(connfd, buffer, MAXLINE);
        if (n <= 0) break;

        printf("Server received: %s\n", buffer);

        if (strncmp(buffer, "ADD_RECORD", 10) == 0) {
            P(&w);                                              // Locking for writing
            bzero(buffer, MAXLINE);
            n = read(connfd, buffer, MAXLINE);
            if (n <= 0) break;

            if (count < MAXSTRUCT) {
                    // Parse the buffer data into the employee record
                    sscanf(buffer, "%[^,],%[^,],%[^,],%[^,],%d", 
                           employees[count].firstName,     // Adds first name
                           employees[count].lastName,      // Adds last name
                           employees[count].zipCode,       // Adds zip code
                           employees[count].department,    // Adds department
                           &employees[count].salary);  
            
                file = fopen("records.csv", "a");
            if (file) {                                  // Append the employee data to end of the file
                fprintf(file, "%s\n", buffer);
                count++;
                fclose(file);
                bzero(buffer, MAXLINE);
                write(connfd, "Record added successfully\nServer Request Completed\n", 
                      strlen("Record added successfully\nServer Request Completed\n"));
            }
            } else {
                bzero(buffer, MAXLINE);
                write(connfd, "Error: Could not open file\nServer Request Completed\n",     // Send error message if file could not be opened
                      strlen("Error: Could not open file\nServer Request Completed\n"));
            }
            V(&w);

        } else if (strncmp(buffer, "SEARCH_NAME", 11) == 0) {                               // For searching the name of a person in file
            P(&mutex);                                                                      // Locking for reading 
            readCount++;
            if (readCount == 1) {
                P(&w);
            }
            V(&mutex);

            bzero(buffer, MAXLINE);
            n = read(connfd, buffer, MAXLINE);                                              // Read in first and last name from buffer
            if (n <= 0) break;

            sscanf(buffer, "%[^,],%[^,]", firstName, lastName);
            char *result = searchByName(employees, firstName, lastName);                    // Parse and pass the string into searchByName
            bzero(buffer, MAXLINE);
            strcpy(buffer, result);                                                        // Copy result into buffer
            n = write(connfd, buffer, strlen(buffer));                                     // Send back to client
            if (n < 0) {
                perror("Write Error!!");
                write(connfd, errorMessage, strlen(errorMessage));
                return;
             }
            
            bzero(buffer, MAXLINE);
            free(result);
            write(connfd, successMessage, strlen(successMessage));

            P(&mutex);                                                                      // Unlocking
            readCount--;
            if (readCount == 0) {
                V(&w);
            }
            V(&mutex);

        } else if (strncmp(buffer, "SEARCH_ZIPCODE", 13) == 0) {                            // For searching Zip Code in file
            P(&mutex);                                                                      // Locking for reading
            readCount++;
            if (readCount == 1) {
                P(&w);
            }
            V(&mutex);

            bzero(buffer, MAXLINE);
            n = read(connfd, buffer, MAXLINE);                                              // Read in zip code from client
            if (n <= 0) break;

          char *result = searchByZipCode(employees, buffer);                                // Search for Zip Code entered in from client
            bzero(buffer, MAXLINE);
            strcpy(buffer, result);
            n = write(connfd, buffer, strlen(buffer));                                      // Send results back to client
            if (n < 0) {
                perror("Write Error!!");
                write(connfd, errorMessage, strlen(errorMessage));
                return;
            }
            
            bzero(buffer, MAXLINE);
            free(result);
            write(connfd, successMessage, strlen(successMessage));

            P(&mutex);                                                                      // Unlocking
            readCount--;
            if (readCount == 0) {
                V(&w);
            }
            V(&mutex);

        } else if (strncmp(buffer, "SEARCH_SALARY", 13) == 0) {                             // For searching salary in file
            P(&mutex);                                                                      // Locking for reading
            readCount++;
            if (readCount == 1) {
                P(&w);
            }
            V(&mutex);

            bzero(buffer, MAXLINE);
            n = read(connfd, buffer, MAXLINE);                                              // Read in salary and comparison operator from client    
            if (n <= 0) break;

            char salaryStr[MAXLINE], comparison[MAXLINE];                                   // Temp var for salary and comparison parsing
            sscanf(buffer, "%[^,],%[^,]", salaryStr, comparison);                           // Parse the client string
            salary = atoi(salaryStr);                                                       // Convert salary string to integer

            char *result = searchBySalary(employees, salary, comparison);                   // Search for salary
            write(connfd, result, strlen(result));
            if (n < 0) {
                perror("Write Error!!");
                write(connfd, errorMessage, strlen(errorMessage));
                return;
            }

            bzero(buffer, MAXLINE);
            free(result);
            write(connfd, successMessage, strlen(successMessage));

            P(&mutex);                                                                      // Unlocking
            readCount--;
            if (readCount == 0) {
                V(&w);
            }
            V(&mutex);
        }
    }
}

char* searchByName(Struct_Employee_Info employees[], const char *firstName, const char *lastName) {
    //creating a character array to store the result of the search operation
    char *result = malloc(MAX_BUFFER);
    /*The bzero() function erases the data in the MAX_SIZE bytes of the memory starting at the location pointed to by
     result, by writing zeros (bytes containing '\0') to that area.*/
    bzero(result, MAX_SIZE);
    //TODO Start:
    char *str = malloc(MAX_SIZE * sizeof(char));        // Allocate a temporary array to store string read
    for (int i = 0; i < MAXSTRUCT; i++) {
        if ((strcmp(firstName, employees[i].firstName) == 0) && (strcmp(lastName, employees[i].lastName) == 0)) {               // If first and last name match with employee, write the string into str buffer using sprintf
            sprintf(str, "%s %s\t %s\t %s\t %d\n", employees[i].firstName, employees[i].lastName, 
                employees[i].zipCode, employees[i].department, employees[i].salary);
            strcat(result, str);                                                                                                // Append to the result string
        }
    }

    if (result[0] == '\0') {                                // Function to check if result string is empty, then append a no matching records found function.
        strcpy(result, "No matching records found.\n");
    }

    free(str);                                              // Free str memory
    //TODo End
    return result;
}

char* searchByZipCode(Struct_Employee_Info employees[], const char *zipCode) {
    //creating a character array to store the result of the search operation
    char *result = malloc(MAX_BUFFER);
    /*The bzero() function erases the data in the MAX_SIZE bytes of the memory starting at the location pointed to by
     result, by writing zeros (bytes containing '\0') to that area.*/
    bzero(result, MAX_SIZE);
    //TODO Start:
    char *str = malloc(MAX_BUFFER);
    for (int i = 0; i < MAXSTRUCT; i++) {                           // Similar to the first function, append the found record to the result string if zip code found
        if ((strcmp(zipCode, employees[i].zipCode) == 0)) {
            sprintf(str, "%s %s\t %s\t %s\t %d\n", employees[i].firstName, employees[i].lastName, 
                employees[i].zipCode, employees[i].department, employees[i].salary);
            strcat(result, str);
        }
    }

    if (result[0] == '\0') {                                    // Same error check
        strcpy(result, "No matching records found.\n");
    }

    free(str);
    //TODo End
    return result;
}

char* searchBySalary(Struct_Employee_Info employees[], int salary, const char *comparisonOperator) {
    char *result = malloc(MAX_SIZE);
    bzero(result, MAX_SIZE);
    char temp[MAX_LENGTH * 5];
    bool found = false;

    for (int i = 0; i < MAXSTRUCT; i++) {
        bool match = false;
        if (strcmp(comparisonOperator, "==") == 0) match = (employees[i].salary == salary);
        else if (strcmp(comparisonOperator, "<=") == 0) match = (employees[i].salary <= salary);
        else if (strcmp(comparisonOperator, ">=") == 0) match = (employees[i].salary >= salary);
        else if (strcmp(comparisonOperator, "<") == 0) match = (employees[i].salary < salary);
        else if (strcmp(comparisonOperator, ">") == 0) match = (employees[i].salary > salary);

        if (match) {
            snprintf(temp, sizeof(temp), "%s %s\t%s\t%s\t%d\n",
                   employees[i].firstName, employees[i].lastName,
                   employees[i].zipCode, employees[i].department,
                   employees[i].salary);
            strcat(result, temp);
            found = true;
        }
    }

    if (!found) {
        strcpy(result, "No matching records found.\n");
    }
    return result;
}

void *thread(void *vargp) {             // Threading for serverfunction
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    serverFunction(connfd);
    Close(connfd);
    return NULL;
}