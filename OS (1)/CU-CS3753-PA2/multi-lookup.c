//Heather Dykstra
//Due: 2/27/2013
//This is the c file for my DNS lookup
//I built this off of the lookup.h we were provided
//Eric Lobato worked with me

#include "multi-lookup.h"
#include "queue.h"

int main(int argc, char* argv[]){ 

	int infiles = argc-2; 
	int i,t,errorc; 
	int alive=1; // Is there an input file still alive? -output check

	
	//Makes an array of input files so that there is no race condition for the reader threads
	FILE* inputFiles[infiles]; 
	FILE* outputfile=NULL; 
	//Makes an array of thread trackers to keep track of the spawned input and output threads
	pthread_t ithreads[infiles];
	pthread_t othreads[MAX_RESOLVER_THREADS];
	//Declares the two mutexes used 
	pthread_mutex_t queuelock;
	pthread_mutex_t filelock;
	//Declares the shared queue	
	queue shared;

	//Declares an array of the two parameter structs
	inFunP inparameters[infiles];
	outFunP outparameters[MAX_RESOLVER_THREADS];
	//If there are not enough input arguments (copied from lookup.c)
	if(argc < MINARGS){
		fprintf(stderr, "Not enough arguments: %d\n", (argc - 1));
		fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
		return EXIT_FAILURE;
	    }
	// Error Handling for the queue creation
	if(queue_init(&shared, QUEUE_SIZE)==QUEUE_FAILURE) {
		fprintf(stderr,"Error creating the Queue\n");
		return EXIT_FAILURE; 
	}
	
	//Error handeling for the queue mutex creation
	errorc=pthread_mutex_init(&queuelock,NULL);
	if(errorc){ 
		fprintf(stderr, "Error creating the Queue Mutex\n");
		fprintf(stderr, "Error No: %d\n",errorc);  
		return EXIT_FAILURE; 
	}
	//Error handeling for the file mutex creation
	errorc=pthread_mutex_init(&filelock,NULL);
	if(errorc){ 
		fprintf(stderr, "Error creating the output file Mutex\n");
		fprintf(stderr, "Error No: %d\n",errorc);  
		return EXIT_FAILURE; 
	}
	
	//Opens the output file and handles any errors
	outputfile = fopen(argv[(argc-1)], "w");
	    if(!outputfile){
		fprintf(stderr,"Error Opening Output File\n");
		return EXIT_FAILURE;
	    }

	// opens the input files into an array 
	for (i = 1; i < argc-1; i++)
	{
	    inputFiles[i-1] = fopen(argv[i], "r");
	    if(!inputFiles[i-1]) { //error handling for input file opening 
	       fprintf(stderr, "Error Opening Input File: %s\n", argv[i]);
	       return EXIT_FAILURE;
	    }
	}

       //creates the input reading threads
       for(t=0;t < infiles ;t++){
	FILE* curFile=inputFiles[t];
	inparameters[t].queueL=&queuelock; //Fills the paramater array 
	inparameters[t].file_name=curFile; 
	inparameters[t].q=&shared;
	//This creates the reading threads with a pointer to the parameter struct
	 errorc = pthread_create(&(ithreads[t]), NULL,InputThread , &inparameters[t]); 
	if (errorc){//(Copied from pthread-hello.c)
            fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", errorc);
            exit(EXIT_FAILURE);
        }
    }

	//creates the output writing threads
       for(t=0;t < MAX_RESOLVER_THREADS;t++){
		//Creates the output file parameters struct 
		outparameters[t].queueL=&queuelock; 
		outparameters[t].file_name=outputfile; 
		outparameters[t].outL=&filelock; 
		outparameters[t].q=&shared;
		outparameters[t].alive=&alive;
	 errorc = pthread_create(&(othreads[t]), NULL, OutputThread, &(outparameters[t]));
	if (errorc){
            fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", errorc);
            exit(EXIT_FAILURE);
        }
    }


	//Wait for all the input threads to finish
	for(t=0;t<infiles;t++){
        pthread_join(ithreads[t],NULL);
    }
	//Set alive to zero to let the output threads know that there are no more input treads
	alive=0;
	//Now we wait for the output threads to finish running
	for(t=0;t<MAX_RESOLVER_THREADS;t++){
        pthread_join(othreads[t],NULL);
    }

	//error handling for closing the output file
	if(fclose(outputfile)){
		fprintf(stderr, "Error Closing output file \n");
	}
	//Frees the space allocated by the queue 
	queue_cleanup(&shared);
	//Destroyes the two mutexes for cleanup 
	pthread_mutex_destroy(&queuelock);
	pthread_mutex_destroy(&filelock);

	return EXIT_SUCCESS;
}

