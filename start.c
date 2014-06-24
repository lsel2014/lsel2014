/**
 * @file    start.c
 * @date    2014-06-22
 * @brief   Train scale model control program
 *
 * Starts up daemon and gui for our train scale model
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void call_daemon()
{
    execlp("./daemon", "./daemon", "NULL");
    exit(1);
}

void call_gui()
{
    exelp("./gui", "./gui", "NULL");
    exit(1);
}

int main(int argc, const char *argv[])
{
    /*
     * Initialize gui - daemon pipes
     */
    int pipe_to_daemon[2], pipe_from_daemon[2];

    pipe(pipe_to_daemon);
    pipe(pipe_from_daemon);

    /*
     * Call both gui and daemon
     */
    int pid;
    if ((pid = fork()) == 0) 
    {
    //gui
        //Pipe setup
        close(pipe_to_daemon[0]); dup2(pipe_to_daemon[1], 1); close(pipe_to_daemon[1]);
        close(pipe_from_daemon[1]); dup2(pipe_from_daemon[0], 0); close(pipe_from_daemon[0]);

        //Child process
        call_gui();
    }
    else
    {
    //daemon
        //Pipe setup
        close(pipe_to_daemon[1]); dup2(pipe_to_daemon[0], 0); close(pipe_to_daemon[0]);
        close(pipe_from_daemon[0]); dup2(pipe_from_daemon[1], 1); close(pipe_from_daemon[1]);

        //Father process
        call_daemon();
    }

    return 0;
}
