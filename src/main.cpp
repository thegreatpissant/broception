#include <iostream>
#include <string>

#ifdef BROCCOLI
#include <broccoli.h>
#endif

#include <ncurses.h>

using namespace std;

//  Some function declerations
void update_stats();

//  Global state vars
BroConn *bc;
int queue_length = 0;
int queue_length_max = 0;
int event_count = 0;

//  callback function signature
//  typedef void (*BroEventFunc) (BroConn *bc, void *user_data, ...);
//static void remote_conn_cb(BroConn *bc, void *user_data, BroRecord *conn);
static void remote_conn_cb(BroConn *bc, void *user_data, BroRecord *conn)
{
    printw("new_connection\n");
    event_count++;
}
static void http_request_cb(BroConn *bc, void *user_data, BroRecord *conn)
{
    printw("http_request\n");
    event_count++;
}

int main()
{

    //  Initialize broccoli
    bro_init(NULL);

    //  Turn on debuging
    bro_debug_calltrace = 1;
    bro_debug_messages = 1;

    //  Where our server lives
    string host_str = "localhost:47758";

    // Setup the connection
    if (!(bc = bro_conn_new_str(host_str.c_str(), BRO_CFLAG_RECONNECT | BRO_CFLAG_ALWAYS_QUEUE))) {
        cerr << "Requesting new bro connection failed" << endl;
        exit(EXIT_FAILURE);
    }

    //  Setup our callback for connection events 
    bro_event_registry_add(bc, "http_request", (BroEventFunc)http_request_cb, NULL);
    bro_event_registry_add(bc, "new_connection", (BroEventFunc)remote_conn_cb, NULL);

    //  Check the connection is valid
    if (!bro_conn_connect(bc)) {
        cerr << "Error connecting to remote bro instance: \"" << host_str << "\"" << endl;
        exit(EXIT_FAILURE);
    }

    //  ncurses section
    initscr();
    raw ();
    nodelay(stdscr, TRUE);
    bool quit = false;
    do {
      mvprintw(1,4,"Hello broception!\n");
      if (bro_conn_process_input(bc))
        printw("input proccessed\n");
      else
        printw("no input\n");
      if (bro_conn_alive(bc))
        printw("connection alive\n");
      else printw("connection dead\n");
      update_stats();
      printw("bro queue length is: %d\n", queue_length);
      printw("bro queue length size is: %d\n ", queue_length_max);
      printw("event count: %d\n", event_count);
      refresh();
      char keycommand = getch ();
      if (keycommand == 'q')
        quit = true;

    } while (!sleep(1) && !quit);
    endwin();
    bro_conn_delete(bc);
    return 0;
}


void update_stats()
{
    queue_length = bro_event_queue_length(bc);
    queue_length_max = bro_event_queue_length_max(bc);
}

