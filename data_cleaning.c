//  data_cleaning.c
//  COP4610_Assignment1
//
//  Group Memebers: Jowayne Hudson
// TODO: Validate all six fields and reject any malformed record silently.
// TODO: Print the exact required CSV header and valid student rows to standard output.
// TODO: Create the Makefile with all, clean, and rebuild targets using the required gcc flags.
// TODO: Test input1.txt through input5.txt and compare output with diff -BZ.
// TODO: Complete README.txt with team info, AI disclosure, and AI critique.
//

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 512

typedef struct {
    char name[100];
    char pid[20];
    char email[100];
    char phone[20];
    char department[150];
    char year[20];
} Student;

//removes whitespaces from beginning and end of a string
void trim(char *str) {
    char *start = str;
    
    //moves pointer forward to first non whitespace
    while (isspace((unsigned char)*start)) {
        start++;
    }
    
    //shifts string left
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    int len = (int)strlen(str);
    
    // moves pointer backwards to first non whitespace
    while (len > 0 && isspace((unsigned char)str[len-1])) {
        str[len - 1] = '\0';
        len--;
    }
}

int is_separator(const char *line) {
    if (line[0] == '\0') {
        return 0;
    }
    
    char separator = line[0];
    
    if (separator != '=' && separator != '-') {
        return 0;
    }
    
    for (int i = 1; line[i] != '\0'; i++) {
        if (line[i] != separator) {
            return 0;
        }
    }
    
    return 1;
}

//FIELD PARSERS

// will be first and last, just grab 2 words after the label
void parse_name(char *line, Student *student) {
    char first_name[50];
    char last_name[50];
    
    char *name_position = strstr(line, "Name:");
    
    if (name_position != NULL) {
        name_position += strlen("Name:");
        
        while (isspace((unsigned char)*name_position)) {
            name_position++;
        }
        
        //reads only the first 2 words and stores them into the name field of the student struct
        if (sscanf(name_position, "%49s %49s", first_name, last_name) == 2) {
            snprintf(student -> name, sizeof(student->name),
                     "%s %s",
                     first_name,
                     last_name);
        }
    }
}
// PID, Year, Phone and email all follow the same generic rule, grabs first token after the label
void parse_field(char *line, const char *field_label, char *field, int field_size) {
    
    char *field_position = strstr(line, field_label);
    
    if (field_position != NULL) {
        //move past the field label
        field_position += strlen(field_label);
        
        //skip whitespace before the value
        while(isspace((unsigned char)* field_position)) {
            field_position++;
        }
        
        char format[20];
        snprintf(format, sizeof(format), "%%%ds", field_size - 1);
        
        sscanf(field_position, format, field);
    }
}

void parse_department(char *line, Student *student) {
    char *dept_position = strstr(line, "Department:");
    char copy[MAX_LINE]; // later use of strtok(), this allows manipulation of the line without destroying it
    char *word;
    
    if (dept_position == NULL) {
        return;
    }
    
    dept_position += strlen("Department:");
    
    while (isspace((unsigned char)*dept_position)) {
        dept_position++;
    }
    
    strcpy(copy, dept_position);
    word = strtok(copy, " \t");
    
    // loops until we don't have another word or we meet another label
    // otherwise, appends to the student's department field
    while (word != NULL) {
        if (strcmp(word, "Name:") == 0 ||
            strcmp(word, "PID:") == 0 ||
            strcmp(word, "Email:") == 0 ||
            strcmp(word, "Phone:") == 0 ||
            strcmp(word, "Year:") == 0) {
            break;
        }
        
        if (student->department[0] != '\0') {
            strcat(student->department, " ");
        }
        
        strcat(student->department, word);
        
        word = strtok(NULL, " \t");
    }
}

int main(int argc, char *argv[]) {
    FILE *file;
    char line[MAX_LINE];
    Student student = {0};
    
    if (argc != 2) {
        return 1;
    }
    
    //open file
    file = fopen(argv[1], "r");
    
    if (file == NULL) {
        return 1;
    }
    
    while (fgets(line, sizeof(line), file) != NULL) {
        trim(line);
        
        // ignores blank lines
        if (line[0] == '\0') {
            continue;
        }
        
        //ignores file heading
        if (strcmp(line, "STUDENT RECORDS") == 0){
            continue;
        }
        
        if (is_separator(line)) {
            printf("[Name: %s, PID: %s, Email: %s, Phone: %s, Year: %s, Department: %s]\n"
                   , student.name, student.pid, student.email, student.phone, student.year, student.department);
            memset(&student, 0, sizeof(student));
            continue;
        }
        
        parse_name(line, &student);
        parse_field(line, "PID:", student.pid, sizeof(student.pid));
        parse_field(line, "Email:", student.email, sizeof(student.email));
        parse_field(line, "Phone:", student.phone, sizeof(student.phone));
        parse_field(line, "Year:", student.year, sizeof(student.year));
        parse_department(line, &student);
        
    }
    
    fclose(file);
    
    return 0;
}
