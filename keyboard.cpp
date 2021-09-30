#include "keyboard.h"

Keyboard::Keyboard() {
#if defined (_WIN32)
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0; 
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);

#elif defined (__linux__)
    tcgetattr(0,&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
    peek_character=-1;
#endif
}

Keyboard::~Keyboard() {
#if defined (_WIN32)
	
#elif defined (__linux__)
    tcsetattr(0, TCSANOW, &initial_settings);
#endif
}

void Keyboard::get_terminal_size(int &width, int &height) {
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = (int)(csbi.dwSize.X);
    height = (int)(csbi.dwSize.Y);
#elif defined(__linux__)
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
    width = (int)(w.ws_col);
    height = (int)(w.ws_row);
#endif // Windows/Linux
}

int Keyboard::kbhit(){
#if defined (_WIN32)

#elif defined (__linux__)
    unsigned char ch;
    int nread;
    if (peek_character != -1) return 1;
    new_settings.c_cc[VMIN]=0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0,&ch,1);
    new_settings.c_cc[VMIN]=1;
    tcsetattr(0, TCSANOW, &new_settings);

    if (nread == 1){
        peek_character = ch;
        return 1;
    }
#endif
    return 0;
}

unsigned long Keyboard::getCurPosCursor()
{
    printf(ESC"[6n");
    fflush(stdout);
    int pos = 0;
    unsigned long save_upper = 0;
    char ch = ' ';
    while(ch != 'R') {
        ch = __getch();
        switch (ch) {
        case '[':
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            save_upper |= 0xFFFF0000 & pos;
            pos &= 0x0000FFFF;
            pos *= 10;
            pos += ch - 48;
            pos |= save_upper;
            break;
        case ';':
            pos <<= 16;
            break;
        }
    }
    return pos;
}

int Keyboard::__getch(){
    char ch;
#if defined (_WIN32)
    ch = getch();
    return ch;
#elif defined (__linux__)
    if (peek_character != -1){
        ch = peek_character;
        peek_character = -1;
    }
    else read(0,&ch,1);
#endif
    return ch;
}
