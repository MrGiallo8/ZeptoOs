#include "stout.h"
#include "keyboard.h"
#include "time.h"

void shell_run(){
	clear_screen();
	char input[256];
	write("\n%k=====================================%k[SHELL]%k====================================",BIANCO,VERDE,BIANCO);
	write("\n");
	while(1){
		write("%k> ",BIANCO);
		kb_readline(input, 256 ,pputc);
		write("\n");

		if(strcmp(input, "help") == 0){
			write("%k help   - show commands\n",BIANCO);
			write("%k clear  - clear the screen\n",BIANCO);
			write("%k quit   - exit the shell\n",BIANCO);
			write("%k echo   - print the following string\n\n",BIANCO);
		}
		else if (strcmp(input, "clear") == 0){
			clear_screen();
			write("\n%k=====================================%k[SHELL]%k====================================",BIANCO,VERDE,BIANCO);
			write("\n");
		}
		else if (strcmp(input, "quit") == 0){
			write("%kexiting the shell...",BIANCO);
			delay_s(1);
			return;
		}
		else if (strcmp_n(input, "echo ",5) == 0){
			write( input + 5 );
			write("\n\n");
		}
		else if(strcmp(input, "") == 0){
			// Nulla
		}
		else {
			write("%k Unknown command:",ROSSO);
			write(input);
			write("\n");
		}
		
	}
	
}

#define MAX_LINES 64
#define MAX_LINE_LEN 256

char editor_buffer[MAX_LINES][MAX_LINE_LEN];
int  editor_line_count = 0;

void text_editor() {
    clear_screen();
    char input[MAX_LINE_LEN];
    int line = 0;

    write("\n%k====================================%k[EDITOR]%k====================================\n",
          BIANCO, VERDE, BIANCO);

    // Mostra il contenuto già salvato (se si riapre)
    if (editor_line_count > 0) {
        write("%k-- contenuto salvato --%k\n", GIALLO, BIANCO);
        for (int i = 0; i < editor_line_count; i++) {
            write("  %d: %s\n", i + 1, editor_buffer[i]);
        }
        write("%k-- fine -- continua a scrivere, ***quit per uscire --%k\n", GIALLO, BIANCO);
        line = editor_line_count;  // continua dall'ultima riga 
    }

    while (1) {
        kb_readline(input, MAX_LINE_LEN, pputc);

        if (strcmp(input, "***quit") == 0) {
            // Salva in RAM
            editor_line_count = line;
            return;

        } else if (strcmp(input, "***show") == 0) {
            // Mostra il testo scritto fino ad ora
            for (int i = 0; i < line; i++) {
                write("  %d: %s\n", i + 1, editor_buffer[i]);
            }

        } else if (strcmp(input, "***clear") == 0) {
            // Svuota il buffer
            line = 0;
            editor_line_count = 0;
            write("%k[buffer svuotato]%k\n", ROSSO, BIANCO);

        } else if (line < MAX_LINES) {
            // Salva la riga nel buffer
            int i;
            for (i = 0; input[i] && i < MAX_LINE_LEN - 1; i++)
                editor_buffer[line][i] = input[i];
            editor_buffer[line][i] = '\0';
            line++;
        } else {
            write("%k[buffer pieno!]%k\n", ROSSO, BIANCO);
        }
    }
}

void terminal_run(){
	char input[256];
	clear_screen();
    write_center("ZeptoOS", 1, BLU_CHIARO);
    write_center("v0.2.0",  3, VERDE_CHIARO);
    write("\n\n%k================================================================================",BIANCO);
	write("\n");
	pos_char = 5 * 80;
    while (1) {
		write("%k\n\nZeptoOS > ",BIANCO);
		kb_readline(input, 256, pputc);
		write("\n");
		    
		if (strcmp(input, "open shell") == 0) {
		clear_screen();
		shell_run();
		// quando shell esce tornia qui
		clear_screen();
		write_center("ZeptoOS", 1, BLU_CHIARO);
    	write_center("v0.2.0",  3, VERDE_CHIARO);
    	write("\n\n%k================================================================================",BIANCO);
		} else if (strcmp(input, "open editor") == 0) {
			text_editor();
			clear_screen();
			write_center("ZeptoOS", 1, BLU_CHIARO);
    		write_center("v0.2.0",  3, VERDE_CHIARO);
    		write("\n\n%k================================================================================",BIANCO);
		} else {
		write("%kUnknown Command\n",ROSSO);
		}
	}
	
}

