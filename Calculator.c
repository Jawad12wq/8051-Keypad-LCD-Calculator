#include <reg51.h>
#include <stdio.h>
#include <string.h>  // Include for strcat and strlen

sbit RS = P3^7;
sbit E = P3^6;

sbit RowA = P1^0;
sbit RowB = P1^1;
sbit RowC = P1^2;
sbit RowD = P1^3;

sbit C1 = P1^4;
sbit C2 = P1^5;
sbit C3 = P1^6;
sbit C4 = P1^7;

void cct_init(void);
void delay(int);
char READ_SWITCHES(void);
char get_key(void);
int get_num(char);
void disp_string(char*);
void Lcdinit(void);
void writecmd(int);
void writedata(char);
char get_operator(char);

void cct_init(void){
    P0 = 0x00;
    P1 = 0xFF;
    P2 = 0x00;
    P3 = 0x00;
}

char READ_SWITCHES(void){
    RowA = 0; RowB = 1; RowC = 1; RowD = 1;
    if(C1 == 0) { delay(10000); while (C1 == 0); return '7';}
    if(C2 == 0) { delay(10000); while (C2 == 0); return '8';}
    if(C3 == 0) { delay(10000); while (C3 == 0); return '9';}
    if(C4 == 0) { delay(10000); while (C4 == 0); return '/';}
    
    RowA = 1; RowB = 0; RowC = 1; RowD = 1;
    if(C1 == 0) { delay(10000); while (C1 == 0); return '4';}
    if(C2 == 0) { delay(10000); while (C2 == 0); return '5';}
    if(C3 == 0) { delay(10000); while (C3 == 0); return '6';}
    if(C4 == 0) { delay(10000); while (C4 == 0); return '*';}
    
    RowA = 1; RowB = 1; RowC = 0; RowD = 1;
    if(C1 == 0) { delay(10000); while (C1 == 0); return '1';}
    if(C2 == 0) { delay(10000); while (C2 == 0); return '2';}
    if(C3 == 0) { delay(10000); while (C3 == 0); return '3';}
    if(C4 == 0) { delay(10000); while (C4 == 0); return '-';}
    
    RowA = 1; RowB = 1; RowC = 1; RowD = 0;
    if(C1 == 0) { delay(10000); while (C1 == 0); return 'C';}
    if(C2 == 0) { delay(10000); while (C2 == 0); return '0';}
    if(C3 == 0) { delay(10000); while (C3 == 0); return '=';}
    if(C4 == 0) { delay(10000); while (C4 == 0); return '+';}
    
    return 'n';
}

char get_operator(char ch){
    switch(ch){
        case '+' : return '+'; 
        case '-' : return '-'; 
        case '*' : return '*'; 
        case '/' : return '/'; 
        default  : return '\0'; 
    }
}

char get_key(void){
    char key = 'n';
    while(key == 'n'){
        key = READ_SWITCHES();
    }
    return key;
}

int get_num(char ch){
    switch(ch){
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        default : return -1;
    }
}

void disp_string(char *str){
    while(*str){
        writedata(*str++);
    }
}

void delay(int x){
    unsigned int j;
    for(j=0; j<x; j++){}
}

void writedata(char t){
    RS = 1;
    P2 = t;
    E = 1;
    delay(100);
    E = 0;
    delay(100);
}

void writecmd(int z){
    RS = 0;
    P2 = z;
    E = 1;
    delay(100);
    E = 0;
    delay(100);
}

void Lcdinit(void){
    delay(1500);
    writecmd(0x38);  // 8-bit mode with 2 lines
    writecmd(0x0C);  // Display on, cursor off
    writecmd(0x06);  // Increment cursor
    writecmd(0x01);  // Clear display
    delay(1500);
}

int main(void){
    char key;
    int num1 = 0, num2 = 0, result = 0;
    char operators = '\0';
    char expression[16] = "";
    int expr_index = 0;
    char buffer[16];

    cct_init();
    Lcdinit();
    
    while(1){
        key = get_key();
        
        if(key == 'C'){
            writecmd(0x01); // Clear display
            delay(1000);
            num1 = num2 = result = 0;
            operators = '\0';
            expr_index = 0;
            expression[0] = '\0';
        }
        else if(key == '='){
            if (operators != '\0') {
                switch(operators){
                    case '+' :
                        result = num1 + num2;
                        break;
                    case '-' :
                        result = num1 - num2;
                        break;
                    case '*' :
                        result = num1 * num2;
                        break;
                    case '/' :
                        if(num2 != 0)
                            result = num1 / num2;
                        else 
                            result = 0;  // Handle divide by zero
                        break;
                }
                sprintf(buffer, "=%d", result);
                strcat(expression, buffer);
                writecmd(0x01); // Clear display
                delay(1000);
                disp_string(expression);
                delay(1000);
                num1 = result; // Assign the result to num1 for further calculations
                num2 = 0;      // Reset num2
                operators = '\0'; // Reset the operator
                expr_index = 0;
                expression[0] = '\0';
                sprintf(expression, "%d", num1);
                expr_index = strlen(expression);
            }
        }
        else {
            if(key >= '0' && key <= '9') {
                if(operators == '\0') {
                    num1 = num1 * 10 + get_num(key);
                } else {
                    num2 = num2 * 10 + get_num(key);
                }
                expression[expr_index++] = key;
                expression[expr_index] = '\0';
                writedata(key);
            }
            else if(key == '+' || key == '-' || key == '*' || key == '/'){
                if (operators == '\0') {
                    operators = get_operator(key);
                    expression[expr_index++] = key;
                    expression[expr_index] = '\0';
                    writedata(key);
                }
            }
        }    
    }
}
