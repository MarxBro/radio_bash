/*#include <stdio.h>*/
#include <stdlib.h>
#include <string.h>
/*interface*/
#include <curses.h>
#include <menu.h>
/*processes*/
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#define BUF 500 
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

int terminal_max_x;
int terminal_max_y;

int cuenta = 0;
int *contador = &cuenta;

char *choices[BUF];
char *descriptions[BUF];
char *uris[BUF];
char *description_fn ;
char *radio_current;

pid_t pid;

void play_radio(void);
void kill_child(void);
void func(char *name);

int main(){	
    char line[BUF][BUF];
    FILE *rlist = NULL; 
    int igt     = 0;
    int total   = 0;
    rlist       = fopen("STATIONS.txt", "r");
    while( fgets ( line[igt], BUF, rlist )) {
        /* get rid of ending \n from fgets */
        line[igt][strlen(line[igt]) - 1] = '\0';
        char *line_f        = line[igt];
        uris[igt]           = strtok(line_f,    "@");
        choices[igt]        = strtok(NULL,      "@");
        descriptions[igt]   = strtok(NULL,      "@");
        igt++;
    }
    
	/* Initialize curses */	
    ITEM **my_items;
	int c;				
	MENU *my_menu;
        int n_choices, i;
	ITEM *cur_item;
	initscr();
    /*We dont really need colors for now...*/
    /*start_color();*/
        cbreak();
        noecho();
	keypad(stdscr, TRUE);
    int max_x;
    int max_y;
    getmaxyx(stdscr,max_x,max_y);
    /*init_pair(1, COLOR_RED,COLOR_WHITE);*/
    /*init_pair(2, COLOR_GREEN,COLOR_WHITE);*/
    /*init_pair(3, COLOR_MAGENTA,COLOR_WHITE);*/
	/* Initialize items */
        n_choices = ARRAY_SIZE(choices);
        my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i) {
            my_items[i] = new_item(choices[i],descriptions[i]);
		    /* Set the user pointer */
		    set_item_userptr(my_items[i], func);
	    }
	my_items[n_choices] = (ITEM *)NULL;

	/* Create menu */
	my_menu = new_menu((ITEM **)my_items);
    set_menu_opts(my_menu,O_SHOWDESC);

	/* Post the menu */
	mvprintw(LINES - 3, 0, "Press <ENTER> to play the station,");
	mvprintw(LINES - 2, 0, "<arrows> to move the menu, <Q> to Exit or");
	mvprintw(LINES - 1, 0, "<K> to kill all mplayer ongoing processes.");
	post_menu(my_menu);
	refresh();

    /*big loop thingy*/
    while((c = getch()) != 113){
        switch(c){
            case KEY_DOWN:
                menu_driver(my_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(my_menu, REQ_UP_ITEM);
                break;
            case KEY_LEFT:
                menu_driver(my_menu, REQ_SCR_UPAGE);
                break;
            case KEY_RIGHT:
                menu_driver(my_menu, REQ_SCR_DPAGE);
                break;
            case 107:   // k
                kill_child();
                break;
            case 10: {    /* Enter  == Play some radio! */
                ITEM *cur;
                void (*p)(char *);
                if (*contador > 1){
                    kill_child();
                }
                cur = current_item(my_menu);
                description_fn = (char *)item_description(cur);
                p = item_userptr(cur);
                p((char *)item_name(cur));
                pos_menu_cursor(my_menu);
                play_radio();
                cuenta = cuenta + 1;
                break;
            }
            break;
        }
    }
    /*That's all, free memory, kill any pid and exit.*/
    unpost_menu(my_menu);
    for(i = 0; i < n_choices; ++i){
        free_item(my_items[i]);
    }
    free_menu(my_menu);
    endwin();
    kill_child();
    exit(0);
}

void func(char *name){
    move(20, 0);
	clrtoeol();
    int elementos = sizeof (choices) / sizeof (choices[0]);
    int indice;
    /*Encontrar el indice del elemento en el array*/
    for(indice = 0; indice < sizeof(choices) / sizeof(choices[0]); indice++) {
        if (strcmp(choices[indice],name)==0){
            break;
        }
    }
    radio_current = uris[indice];
    /*Some terminals may leave a trace of previous screen texts,*/
    /*this just put a line fill of spaces before writing new stuffs in...*/
    /*Silly clean-up.*/
    mvprintw(20, 0, "%-100s", " ");
    mvprintw(22, 0, "%-100s", " ");
    mvprintw(25, 0, "%-100s", " ");
    mvprintw(27, 0, "%-100s", " ");
    mvprintw(29, 0, "%-100s", " ");
    mvprintw(30, 0, "%-100s", " ");
    mvprintw(31, 0, "%-100s", " ");
	mvprintw(20, 0, "NOW PLAYING : %-3s", name);
	mvprintw(22, 0, "%s", "-------------------------");
    mvprintw(25, 0, "%s", description_fn);
    mvprintw(27, 0, "%s", uris[indice]);
    mvprintw(29, 0, "%i", indice+1);
}	
/*Play the radio as a forked process,*/
/*detach it from any tty and throw away any output.*/
void play_radio (void){
    char comando[200]; 
    snprintf(comando, sizeof comando, "mplayer %s >/dev/null 2>/dev/null &", radio_current);
    if ((pid = fork()) == 0){
        system(comando);
        exit(0);
    }
}
/*This function kills child pid */
/*and all mplayer instances...*/
void kill_child(void){
    kill(pid,SIGTERM);
    system("killall mplayer > /dev/null 2> /dev/null");
    wait(NULL);
}
