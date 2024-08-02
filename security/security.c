#include <stdio.h>
#include <string.h>
#include "security.h"

int check_xss(const char *request) {
    if (strstr(request, "<script>") != NULL) {
        // Add logger
        printf("Potential XSS detected\n");
        return 1;
    }
    
    return 0;
}

int check_sql_injection(const char *request) {
    if (strstr(request, "DROP TABLE") != NULL) {
        printf("Possible SQL Injection detected\n");
        return 1;
    }
    return 0;
}

int check_security(const char *request) {
    if (check_xss(request) || check_sql_injection(request)) {
        return 1;
    }
    return 0;
}