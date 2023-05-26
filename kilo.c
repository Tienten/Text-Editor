// includes

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

// data

struct termios orig_termios;     // global variable. struct is a collection of dat and the members are allocated sequentially

 // low-level terminla input

void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {  //get us into raw mode
    // canonical mode: keyboard input is sent to the propram after hitting Enter, but text editor want raw mode where it process each keypress as it comes in.
    //struct termios, tcgetattr(), tcsetattr(), ECHO, and TCSAFLUSH come from <termios.h>
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");      // terminal attributes can be read into the termios struct by tcgetattr()
    atexit(disableRawMode); //atexit() comes from <stdlib.h>. It is used to register the disableRawMode() to be called automatically when the program exits
    struct termios raw = orig_termios; //assign orig_termios struct to the raw struct to make a copy before making any chages
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); //IXON and ICRNL comes from <termios.h>. The I stands for "input flag" and XON comes from the names of the two control characters that Ctrl-S and Ctrl_Q produce: XOFF to pause transmission and XON to resume transmission. In ICRNL, CR stands for "carriage return", and NL stands for "new line"
    raw.c_oflag &= ~(OPOST);    //OPOST comes from <termios.h>. O means it's an output flag
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);     // c_lflag is for "local flags". ECHO causes each key you type to be printed to the terminal, so you can see what you are typing. For raw mode, we will turn it off since we will render a user interface. It just doesn't print out what you are typing. ICANON comes from <termios.h>. ICANON is a local flag in the c_lflag field. This flag turns off canonical mode. ISIG is similar to ICANON, and is used to turn off the Ctrl-C and Ctrl-Z signals. Ctrl-V asks the terminal waits for you to type another character and then sends that character littlerally, IEXTEN flag turns off this feature and also Ctrl-O in MacOS
    raw.c_cc[VMIN] = 0; //sets the minimum number of bytes of input needed before read() can return, which is 0
    raw.c_cc[VTIME] = 1; //sets the maximum amount of time to wait before read() returns, which is in tenths of a second, so we set it to 1/10 of a second, or 100 milisecond
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");   // AFter modifying the terminal attributes, they can be applyied to the terminal using tcsetattr(). the TCSAFLUSH argument specifies when to apply the change: in this func, it waits for all pending output to be written to the terminal, and also discards any input that hasn't been read.
}

char editorReadKey() {  //wait for keypress and return 
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && erno != EAGAIN) die("read");
    }
    return c;
}

// high-level input

void editorProcessKeypress() { //wait for keypress and handles
    char c = editorReadKey();
    switch (c) {
        case CTRL_KEY('q'):
            exit(0);
            break;
    }
}

// init

int main() {
    enableRawMode();

    
    while (1) {     
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");    //read() and STDIN_FILENO come from <unistd.h>. asking read() to keep reading 1 byte from the standard input into the variable c until there is no more byte to read. read() returns the number of byte that it rwad and will return 0 at the end
        if (iscntrl(c)) {   //iscntrl() comes from <ctype.h>. It test whether a char is a control char. Control characters are nonprintable characters that we don't want to print to the screen. ASCII codes 0-31 and 127 are control characters, while codes 32-126 are printable
            printf("%d\r\n", c);  //printf() comes from <stdio.h>. %d tells it to format the byte as a decimal number, and %c tells it to write out the byte directly, as a character
        } else {
            printf("%d ('%c')\r\n", c, c);  // \r carriage return
        }
        if (c == CTRL_KEY('q')) break;   
    }
    
    return 0;   //a return of value 0 means success
}





