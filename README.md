The concurrent HashMap goals is to be a thread safe hash map. That allows many 
concurrent readers and writers.  
The API consists of 4 functions. 
void * newtable(); 
This function is used for making new tables and it will return a void pointer to the table. This 
pointer is how the other function identifies the table; 
int put(void *table,uint64_t key,uint64_t value); 
This is the Put function for the table it takes in a void pointer to the table you are using. If the 
pointer is Null, then the table will return -1 else it will return 0. The other two parameters are 
the key and the value you want to store. This function in this current state treats putting 
with the value=0 as a delete. Also do not put with the key set to 0. This can lead to some 
inconsistent behavior.(fixing this is one of the next thing I will be working on). 
uint64_t get(void * table, uint64_t key); 
This is the get function. If the table is null get will return 0.  It takes in the key you want to 
retrieve. If the key does not exist within the table  it will return 0. 
int destroytable(void **t); 
This function is used for destroying existing tables it is need be called when use of the table 
is complete. This function frees up the allocated memory for the table and sets the table 
pointer to null . This takes in a pointer to the table pointer. It returns -1 if the table pointer is 
already null else it return 0;
