#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libapi.h"

int main() {
    int choice;
    char filename[30];
    char tag[30];
    char content[100];

    printf("\n-----------------------------------------------------\n");
    printf("\t\t\tMENU");
    printf("\n-----------------------------------------------------\n\n");
    printf("\t1. Create File\n");
    printf("\t2. Write File\n");
    printf("\t3. Read File\n");
    printf("\t4. Add Tag\n");
    printf("\t5. Search Tag\n");
    printf("\t6. Remoev Tag\n");
    printf("\t7. Exit\n");
    printf("\n-----------------------------------------------------\n");

    while (1) {
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: 
                printf("\nEnter filename: ");
                scanf("%s", filename);

                if (create_file(filename) == -1) {
                    printf("Failed to create file\n");
                } else {
                    printf("File created successfully\n");
                }
                break;

            case 2:
                printf("\nEnter filename: ");
                scanf("%s", filename);

                printf("Enter content: ");
                getchar(); // consume the newline character left by scanf
                fgets(content, sizeof(content), stdin);

                if (write_file(filename, content) == -1) {
                    printf("Failed to write file\n");
                } else {
                    printf("File written successfully\n");
                }
                break;

            case 3:
                printf("\nEnter filename: ");
                scanf("%s", filename);

                if (read_file(filename) == -1) {
                    printf("Failed to read file\n");
                }
                break;

            case 4:
                printf("\nEnter filename: ");
                scanf("%s", filename);

                printf("Enter tag: ");
                scanf("%s", tag);
                int length = strlen(tag);
                tag[length] = ',';  
                tag[length+1] = '\0';  

                add_tag(filename, tag);
                printf("Tag added successfully\n");
                break;

            case 5:
                printf("\nEnter tag: ");
                scanf("%s", tag);

                char *name = search_tag(tag);
                if (name) {
                    printf("Filename: %s\n", name);
                } else {
                    printf("Tag not found\n");
                }
                break;

            case 6:
                printf("\nEnter filename: ");
                scanf("%s", filename);

                printf("Enter tag: ");
                scanf("%s", tag);

                remove_tag(filename, tag);
                break;

            case 7: 
                exit(0);

            default:
                printf("\nInvalid choice\n");
        }
    }

    return 0;
}
