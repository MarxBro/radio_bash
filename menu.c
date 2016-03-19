#include <stdlib.h>
#include <string.h>
/*interface*/
#include <menu.h> // includes stdio.h
/*processes*/
#include <signal.h>
#include <unistd.h>

#define BUF 500 
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

int y_lines_reserved = 10 + 1;                  // Reserve the last 10 lines.

int cuenta = 0;
int *contador = &cuenta;

char *choices[BUF];
char *descriptions[BUF];
char *uris[BUF];
char *description_fn ;
char *radio_current;

pid_t pid;
pid_t main_pid;

void play_radio(void);
void kill_child(void);
void func(char *name);
void hacete_una_linea_putin(int offset);

int main(){	
    main_pid = getpid();
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
    /*wborder(0 , 0,0,0,0,0,0,0,0,);*/
    /*init_pair(1, COLOR_RED,COLOR_WHITE);*/
    /*init_pair(2, COLOR_GREEN,COLOR_WHITE);*/
    /*init_pair(3, COLOR_MAGENTA,COLOR_WHITE);*/
	/* Initialize items */
        n_choices = ARRAY_SIZE(choices);
        my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i) {
            my_items[i] = new_item(choices[i],descriptions[i]);
		    /* The F*cking user pointer */
		    set_item_userptr(my_items[i], func);
	    }
	my_items[n_choices] = (ITEM *)NULL;

	/* Create menu */
	my_menu = new_menu((ITEM **)my_items);
    set_menu_opts(my_menu,O_SHOWDESC);
    int y_lines_menu = LINES - y_lines_reserved;
    set_menu_format(my_menu,y_lines_menu,1);

	/* Post the menu */
    /*Help thingy*/
    hacete_una_linea_putin(5);
	mvprintw(LINES - 4, 0, " Press <ENTER> to play the station,");
	mvprintw(LINES - 3, 0, " any <arrow> to move the menu buffer, <Q> to Quit or");
	mvprintw(LINES - 2, 0, " <K> to Kill child mplayer process.");
    hacete_una_linea_putin(1);
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
                if (*contador >= 1){
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
    /*clean up */
    for (int asd=0; asd<= 11; asd++){
        move(LINES - asd, 0);
        clrtoeol();
    }
    int elementos = sizeof (choices) / sizeof (choices[0]);
    int indice;
    for(indice = 0; indice < sizeof(choices) / sizeof(choices[0]); indice++) {
        if (strcmp(choices[indice],name)==0){
            break;
        }
    }
    radio_current = uris[indice];
    hacete_una_linea_putin(10);
	mvprintw(LINES -9 , 0, "Now Playing   %s", name);
    mvprintw(LINES -8 , 0, "%s", description_fn);
    mvprintw(LINES -7 , 0, "Url   %s", uris[indice]);
    /*Flechas macristas (a la derecha)*/
    mvaddch(LINES -9, 12,ACS_RARROW);
    mvaddch(LINES -7, 4, ACS_RARROW);
    /*Help thingy*/
    hacete_una_linea_putin(5);
	mvprintw(LINES - 4, 0, " Press <ENTER> to play the station,");
	mvprintw(LINES - 3, 0, " any <arrow> to move the menu buffer, <Q> to Quit or");
	mvprintw(LINES - 2, 0, " <K> to Kill child mplayer process.");
    hacete_una_linea_putin(1);
}	
/*Play the radio as a forked process,*/
void play_radio (void){
    char comando[200]; 
    snprintf(comando, sizeof comando, "mplayer %s -really-quiet 2>/dev/null &", radio_current);
    if ((pid = fork()) == 0){
        system(comando);
        pid = getpid();
        exit (0);
    }
}
/*This function kills child pid */
/*and all mplayer instances... Still not happy with this*/
void kill_child(void){
    if (pid != main_pid){
        kill(pid,SIGKILL);
    }
    system("killall mplayer 2>/dev/null");
}
void hacete_una_linea_putin(int offset){
    for (int py = 0; py <= 80; py++){
        mvaddch(LINES - offset,py,ACS_HLINE);
    }
}
