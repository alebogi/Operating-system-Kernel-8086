/*
 * main.cpp
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include "thread.h"
#include "queue.h"
#include "PCB.h"
#include "system.h"

extern int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]){
	int res;
    System::init();
    res = userMain(argc, argv);
    System::restore();

    return res;
}


