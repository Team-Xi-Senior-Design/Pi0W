/* Author: MSOE Team Xi Senior Design 2017
 * Date: 12/2017
 * Description: Main c file for the helmet side controller
 */

 /********************
 * External Includes *
 *********************/
#include "main.h"
#include "Audio.h"
#include "Video.h"
#include <ncurses.h>
#include <stdio.h>
#include "Bluetooth_Pi3.h"

#define MONITOR_WIDTH 127
#define MONITOR_HEIGHT 116

int main(int argc, char* argv[]){
<<<<<<< HEAD
//	printf("Hello\n");
//	initscr();
//	int i = 1;
//	while(i<=MONITOR_HEIGHT){
//		printw("%i\n", i);
//		i++;
//		refresh();
//	}
//	getch();

//	endwin();
	char buffer[2048000];
	initCapture();
	initPlayback();
	while(1){
		int rc = testCapture(buffer);
		fprintf(stderr,"read: %d bytes\n",rc);
		testPlayback(buffer, rc);
	}
	return 0;
}
