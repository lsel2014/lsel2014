/**
 * @file    gui.c
 * @date    2014-06-22
 * @brief   Train scale model Graphical User Interface
 *
 * GUI for our train scale model
 */

#include <ncurses.h>
#include <stdlib.h>


#define ENTER 10 
#define ESCAPE 27 

/**
 * Ncurses initialization
 */
WINDOW* ncurses_init(int* max_lines, int* max_cols)
{
    WINDOW* window;

    /*
     * Starting up
     */
    if ((window = initscr()) == NULL)
    {
        fprintf(stderr, "Error initializing ncurses");
        exit(1);
    }
    cbreak();
    noecho();
    keypad(window, TRUE);

    /*
     * Life in color
     */
    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        init_pair(2, COLOR_BLUE,  COLOR_WHITE);
        init_pair(3, COLOR_RED,   COLOR_WHITE);
    }

    /*
     * Get boundaries
     */
    getmaxyx(window, *max_lines, *max_cols);
    wrefresh(window);
    
    return window;
}

/**
 * Ncurses termination
 */
void ncurses_stop()
{
    endwin();
}

void window_menubar(WINDOW* menubar)
{
    wbkgd(menubar,COLOR_PAIR(2)); 
    
    waddstr(menubar," Trenes"); 
    wattron(menubar,COLOR_PAIR(3)); 
    waddstr(menubar,"(1)"); 
    wattroff(menubar,COLOR_PAIR(3)); 
    wmove(menubar,0,21); 
    waddstr(menubar,"Cambio de vía"); 
    wattron(menubar,COLOR_PAIR(3)); 
    waddstr(menubar,"(2)"); 
    wattroff(menubar,COLOR_PAIR(3));
    wmove(menubar,0,51); 
    waddstr(menubar,"Barrera"); 
    wattron(menubar,COLOR_PAIR(3)); 
    waddstr(menubar,"(3)"); 
    wattroff(menubar,COLOR_PAIR(3));
}

WINDOW **draw_menu(int start_col, const char* s_items[], int n_items) 
{ 
    int i; 
    WINDOW **items; 
    items=(WINDOW **)malloc((n_items+1)*sizeof(WINDOW *)); 

    items[0]=newwin(2 + n_items,19,1,start_col); 
    wbkgd(items[0],COLOR_PAIR(2)); 
    box(items[0],ACS_VLINE,ACS_HLINE);
    
    for(i = 1; i < n_items+1; ++i)
    {
        items[i]=subwin(items[0],1,17,i+1,start_col+1); 
        wprintw(items[i], s_items[i-1]); 
    }

    wbkgd(items[1],COLOR_PAIR(1)); 
    wrefresh(items[0]); 

    return items;
} 

void delete_menu(WINDOW **items,int count) 
{ 
    int i; 
    for (i=0;i<count+1;i++)
        delwin(items[i]);
    free(items); 
}

int scroll_menu(WINDOW **items,int count,int menu_start_col) 
{ 
    int key; 
    int selected=0; 
    while (1)
    { 
        key=getch(); 
        if (key==KEY_DOWN || key==KEY_UP)
        { 
            wbkgd(items[selected+1],COLOR_PAIR(2)); 
            wnoutrefresh(items[selected+1]); 
            
            if (key==KEY_DOWN)
            { 
                selected=(selected+1) % count; 
            }
            else 
            { 
                selected=(selected+count-1) % count; 
            } 

            wbkgd(items[selected+1],COLOR_PAIR(1)); 
            wnoutrefresh(items[selected+1]); 
            doupdate(); 
        }
        else if (key==KEY_LEFT || key==KEY_RIGHT || key==' ')
        { 
            return -1; 
        } else if (key==ESCAPE) { 
            return -1; 
        } else if (key==ENTER) { 
            return selected; 
        } 
    } 
} 


void window_messagebar(WINDOW* messagebar)
{
    wbkgd(messagebar,COLOR_PAIR(2)); 
}


void draw_model(WINDOW* window)
{
    int max_lines, max_cols;
    getmaxyx(window, max_lines, max_cols);

    /*
     * Parameters
     */
    int cols_space = 12;
    int lines_space = 6;
    int curvature = 0;
    /*
     */

    //Horizontal
    wmove(window, lines_space, 2*cols_space);
    whline(window, '#', max_cols - 4*cols_space);

    wmove(window, max_lines - lines_space - 1, 2*cols_space);
    whline(window, '#', max_cols - 4*cols_space);

    //Vertical
    wmove(window, lines_space + curvature, 2*cols_space - curvature);
    wvline(window, '#', max_lines - 2*lines_space - 2*curvature);

    wmove(window, lines_space + curvature, max_cols - (2*cols_space - curvature) - 1);
    wvline(window, '#', max_lines - 2*lines_space - 2*curvature);

    wrefresh(window);
}



