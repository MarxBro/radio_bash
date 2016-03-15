#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*                  Interface */
#include <curses.h>
#include <menu.h>

#define BUF 500 
#define TOT 500

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

/**********************************************************************************/
/* Esta función lee el archivo y devuelve un array para usar con las opciones de  */
/* ncurses                                                                        */
/**********************************************************************************/
/*char read_stations(void) {*/
/*char line[TOT][BUF];*/
/*FILE *rlist = NULL; */
/*int i = 0;*/
/*int total = 0;*/
/*rlist = fopen("STATIONS.txt", "r");*/
/*while(fgets(line[i], BUF, rlist)) {*/
/**//* get rid of ending \n from fgets */
/*line[i][strlen(line[i]) - 1] = '\0';*/
/*i++;*/
/*}*/
/*total = i;*/
/**//*for(i = 0; i < total; ++i)*/
/**//*printf("%s\n", line[i]);*/
/*return line;*/
/*}*/


char *choices[BUF];
char *descriptions[BUF];
char *uris[BUF];
void func(char *name);

int main(){	
    /*read file and save it to choices*/
    char line[TOT][BUF];
    FILE *rlist = NULL; 
    int igt = 0;
    int total = 0;
    rlist = fopen("STATIONS.txt", "r");
    while(fgets(line[igt], BUF, rlist)) {
        /* get rid of ending \n from fgets */
        line[igt][strlen(line[igt]) - 1] = '\0';
        choices[igt]        = line[igt];
        descriptions[igt]   = line[igt];
        uris[igt]           = line[igt];
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
	start_color();
        cbreak();
        noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
	/* Initialize items */
        n_choices = ARRAY_SIZE(choices);
        my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i) {
            my_items[i] = new_item(choices[i], choices[i]);
		    /* Set the user pointer */
		    set_item_userptr(my_items[i], func);
	    }
	my_items[n_choices] = (ITEM *)NULL;

	/* Create menu */
	my_menu = new_menu((ITEM **)my_items);

	/* Post the menu */
	mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
	mvprintw(LINES - 2, 0, "Up and Down arrow keys to naviage (F1 to Exit)");
	post_menu(my_menu);
	refresh();

	while((c = getch()) != KEY_F(1))
	{       switch(c)
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
				p = item_userptr(cur);
				p((char *)item_name(cur));
				pos_menu_cursor(my_menu);
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

void func(char *name) {	
    move(20, 0);
	clrtoeol();
	mvprintw(20, 0, "Item selected is : %s", name);
}	

