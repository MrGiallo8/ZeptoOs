#include "../includes/stout.h"
#include "../includes/time.h"
#include "../includes/keyboard.h"
#include "../includes/stdfuncs.h"

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

volatile char last_key = 0;

char get_key(void) {
    // Controlla il registro di stato della tastiera (porta 0x64)
    // Se il bit 0 è 1, c'è un byte pronto nella porta 0x60
    if (inb(0x64) & 0x01) {
        uint8_t scancode = inb(0x60);

        switch (scancode) {
            case 0x11: return 'w'; // Tasto W premuto
            case 0x1F: return 's'; // Tasto S premuto
            case 0x1E: return 'a'; // Tasto A premuto
            case 0x20: return 'd'; // Tasto D premuto
            default: return 0;
        }
    }
    return 0; // Nessun tasto premuto
}

void game_menu(){
    clear_screen();
    bordo();

    write_center("ZEPTO GAMES!",3, BLU_CHIARO);write("\n");
    write_center("0. Exit",6, BLU_CHIARO);
    write_center("1. NUMBER_GUESSER",7, BLU_CHIARO);
    write_center("2. SNAKE",8, BLU_CHIARO);

    char input[256];
    while (1){
        write("%g%k Games> ",3,12,BIANCO);
        kb_readline(input, 256, pputc);
        write("\n");
        int int_input = str_to_int(input);
        if(int_input == 1){ clear_screen(); number_guesser();return;}
        else if(int_input == 0){return;}
        else if(int_input == 2){clear_screen(); snake_game();return;}
        else { write("%g%k No game found... ", 3, 12, ROSSO);}
    }
}

void number_guesser(){
	for(int i=0;i<80;i++){
		write("%g%b ",i,0,GRIGIO);
	}

    write_center("[ Number Guesser ]",0,NERO);
    char input[256];
    write("%b%k\n\nWelcome to number guesser!\n",NERO,BIANCO);
    write("I will think of a number and you will have to guess it (1-99)!\n");
    uint64_t random_number = random_dig(2);
    write("Number generated !\n");

    int tentativi=0;
    while (1){
        write("Insert the number:");
        kb_readline(input, 256, pputc);
        tentativi++;
        int int_input = str_to_int(input);
        if(int_input > random_number ){
            write("%kLower than ",BIANCO); write(input);write("\n");
        }else if (int_input < random_number){
            write("%kHigher than ",BIANCO); write(input);write("\n");
        }else if(int_input == random_number){
            write("%kCongratulations! You won!",BIANCO);write("\n");
            write("%kAttempts = %d! ",BIANCO,tentativi);
            delay_s(3);
            return;
        }
    }
}

int find_coordx(int num){	
	return num % 80;
} 

int find_coordy(int num){
	return num / 80;
} 

int no_numbers[206];

void bordi_init(){
    
    int index = 0;


    // sopra

    for ( int i = 0 ; i< 80 ; i++){
        no_numbers[index] = i; index++;
    }

    // sotto
    for ( int i = 1920 ; i< (1920 + 80) ; i++){
        no_numbers[index] = i; index ++;
    }

    // lato sx

    for ( int i = 1 ; i < 24 ; i++){
        no_numbers[index] = 80 * i ; index++;
    }

    // lato sx

    for ( int i = 1; i<24 ; i++){
        no_numbers[index] = (80 * i) + 79; index++;
    }

}

bool check(int random_pos, int *snake, int snake_len){
	for (int i = 0; i < 206; i++) {
        if (random_pos == no_numbers[i]) {
            return false;
        }
    }

	return true;
}

int apple_spawn(int *snake, int snake_len){

	bool successo = false;
	int valid_num = 0;

	do{
		valid_num = random_between(0, 1999);
        successo = check(valid_num, snake, snake_len);
	}
	while(!successo);

	int x= find_coordx(valid_num);
	int y= find_coordy(valid_num);

	write("%g%k#", x, y, ROSSO);
    return valid_num;
}

void snake_game(){

    bordi_init();
    int snake[100];
    int snake_len = 3;
    
    // Posizione iniziale al centro 
    snake[0] = (12 * 80) + 40; // testa
    snake[1] = (12 * 80) + 39; // corpo
    snake[2] = (12 * 80) + 38; // coda

    // x : 1 dx, -1 sx, 0 ferma x
    // y : -1 su, 1 giu, 0 ferma y
    int dir_x = 1, dir_y = 0;   
    int mela_pos = 0;

    clear_screen();
    bordo();

    mela_pos = apple_spawn(snake, snake_len);

    write("%k%g0", VERDE, find_coordx(snake[0]), find_coordy(snake[0]));
    write("%k%g#", VERDE, find_coordx(snake[1]), find_coordy(snake[1]));
    write("%k%g#", VERDE, find_coordx(snake[2]), find_coordy(snake[2]));


    int points = 0;

    while(1){

        char key = get_key();

        if ( key == 'w' && dir_y != 1) { dir_x = 0; dir_y= -1; }
        if ( key == 's' && dir_y != -1) { dir_x = 0; dir_y= 1; }
        if ( key == 'd' && dir_x != -1) { dir_x = 1; dir_y= 0; }
        if ( key == 'a' && dir_x != 1) { dir_x = -1; dir_y= 0;}

        int tail_pos = snake[snake_len - 1];
        write("%g ",find_coordx(tail_pos), find_coordy(tail_pos));

        for (int i = snake_len - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }

        int new_x = find_coordx(snake[0]) + dir_x;
        int new_y = find_coordy(snake[0]) + dir_y;
        int new_head = (new_y * 80) + new_x;
        snake[0] = new_head;

        bool collition = false;

        //controllo sbatte bordi
        for(int i=0; i<206; i++){
            if(new_head == no_numbers[i]){
                collition = true;
                break;
            }
        }

        //controllo sbatte se stesso
        for (int i = 1; i < snake_len; i++) {
            if (new_head == snake[i]) {
                collition = true;
                break;
            }
        }


        if (collition) {
            
            delay_ms(1);
            break;
        }

        if (new_head == mela_pos) {
            if (snake_len < 100) {
                snake[snake_len] = tail_pos; 
                snake_len++;
            }
            delay_ms(50);
            points++;
            mela_pos = apple_spawn(snake, snake_len);
            
        }

        write("%g%k#",find_coordx(mela_pos), find_coordy(mela_pos), ROSSO);
        write("%g%k0",find_coordx(snake[0]), find_coordy(snake[0]), VERDE);

        delay_ms(150);
    }

    write_center("GAME OVER!",6,ROSSO);
    write_center("Points:",7,BIANCO); write("%k%d",VERDE_CHIARO,points);
    delay_s(5);
}