int main(int argc, const char *argv[])
{
    //Start ncurses
    WINDOW* stdscr;
    int screen_max_lines, screen_max_cols;
    stdscr = ncurses_init(&screen_max_lines, &screen_max_cols);
    wbkgd(stdscr, COLOR_PAIR(1));
    
    /*
     * Menubar and messagebar
     */
    WINDOW* menubar;
    menubar = subwin(stdscr, 1, screen_max_cols, 0, 0);
    window_menubar(menubar);

    WINDOW* messagebar;
    messagebar = subwin(stdscr, 9, screen_max_cols, screen_max_lines-9, 0);
    window_messagebar(messagebar);

    wrefresh(menubar);
    wrefresh(messagebar);

    move(2,1);
    printw("Pulsa un número para acceder a los menús");
    move(3,1);
    printw("Pulsa 'espacio' para salir de un menú");
    move(4,1);
    printw("Pulsa 'q' para salir");


    int pos_diesel = 8;

    move(pos_diesel,1);
    attron(A_BOLD);
    printw("DIESEL");
    attroff(A_BOLD);
    mvprintw(pos_diesel+2,1, "Velocidad real");
    mvprintw(pos_diesel+3,1, "Velocidad objetivo");
    mvprintw(pos_diesel+5,1, "Sector");
    mvprintw(pos_diesel+7,1, "Seguridad activada");

    mvprintw(pos_diesel+2,30, "37 km/h");
    mvprintw(pos_diesel+3,30, "28");
    mvprintw(pos_diesel+5,30, "2");
    mvprintw(pos_diesel+7,30, "Sí");


    int pos_renfe = 18;

    move(pos_renfe,1);
    attron(A_BOLD);
    printw("RENFE");
    attroff(A_BOLD);
    mvprintw(pos_renfe+2,1, "Velocidad real");
    mvprintw(pos_renfe+3,1, "Velocidad objetivo");
    mvprintw(pos_renfe+5,1, "Sector");
    mvprintw(pos_renfe+7,1, "Seguridad activada");

    mvprintw(pos_renfe+2,30, "37 km/h");
    mvprintw(pos_renfe+3,30, "28");
    mvprintw(pos_renfe+5,30, "2");
    mvprintw(pos_renfe+7,30, "Sí");
    
  
    int pos_date = 30;

    move(pos_date,1);
    printw("Hoy es"); 
    attron(A_BOLD);
    mvprintw(pos_date+1,1, "10/08/2014");
    attroff(A_BOLD);
    mvprintw(pos_date+3,1, "Son las");
    attron(A_BOLD);
    mvprintw(pos_date+4,1, "19:02");
    attroff(A_BOLD);
    mvprintw(pos_date+6,1, "Y hace un día perfecto para ir en tren");


    refresh();


    const char* trenes[] = { "Diesel", "Renfe" };
    const char* cvia[] = { "Interior", "Exterior" };
    const char* barrera[] = { "Manual", "Auto"};

    int key;

    do
    {
        int selected_item; 
        WINDOW **menu_items; 

        key=getch(); 
        
        werase(messagebar); 
        wrefresh(messagebar);
 
        if (key=='1')
        { 
            menu_items=draw_menu(1, trenes, 2); 
            selected_item=scroll_menu(menu_items,2,1); 
            delete_menu(menu_items,2); 
            
            if (selected_item<0)
            { 
                mvwprintw(messagebar, 4,1, "No has seleccionado nada"); 
            }
            else
            {
                switch(selected_item)
                {
                    case 0:
                        mvwprintw(messagebar, 4,1, "Has seleccionado el tren: \"DIESEL\""); 
                        
                        move(pos_diesel,0);
                        attron(COLOR_PAIR(3)); 
                        addstr(">"); 
                        attroff(COLOR_PAIR(3)); 
                        attron(A_REVERSE);
                        printw("DIESEL");

                        move(pos_renfe,0);
                        attroff(A_REVERSE);
                        printw(" RENFE");
                        break;
                    case 1:
                        mvwprintw(messagebar, 4,1, "Has seleccionado el tren: \"RENFE\""); 

                        move(pos_diesel,0);
                        attroff(A_REVERSE);
                        printw(" DIESEL");

                        move(pos_renfe,0);
                        attron(COLOR_PAIR(3)); 
                        addstr(">"); 
                        attroff(COLOR_PAIR(3)); 
                        attron(A_REVERSE);
                        printw("RENFE");
                        break;
                }                        
            }

            touchwin(stdscr); 
            refresh();
        }
        else if (key=='2')
        {
            menu_items=draw_menu(21, cvia, 2); 
            selected_item=scroll_menu(menu_items,2,21); 
            delete_menu(menu_items,2); 
            
            if (selected_item<0)
            { 
                mvwprintw(messagebar, 4,1, "No has seleccionado nada"); 
            }
            else
            { 
                switch(selected_item)
                {
                    case 0:
                        mvwprintw(messagebar, 4,1, "Seleccionada vía INTERIOR"); 
                        break;
                    case 1:
                        mvwprintw(messagebar, 4,1, "Seleccionada vía EXTERIOR"); 
                        break;
                }                        
            }

            touchwin(stdscr); 
            refresh(); 
        }
        else if (key=='3')
        {
            menu_items=draw_menu(50, barrera, 2); 
            selected_item=scroll_menu(menu_items,2,50); 
            delete_menu(menu_items,2); 
            
            if (selected_item<0)
            { 
                mvwprintw(messagebar, 4,1, "No has seleccionado nada"); 
            }
            else
            { 
                switch(selected_item)
                {
                    case 0:
                        mvwprintw(messagebar, 4,1, "Modo de barrera: \"MANUAL\""); 
                        break;
                    case 1:
                        mvwprintw(messagebar, 4,1, "Modo de barrera: \"AUTO\""); 
                        break;
                }                        
            }

            touchwin(stdscr); 
            refresh(); 
        }
    }
    while (key!='q');

    //Stop ncurses
    delwin(menubar);
    delwin(messagebar);

    ncurses_stop();

    return 0;
}
