#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/xattr.h>
#include <dirent.h>

#define MAX_TAG_LEN 20
#define MAX_TAGS 20
#define MAX_FILENAME_LEN 256

// Create a new file with the given name
int create_file(const char *filename) {
    int fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0644);

    if (fd == -1) {
        perror("Failed to create file");
        return -1;
    }

    close(fd);
    return 0;
}

int open_file(const char* filename) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    return fd;
}

// Read the contents of the file with the given name
char* read_file(const char *filename) {
    int fd = open(filename, O_RDONLY);

    if (fd == -1) {
        perror("Failed to read file");
        return NULL;
    }

    struct stat st;
    fstat(fd, &st);
    char *buffer = malloc(st.st_size + 1);
    memset(buffer, 0, st.st_size + 1);

    if (read(fd, buffer, st.st_size) == -1) {
        perror("Failed to read file");
        close(fd);
        free(buffer);
        return NULL;
    }

    close(fd);
    return buffer;
}

// Write the contents to the file with the given name
int write_file(const char *filename, const char *content) {
    int fd = open(filename, O_WRONLY);

    if (fd == -1) {
        perror("Failed to write file");
        return -1;
    }

    if (write(fd, content, strlen(content)) == -1) {
        perror("Failed to write file");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// Rename the file with the given name to the new name
int rename_file(const char *old_filename, const char *new_filename) {
    if (rename(old_filename, new_filename) == -1) {
        perror("Failed to rename file");
        return -1;
    }

    return 0;
}

// Add tag to the file with the given name
void add_tag(const char *filename, const char *tag) {
    char tag_filename[MAX_FILENAME_LEN];
    FILE *tag_file;
    char line[MAX_TAGS * (MAX_TAG_LEN + 1)];
    int i;

    // create the tag filename by appending ".tags" to the original filename
    int fd = open(filename, O_WRONLY);
    if (fd == -1) {
        printf("File does not exist. New file will be created.");
        create_file(filename);
        snprintf(tag_filename, MAX_FILENAME_LEN, "%s.tags", filename);
    }
    else {
        snprintf(tag_filename, MAX_FILENAME_LEN, "%s.tags", filename);
    }

    // open the tag file for appending
    tag_file = fopen(tag_filename, "a");
    if (!tag_file) {
        printf("Failed to open tag file for %s\n", filename);
        return;
    }

    // read the existing tags from the file
    if (fgets(line, MAX_TAGS * (MAX_TAG_LEN + 1), tag_file)) {
        // remove the newline character at the end of the line
        line[strcspn(line, "\n")] = 0;

       // append the new tag to the end of the line, separated by comma if there are already tags
    char *existing_tags = line;
    int existing_tags_len = strlen(existing_tags);
    if (existing_tags_len > 0) {
        existing_tags[existing_tags_len] = ',';  // Add a comma after the existing tags
        existing_tags_len++;
        existing_tags = line + existing_tags_len;  // Move the pointer to the next position
        line[existing_tags_len] = '\0';  // Add a null terminator at the end of the line
    }

strncat(existing_tags, tag, MAX_TAG_LEN);  // Concatenate the new tag to the line

        // write the updated tags to the file
        fseek(tag_file, 0, SEEK_SET);
        fputs(line, tag_file);
    } else {
        // tag file was empty, add new tag to the file
        fputs(tag, tag_file);
    }

    fclose(tag_file);
}


// Search for given tag among all files in specified directory
char *search_tag(const char *tag) {
    DIR *dir;
    struct dirent *ent;
    char filename[MAX_FILENAME_LEN];
    char tag_filename[MAX_FILENAME_LEN];
    FILE *tag_file;
    char line[MAX_TAGS * MAX_TAG_LEN];
    char *token;
    int tag_found;
    char *result = NULL;

    // open the current directory
    dir = opendir(".");
    if (!dir) {
        printf("Failed to open directory\n");
        return NULL;
    }

    // iterate over all files in the directory
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
            // check if the file has a .tags file
            snprintf(tag_filename, MAX_FILENAME_LEN, "%s.tags", ent->d_name);
            tag_file = fopen(tag_filename, "r");
            if (!tag_file) {
                continue;  // file doesn't have a .tags file
            }

            // read the tags from the file and check if the specified tag is present
            tag_found = 0;
            while (fgets(line, MAX_TAGS * MAX_TAG_LEN, tag_file)) {
                // tokenize the line by commas and check if the tag is present
                token = strtok(line, ",");
                while (token) {
                    if (strcmp(token, tag) == 0) {
                        tag_found = 1;
                        break;
                    }
                    token = strtok(NULL, ",");
                }
                if (tag_found) {
                    // tag found, append the path of the file to the result string
                    snprintf(filename, MAX_FILENAME_LEN, "%s/%s", ".", ent->d_name);
                    if (result) {
                        result = realloc(result, strlen(result) + strlen(filename) + 1);
                        strcat(result, " ");
                    } else {
                        result = malloc(strlen(filename) + 1);
                    }
                    strcat(result, filename);
                    break;
                }
            }

            fclose(tag_file);
        }
    }

    closedir(dir);

    // tag not found
    return result;
}


// Remove the specified tag from the file with the given name
void remove_tag(const char *filename, const char *tag) {
    char tag_filename[MAX_FILENAME_LEN];
    FILE *tag_file;
    char line[MAX_TAGS * (MAX_TAG_LEN + 1)];
    char new_line[MAX_TAGS * (MAX_TAG_LEN + 1)] = "";
    int tag_removed = 0;

    snprintf(tag_filename, MAX_FILENAME_LEN, "%s.tags", filename);

    // Open the tag file for reading and writing
    tag_file = fopen(tag_filename, "r+");
    if (!tag_file) {
        printf("Failed to open tag file for %s\n", filename);
        return;
    }

    // Read the existing tags from the file
    if (fgets(line, MAX_TAGS * (MAX_TAG_LEN + 1), tag_file)) {
        // Remove the newline character at the end of the line
        line[strcspn(line, "\n")] = '\0';

        // Tokenize the line by commas and check if the tag is present
        char *token = strtok(line, ",");
        while (token) {
            if (strcmp(token, tag) == 0) {
                tag_removed = 1;
            } else {
                // Append the tag to the new line
                if (strlen(new_line) > 0) {
                    strcat(new_line, ",");
                }
                strcat(new_line, token);
            }
            token = strtok(NULL, ",");
        }

        // Write the updated tags back to the file
        fseek(tag_file, 0, SEEK_SET);
        if (tag_removed) {
            if (strlen(new_line) > 0) {
                fputs(new_line, tag_file);
            } else {
                // No tags remaining, truncate the file
                FILE *temp_file = fopen("temp.txt", "w");
                fclose(temp_file);
                rename("temp.txt", tag_filename);
            }
        }
    }
    fclose(tag_file);
}
