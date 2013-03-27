//still working in it...

void HYBRID_process(int writeFile){
  unsigned long i;
        /*Calculates 5000 factorial*/
        unsigned long N=3e3;
        /*This sets up the storage variable to store the factorial*/
        int bytesize;  
	char *buffer;
	mpz_t factorial;
        mpz_init(factorial);
        mpz_set_str(factorial, "1", 10);
        /*Lets be inefficent and mutiply them all one by one*/
        for(i=2; i<=N; i++) {
                mpz_mul_ui(factorial,factorial,i);
       		bytesize=gmp_asprintf(&buffer,"Current factoraial of is %Zd",
					factorial); 
	if(write(writeFile,buffer,bytesize)<0){
                perror("Error writing output file");
                exit(EXIT_FAILURE);
                }
        }
