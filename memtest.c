#include "my_allocator.h"
#include "ackerman.h"
#include <stdlib.h>
#include <unistd.h> 

int main(int argc, char ** argv) {
    unsigned int basic_block_size =  128;
    unsigned int length  = 524288;
	
	Addr mem[4];
    
    int option;
   
   	while( (option = getopt(argc, argv, "b:s:") ) != -1) {
    	switch(option) {
			case 'b':
				basic_block_size = atoi(optarg);
				break;
			case 's':
				length = atoi(optarg);
				break;
			case '?':
				break;
			default:
				abort();
		}
    }   
  
  	init_allocator(basic_block_size, length);
	ackerman_main(); //comment this out and use the code below, if you want to test it without ackerman
	/*mem[0] = my_malloc (6);
	mem[1] = my_malloc (6);
	mem[2] = my_malloc (6);
	mem[3] = my_malloc (6);
	my_free (mem[3]);
	my_free (mem[2]);
	my_free (mem[1]);
	my_free (mem[0]);*/
	release_allocator();
	
	//atexit(release_allocator());
}


