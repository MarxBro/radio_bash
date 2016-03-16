#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*                  Interface */
#include <curses.h>
#include <menu.h>
/*                  para reproducir*/
#include <sys/types.h>
#include <signal.h>

#define BUF 500 

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4



char *choices[BUF];
char *descriptions[BUF];
char *uris[BUF];
void func(char *name);
void play_radio(void);
char *description_fn ;
int terminal_max_x;
int terminal_max_y;
/*int pid;*/
char *radio_current;

int main(){	
    /*read file and save it to choices*/
    char line[BUF][BUF];
    FILE *rlist = NULL; 
    int igt = 0;
    int total = 0;
    rlist = fopen("STATIONS.txt", "r");
    while(fgets(line[igt], BUF, rlist)) {
        /* get rid of ending \n from fgets */
        line[igt][strlen(line[igt]) - 1] = '\0';
        char *line_f        = line[igt];
        uris[igt]           = strtok(line_f, "@");
        choices[igt]        = strtok(NULL, "@");
        descriptions[igt]   = strtok(NULL,   "@");
        /*printf("%s",choices[igt]);*/
        igt++;
    }
    /*-----------------------------------*/
    
    ITEM **my_items;
	int c;				
	MENU *my_menu;
        int n_choices, i;
	ITEM *cur_item;
	
	/* Initialize curses */	
	initscr();
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
	mvprintw(LINES - 3, 0, "Press <ENTER> to play the station");
	mvprintw(LINES - 2, 0, "Up and Down arrow keys to search and Q to Exit");
	post_menu(my_menu);
	refresh();

	while((c = getch()) != 113){
	    switch(c)
	        {	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case 10: /* Enter */
			{	ITEM *cur;
				void (*p)(char *);

				cur = current_item(my_menu);
                description_fn = item_description(cur);
				p = item_userptr(cur);
				p((char *)item_name(cur));
				pos_menu_cursor(my_menu);
                play_radio();
				break;
			}
			break;
		}
	}	
	unpost_menu(my_menu);
	for(i = 0; i < n_choices; ++i)
		free_item(my_items[i]);
	free_menu(my_menu);
	endwin();
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
    /*Borrar la gilada previa!*/
    int relleno = terminal_max_y - 2;
    mvprintw(20, 0, "%-100s", " ");
    mvprintw(22, 0, "%-100s", " ");
    mvprintw(25, 0, "%-100s", " ");
    mvprintw(27, 0, "%-100s", " ");
    mvprintw(29, 0, "%-100s", " ");
	mvprintw(20, 0, "NOW PLAYING : %-3s", name);
	mvprintw(22, 0, "%s", "-------------------------");
    mvprintw(25, 0, "%s", description_fn);
    mvprintw(27, 0, "%s", uris[indice]);
    mvprintw(29, 0, "%i", indice+1);
}	

void play_radio (void){
    char comando[200]; 
    snprintf(comando, sizeof comando, "mplayer %s >/dev/null 2>&1", radio_current);
    mvprintw(31, 0, "%s", radio_current);
    /*system(comando);*/
}
