/*
* File: multi-lookup.h
* By Heather Dykstra
* heather.dykstra@colorado.edu
* Create Date : 02/25/2013
* Discription : 
* This is the header file for the multi-threaded DNS resolver
* OS - PA2
*/

#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H 

//Include all the allowed libraries
//Taken from PDF
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include "queue.h"
#include "util.h"

//Define our constants 
//Taken from lookup.c and the given constants in the PDF
#define MINARGS 3 
#define USAGE "<inputFilePath> <outputFilePath>" 
#define MAX_NAME_LIMIT 225 
#define BUFSIZE 1024 
#define INPUTFS "%1024s"
#define MAX_RESOLVER_THREADS 10 

#define QUEUE_SIZE 50 //The max queue size - just a helpful const so things remain consistant


 
typedef struct inputFunctionParameter{
    FILE* file_name; //holds name of file
    pthread_mutex_t* queueL; //can hold a mutex
    queue* q; //holds our queue
} inFunP;


typedef struct outputFunctionParameter{
    FILE* file_name;
    pthread_mutex_t* queueL;
    pthread_mutex_t* outL; //holds a second mutex, locks you out
    queue* q; 
    int* alive; //An extra varible to check if we can keep running
} outFunP;

//Function for the input and output thread functions
void* InputThread(void* p); 
void* OutputThread(void* p);


#endif