void* InputThread(void* p) {
	char hostname[MAX_NAME_LIMIT];
	inFunP* parameters = p;
	//Reads the data out of the input parameter struct 
	FILE* fName= parameters->file_name;
	pthread_mutex_t* queuelock = parameters-> queueL; 
	queue* shared= parameters->q;
	//Declares a pointer to the payload
	char* payload;
	// declares a variable to make sure the file was written correctly 
	int writesuccess=0;
	// dummy variable to catch error codes
	int errorc=0;
	while(fscanf(fName, INPUTFS, hostname) > 0){
		while(!writesuccess){ //While we haven't written to the file		
			errorc=pthread_mutex_lock(queuelock);//Catch the pthread lock error
			if(errorc){//error handeling
				fprintf(stderr, "Queue Mutex lock error %d\n",errorc); 
			}
			if(queue_is_full(shared)){//if the queue is full and we can't write
				errorc=pthread_mutex_unlock(queuelock); //unlock the mutex
				if(errorc){//error handling
					fprintf(stderr,"Queue Mutex unlock error %d\n", errorc);
				}				

				usleep((rand()%100)*1000);//sleep for a random amount of time. 
			}
			else {
				payload=malloc(MAX_NAME_LIMIT); //Malloc some space on the heap
				if(payload==NULL) { //error handling
					fprintf(stderr, "Malloc returned an error\n");
					fprintf(stderr, "Warning results non-deterministic\n");
				}
				//Loads the line into the payload
				payload=strncpy(payload, hostname, MAX_NAME_LIMIT); 
				if(queue_push(shared,payload)==QUEUE_FAILURE){
					fprintf(stderr, "Queue Push error\n");
				}
				//Error handling for unlocking the mutex
				errorc=pthread_mutex_unlock(queuelock);
				if(errorc){
					fprintf(stderr,"Queue Mutex unlock error %d\n", errorc);
				} 
				// We have successfully written to the queue 
				writesuccess=1;
			}
		}
		//Reset write success for the next line
		writesuccess=0;
	}
	//Close the input file- check for errors
	if(fclose(fName)){
		fprintf(stderr, "Error closing input file \n");
	}
	return NULL;
}
void* OutputThread(void* p) {
	//Very similar to InputThread
	outFunP* parameters = p;
	//Pulls data out of the parameters 
	FILE* outfile= parameters->file_name;
	pthread_mutex_t* queuelock = parameters-> queueL; 
	pthread_mutex_t* filelock = parameters->outL;
	queue* shared= parameters->q;
	int* alive= parameters->alive;
	//Sets up the reader information from the queue 
	char* payload;
	char ipaddress[INET6_ADDRSTRLEN];
	//Dummy variable to catch error codes
	int errorc=0;
	//As long as there are still writer threads and the queue is not empty 
	while(*alive || !queue_is_empty(shared)){
		//Try locking the mutex
		errorc=pthread_mutex_lock(queuelock);
		if(errorc){
			fprintf(stderr, "Queue Mutex lock error %d\n",errorc); 
		}
		//Get something off the queue 
		payload=queue_pop(shared);
		//if the queue is empty or something wrong was pushed on it
		if(payload==NULL)
		{
			//Unlock the mutex, and handle any errors 
			errorc=pthread_mutex_unlock(queuelock);
			if(errorc){
				fprintf(stderr, "Queue Mutex unlock error %d\n", errorc);
			}		
			//sleep for a bit to give the queue a chance to refill
			usleep((rand()%100)*1000);
		}
		//We have data to work with 
		else {
			//Let go of the queue
			errorc=pthread_mutex_unlock(queuelock);
			if(errorc){
				fprintf(stderr, "Queue Mutex unlock error %d\n", errorc);
			}		
			//Look up the ipaddress from the string given by the queue
			if(dnslookup(payload, ipaddress, sizeof(ipaddress))
			 	 == UTIL_FAILURE){ //Handle errors 
                		fprintf(stderr, "dnslookup error: %s\n", payload);
                		strncpy(ipaddress, "", sizeof(ipaddress));
			}		
			//Lock the file mutex and prepare to write
			errorc=pthread_mutex_lock(filelock);
			if(errorc){
				fprintf(stderr, "File Mutex lock error %d\n", errorc);
			}
			//If we fail to output something catch it otherwise write to file 
			errorc=fprintf(outfile,"%s,%s\n", payload, ipaddress);
			if(errorc<0){
				fprintf(stderr, "Output file write error\n");
			}			
			//unlock the file mutex and handle errors
			errorc=pthread_mutex_unlock(filelock);
			if(errorc){
				fprintf(stderr, "File Mutex unlock error %d\n", errorc);
			}
			//Free the space allocated by the payload 
			free(payload);
			//For safety set payload to null
			payload=NULL;
		}

	}

	return NULL;
}
