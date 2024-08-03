

/*#include "hw2.h"

parseInfo* parse(char* cmdLine) {
    int index = 0; 
    int numTokens = countTokens(cmdLine); 
    parseInfo* p_info = (parseInfo*)malloc(sizeof(parseInfo));
    p_info->tokens = (char**)malloc(sizeof(char*) * (numTokens + 1));
    if (strlen(cmdLine) == 0) {
        p_info->tokens[0] = strdup("");
        return p_info;
    }
    char* token = strtok(cmdLine, " ");
    if (strcmp(token, "exit") == 0) {
        printf("exit!");
        if (cmdLine) {
            free(cmdLine);
        }
        exit(1);
    }

    while (token != NULL) {
        p_info->tokens[index++] = strdup(token);
        token = strtok(NULL, " ");
    }
    p_info->size = index;
    p_info->tokens[index] = NULL;
    if (cmdLine) free(cmdLine);
    return p_info;
}

int countTokens(char* cmdLine) {
    int count = 0;
    int i = 0;
    if (cmdLine == NULL) return 0;
    while (cmdLine[i] != '\0') {
        if (cmdLine[i] == ' ') { count++; }
        i++;
    }
    count++;
    return count;
}

void executeCommand(parseInfo* p_info) {
    if (!CheckCommand(p_info->tokens[0])) {
        freeInfo(p_info);
        exit(1);
    }
    check_pipe(p_info);

    // Redirection for "cat > file"
    if (strcmp(p_info->tokens[0], "cat") == 0 && p_info->tokens[1] && strcmp(p_info->tokens[1], ">") == 0) {
        cat(p_info);
    }

    // Redirection for "ls -l > file"
    if (strcmp(p_info->tokens[0], "ls") == 0 && p_info->tokens[1] && strcmp(p_info->tokens[1], "-l") == 0 &&
        p_info->tokens[2] && strcmp(p_info->tokens[2], ">") == 0) {
        int fd = open(p_info->tokens[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            exit(1);
        }
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(fd);
        char *new_args[] = {p_info->tokens[0], p_info->tokens[1], NULL};
        execvp(p_info->tokens[0], new_args);
        perror("execvp");
        exit(1);
    }

    // Handle other commands
    if (execvp(p_info->tokens[0], p_info->tokens) == -1) {
        perror("execvp");
    }
    freeInfo(p_info);
    exit(1);
}

bool CheckCommand(char* cmdLine) {
    if (!(strcmp(cmdLine, "pwd") == 0 || strcmp(cmdLine, "cd") == 0 || strcmp(cmdLine, "cat") == 0 ||
          strcmp(cmdLine, "wc") == 0 || strcmp(cmdLine, "cp") == 0 || strcmp(cmdLine, "clear") == 0 ||
          strcmp(cmdLine, "grep") == 0 || strcmp(cmdLine, "ls") == 0 || strcmp(cmdLine, "") == 0)) return false;
    return true;
}

void freeInfo(parseInfo *p_info) {
    if (p_info) {
        int i = 0;
        while (i < p_info->size) {
            if (p_info->tokens[i]) free(p_info->tokens[i]);
            i++;
        }
        if (p_info->tokens) free(p_info->tokens);
        free(p_info);
    }
}

void pwd() {
    char curr_path[MAX_L];
    getcwd(curr_path, MAX_L);
    printf("\033[0;35m%s \033[0m:\033[0;36m%s\033[0m:",getenv("USER"), curr_path);
}

int cd(parseInfo* p_info) {
    if (strcmp(p_info->tokens[0], "cd") == 0) {
        if (p_info->tokens[1] == NULL) {
            chdir(getenv("HOME"));
        } else {
            chdir(p_info->tokens[1]);
        }
        freeInfo(p_info);
        return 1;
    }
    return 0;
}

void cat(parseInfo *p_info) {
    FILE *file;
    char text[MAX_L];
    if (!p_info->tokens[2]) {
        printf("there is no file name");
        freeInfo(p_info);
        exit(1);
    }
    file = fopen(p_info->tokens[2], "w");
    if (!file) {
        printf("the file is open");
        freeInfo(p_info);
        exit(1);
    }
    while (fgets(text, MAX_L, stdin)) {
        fprintf(file, "%s", text);
    }
    fclose(file);
    freeInfo(p_info);
    exit(1);
}

int isPipe(parseInfo* p_info) {
    int i = 0;
    while (i < p_info->size) {
        if (strcmp(p_info->tokens[i], "|") == 0) return 1;
        i++;
    }
    return 0;
}

void check_pipe(parseInfo* p_info) {
    parseInfo *p1, *p2;
    int fds_arr[2], childPid;
    for (int i = 0; i < p_info->size; i++) {
        if (!strcmp(p_info->tokens[i], "|")) {
            p1 = (parseInfo*)malloc(sizeof(parseInfo));
            p2 = (parseInfo*)malloc(sizeof(parseInfo));
            p1->size = i;
            p2->size = p_info->size - i - 1;
            p1->tokens = (char**)malloc(sizeof(char*) * (p1->size + 1));

            for (int j = 0, idx1 = 0, idx2 = 0; j < p_info->size; j++) {
                if (j < i) p1->tokens[idx1++] = strdup(p_info->tokens[j]);
                if (j > i) p2->tokens[idx2++] = strdup(p_info->tokens[j]);
            }
            p1->tokens[p1->size] = NULL;
            p2->tokens[p2->size] = NULL;

            pipe(fds_arr);
            childPid = fork();
            if (childPid == 0) {
                close(fds_arr[READ]);
                dup2(fds_arr[WRITE], WRITE);
                executeCommand(p1);
            } else {
                waitpid(childPid, NULL, 0);
                close(fds_arr[WRITE]);
                dup2(fds_arr[READ], READ);
                if (!cd(p2)) executeCommand(p2);
            }
            freeInfo(p_info);
            exit(1);
        }
    }
}*/






