/*
Name: Garrett Johnson
CSCI-3240 Project 3
Dr. Sainju
Date: 04/07/2025
*/

#include "csapp.h"

int main(int argc, char **argv) {
    int clientfd;           //file descriptor to communicate with the server
    char *host, *port; 
    size_t n;
    char buffer[MAXLINE];   //MAXLINE = 8192 defined in csapp.h
    int input;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    host = argv[1];
    port = argv[2];
    clientfd = Open_clientfd(host, port);    //wrapper function that calls getadderinfo, socket and connect functions for client side

    char firstName[MAXLINE], lastName[MAXLINE], zipCode[MAXLINE], department[MAXLINE];
    int salary;

    do {
        printf("\nOptions: \n");                                        // Menu options for client/server interaction
        printf("(1) Add Record\n");
        printf("(2) Search By Name\n");
        printf("(3) Search By Zip Code\n");
        printf("(4) Search By Salary\n");
        printf("(5) Terminate\n");
        printf("Select an option [1,2,3,4,5]: ");

        Fgets(buffer, MAXLINE, stdin);                                  // Get input from user
        input = atoi(buffer);                                           // Convert the input to integer 

    if (input == 5) {                                                   // If input is 5 then break out of the loop
      printf("Terminating the Client...\n");
      break;
      }

        switch (input) {                                                // Case
            case 1: {
                strcpy(buffer, "ADD_RECORD");                           // This is for adding record 
                n = write(clientfd, buffer, strlen(buffer));            // Send request to server
                if (n < 0) {
                    perror("Write Error!!");
                    Close(clientfd);
                    return -1;
                }

                printf("Enter First Name: ");                           // Enter in information to be entered into the file
                Fgets(firstName, MAXLINE, stdin);
                firstName[strcspn(firstName, "\n")] = '\0';

                printf("Enter Last Name: ");
                Fgets(lastName, MAXLINE, stdin);
                lastName[strcspn(lastName, "\n")] = '\0';

                printf("Enter Zip Code: ");
                Fgets(zipCode, MAXLINE, stdin);
                zipCode[strcspn(zipCode, "\n")] = '\0';

                printf("Enter Department: ");
                Fgets(department, MAXLINE, stdin);
                department[strcspn(department, "\n")] = '\0';
                
                printf("Enter Salary: ");
                Fgets(buffer, MAXLINE, stdin);
                salary = atoi(buffer);

                snprintf(buffer, MAXLINE, "%s,%s,%s,%s,%d",         // Combine into one string
                    firstName, 
                    lastName, 
                    zipCode, 
                    department, 
                    salary);
                
                n = write(clientfd, buffer, strlen(buffer));        // Send string to buffer
                if (n < 0) {
                    perror("Write Error!!");
                    Close(clientfd);
                    return -1;
                }

                bzero(buffer, MAXLINE);
                n = read(clientfd, buffer, MAXLINE);                
                if (n <= 0) {
                    if (n == 0) printf("Server disconnected\n");
                    else perror("Read Error!!");
                    Close(clientfd);
                    return -1;
                }
                printf("\nServer Response:\n%s", buffer);
                break;
            }
            case 2: {
                bzero(buffer, MAXLINE);
                strcpy(buffer, "SEARCH_NAME");                                  // Searching name in file for employee
                n = write(clientfd, buffer, strlen(buffer));                    // Write the request to server 
                if (n < 0) {
                    perror("Write Error!!");
                    Close(clientfd);
                    return -1;
                }

                printf("Enter First Name: ");                                   // Client enters first and last name
                Fgets(firstName, MAXLINE, stdin);
                firstName[strcspn(firstName, "\n")] = '\0';                     // Remove newline character

                printf("Enter Last Name: ");
                Fgets(lastName, MAXLINE, stdin);
                lastName[strcspn(lastName, "\n")] = '\0';

                snprintf(buffer, MAXLINE, "%s,%s", firstName, lastName);        // Combine string to send to server
                n = write(clientfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Write Error!!");
                    Close(clientfd);
                    return -1;
                }

                bzero(buffer, MAXLINE);                                         // Read results sent from server 
                n = read(clientfd, buffer, MAXLINE);
                if (n <= 0) {
                    if (n == 0) printf("Server disconnected\n");
                    else perror("Read Error!!");
                    Close(clientfd);
                    return -1;
                }
                printf("\nSearch Results by Name: %s %s\n", firstName, lastName);   // Display results
                printf("Server Response: \n");
                printf("%s", buffer);
                
                bzero(buffer, MAXLINE);
                
                printf("\n");

                break;
            }
            case 3: {
                bzero(buffer, MAXLINE);
                strcpy(buffer, "SEARCH_ZIPCODE");                               // For searching for zip code
                n = write(clientfd, buffer, strlen(buffer));                    // Send request to server
                if (n < 0) {
                    perror("Write Error!!");
                    Close(clientfd);
                    return -1;
                }

                printf("Enter Zip Code: ");                                     // Enter in zip code
                Fgets(zipCode, MAXLINE, stdin);
                zipCode[strcspn(zipCode, "\n")] = '\0';                         // Remove newline

                strcpy(buffer, zipCode);                                        // Copy zipcode to buffer and send to server
                n = write(clientfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Write Error!!");
                    return -1;
                }
                
                n = read(clientfd, buffer, MAXLINE);                            // Read results from server
                if (n < 0) {
                    perror("Read Error!!");
                    return -1;
                }
                
                printf("Search Results by Zip Code: %s\n", zipCode);            // Display results
                printf("Server Response: \n");
                printf("%s", buffer);

                bzero(buffer, MAXLINE);
                break;
            }
            case 4: {
                bzero(buffer, MAXLINE);
                strcpy(buffer, "SEARCH_SALARY");                                // For searching for salary
                n = write(clientfd, buffer, strlen(buffer));                    // Send request to server
                if (n < 0) {
                    perror("Write Error!!");
                    Close(clientfd);
                    return -1;
                }

                printf("Enter Salary: ");                                       // Client enters in salary and comparison operator
                Fgets(buffer, MAXLINE, stdin);
                salary = atoi(buffer);

                printf("Enter Comparison (==, <=, >=, <, >): ");
                char comparison[MAXLINE];
                Fgets(comparison, MAXLINE, stdin);
                comparison[strcspn(comparison, "\n")] = '\0';

                snprintf(buffer, MAXLINE, "%d,%s", salary, comparison);         // Send the input to the server
                n = write(clientfd, buffer, strlen(buffer));                    
                if (n < 0) {
                    perror("Write Error!!");
                    Close(clientfd);
                    return -1;
                }

                bzero(buffer, MAXLINE);                                         // Read the results from server 
                n = read(clientfd, buffer, MAXLINE);
                if (n < 0) {
                    perror("Read Error!!");
                    return -1;
                }
                
                printf("\nSearch Results by Salary: \n");               // Display the results
                printf("%s", buffer);
                break;
            }
            default:
                 printf("Invalid Option Entered. Please Try Again.\n\n");
                 break;
        }
    } while (1);

    Close(clientfd);
    return 0;
}