#include "hw2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

parseInfo* parse(char* cmdLine) {
    int index = 0;
    int numTokens = countTokens(cmdLine);
    parseInfo* p_info = (parseInfo*)malloc(sizeof(parseInfo));
    p_info->tokens = (char**)malloc(sizeof(char*) * (numTokens + 1));
    if (strlen(cmdLine) == 0) {
        p_info->tokens[0] = strdup("");
        return p_info;
    }
    char* token = strtok(cmdLine, " ");
    if (strcmp(token, "exit") == 0) {
        printf("exit!");
        if (cmdLine) {
            free(cmdLine);
        }
        exit(1);
    }

    while (token != NULL) {
        p_info->tokens[index++] = strdup(token);
        token = strtok(NULL, " ");
    }
    p_info->size = index;
    p_info->tokens[index] = NULL;
    if (cmdLine) free(cmdLine);
    return p_info;
}

int countTokens(char* cmdLine) {
    int count = 0;
    int i = 0;
    if (cmdLine == NULL) return 0;
    while (cmdLine[i] != '\0') {
        if (cmdLine[i] == ' ') { count++; }
        i++;
    }
    count++;
    return count;
}

void executeCommand(parseInfo* p_info) {
    if (!CheckCommand(p_info->tokens[0])) {
        freeInfo(p_info);
        exit(1);
    }
    check_pipe(p_info);

    // Redirection for "cat > file"
    if (strcmp(p_info->tokens[0], "cat") == 0 && p_info->tokens[1] && strcmp(p_info->tokens[1], ">") == 0) {
        cat(p_info);
    }

    // Redirection for "ls -l > file"
    if (strcmp(p_info->tokens[0], "ls") == 0 && p_info->tokens[1] && strcmp(p_info->tokens[1], "-l") == 0 &&
        p_info->tokens[2] && strcmp(p_info->tokens[2], ">") == 0) {
        int fd = open(p_info->tokens[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            exit(1);
        }
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(fd);
        char *new_args[] = {p_info->tokens[0], p_info->tokens[1], NULL};
        execvp(p_info->tokens[0], new_args);
        perror("execvp");
        exit(1);
    }

    // Handle other commands
    if (execvp(p_info->tokens[0], p_info->tokens) == -1) {
        perror("execvp");
    }
    freeInfo(p_info);
    exit(1);
}

bool CheckCommand(char* cmdLine) {
    if (!(strcmp(cmdLine, "pwd") == 0 || strcmp(cmdLine, "cd") == 0 || strcmp(cmdLine, "cat") == 0 ||
          strcmp(cmdLine, "wc") == 0 || strcmp(cmdLine, "cp") == 0 || strcmp(cmdLine, "clear") == 0 ||
          strcmp(cmdLine, "grep") == 0 || strcmp(cmdLine, "ls") == 0 || strcmp(cmdLine, "") == 0)) return false;
    return true;
}

void freeInfo(parseInfo *p_info) {
    if (p_info) {
        int i = 0;
        while (i < p_info->size) {
            if (p_info->tokens[i]) free(p_info->tokens[i]);
            i++;
        }
        if (p_info->tokens) free(p_info->tokens);
        free(p_info);
    }
}

void pwd() {
    char curr_path[MAX_L];
    getcwd(curr_path, MAX_L);
    printf("\033[0;35m%s \033[0m:\033[0;36m%s\033[0m:",getenv("USER") , curr_path);
}

int cd(parseInfo* p_info) {
    if (strcmp(p_info->tokens[0], "cd") == 0) {
        if (p_info->tokens[1] == NULL) {
            chdir(getenv("HOME"));
        } else {
            chdir(p_info->tokens[1]);
        }
        freeInfo(p_info);
        return 1;
    }
    return 0;
}

void cat(parseInfo *p_info) {
    FILE *file;
    char text[MAX_L];
    if (!p_info->tokens[2]) {
        printf("there is no file name\n");
        freeInfo(p_info);
        exit(1);
    }
    file = fopen(p_info->tokens[2], "w");
    if (!file) {
        printf("the file cannot be opened\n");
        freeInfo(p_info);
        exit(1);
    }
    while (fgets(text, MAX_L, stdin)) {
        fprintf(file, "%s", text);
    }
    fclose(file);
    freeInfo(p_info);
    exit(1);
}

int isPipe(parseInfo* p_info) {
    int i = 0;
    while (i < p_info->size) {
        if (strcmp(p_info->tokens[i], "|") == 0) return 1;
        i++;
    }
    return 0;
}

void check_pipe(parseInfo* p_info) {
    parseInfo *p1, *p2;
    int fds_arr[2], childPid;
    for (int i = 0; i < p_info->size; i++) {
        if (!strcmp(p_info->tokens[i], "|")) {
            p1 = (parseInfo*)malloc(sizeof(parseInfo));
            p2 = (parseInfo*)malloc(sizeof(parseInfo));
            p1->size = i;
            p2->size = p_info->size - i - 1;
            p1->tokens = (char**)malloc(sizeof(char*) * (p1->size + 1));

            for (int j = 0, idx1 = 0, idx2 = 0; j < p_info->size; j++) {
                if (j < i) p1->tokens[idx1++] = strdup(p_info->tokens[j]);
                if (j > i) p2->tokens[idx2++] = strdup(p_info->tokens[j]);
            }
            p1->tokens[p1->size] = NULL;
            p2->tokens[p2->size] = NULL;

            pipe(fds_arr);
            childPid = fork();
            if (childPid == 0) {
                close(fds_arr[READ]);
                dup2(fds_arr[WRITE], WRITE);
                executeCommand(p1);
            } else {
                waitpid(childPid, NULL, 0);
                close(fds_arr[WRITE]);
                dup2(fds_arr[READ], READ);
                if (!cd(p2)) executeCommand(p2);
            }
            freeInfo(p_info);
            exit(1);
        }
    }
}
