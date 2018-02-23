//Virtual Memory Simulator by Daniel McDonough 2/19/18

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 64 //given memory size

// Given memory array
unsigned char memory[SIZE]; //holds values to be written to disk

//virtual memory
struct virtual_mem{
int pagenum; //pagenumber 
int offset; //the offset
};

struct virtual_mem v_mem[15]; //array of virtual memory (Tehnecally dont need this)

//struct of the page table
struct page_table{
int valid; //valid bit 1 = read&write 0 = read only
int frame_num; //page_table frame number [0,3]
int PID; //corresponding process id [0,3]
int pagenum; //page number [0,15] (should never change through the program)
int useage; //counter of instructions done not on the page table while it is in the TLB
int values[15]; //holds corresponding values in the page table
};


//struct of physical memory address
struct physical_addr{
int frame_num;//frame number mapped to the page table 
int offset;//offset mapping to memory (the array) given by virtual mem
};
struct physical_addr p_addr[3]; //array of physicall addresses so that each index is equal to a frame



struct page_table ptable[15]; //array of page tables holding 0 t0 15 oganized by pagenumber

int TLB_size =0; //holds the size for init/first time mapping purposes

//transistion lookaside buffer 
struct page_table TLB[3]; 


//prints TLB INFO
void print_TLB(){
printf("TLB size: %d\n",TLB_size);
if(TLB_size==0){
printf("TLB IS EMPTY\n");
}
else{
for(int i = 0;i<TLB_size;i++){
printf("TLB has pagenumber: %d in frame %d, with a useage of %d\n\n",TLB[i].pagenum,TLB[i].frame_num,TLB[i].useage);
}
}
}

void print_cache_values(){
for(int i=0;i<4;i++){
for(int j=0;j<16;j++){
printf("TLB:%d = [%d]\n",i,TLB[i].values[j]);

}


}
}


void print_pTable(){
for(int i=0;i<16;i++){
printf("PAGENUM=%d\n",ptable[i].pagenum);
}

}

//initializes all* arrays (not TLB)
void init_structs(){
//Initialize physical addressess
    for (int i = 0; i < 4; i++){
        p_addr[i].frame_num = i+1;
	 p_addr[i].offset = 0;
    }
	
    //Initialiaze physical memory
    for (int i = 0; i < SIZE; i++){
        memory[i] = '~';
	//v_values[i] =-1;
    }
//Initialize page tables & v memory
	for(int i=0; i<16; i++){
	ptable[i].valid = 1;
	ptable[i].pagenum = i;
	//printf("%d\n",ptable[i].pagenum);
	ptable[i].frame_num = i % 4;
	ptable[i].PID = i % 4;
	ptable[i].useage = 0;
	v_mem[i].pagenum = i; 
	v_mem[i].offset = 0; 
	for(int j=0;j<16;j++){
	ptable[i].values[j]=-1;
	}
}

}

//checks if argument 2 is a valid instruction
int valid_instruction(char* argv[]){
//printf("%s\n",argv[0]);
if(strcmp (argv[0],"map") == 0){
return 0;
}
else if(strcmp (argv[0],"load") == 0){
return 1;
}
else if(strcmp (argv[0],"store") == 0){

return 2; 
}
else{
return 3; //bad instruction
}

}

//checks if args are valid
int check_args(int pid, int instType, int vadd, int val){
if(instType == 3){
printf("Invalid instuction type\n");
return 1;
}
else if(pid < 0 || pid > 3){
printf("Invalid PID\n");
return 1;
}
else if(vadd > 63 || vadd <0){
printf("Invalid virtual address\n");
return 1;
}
else if(val > 255 || val < 0){
printf("Invalid value\n");
return 1;
}
else if(instType == 0 && val > 1){
printf("Invalid Map Value\n");
return 1;
}
else{
return 0;
}
}

//print memory
void print_memory(){
for(int i=0; i<SIZE; i++){
printf("INDEX %d IN MEMORY IS: [%c]\n", i, memory[i]);
}

}
void print_v_mem(int pagenum){
for(int i=0; i<SIZE; i++){
printf("INDEX %d IN MEMORY IS: [%c]\n", i, ptable[pagenum].values[i]);
}
}

//prints error
void print_error(){
//printf("\nHold on there space cowboy...\n");
            printf("Please use:\n\t ./vmSim process_id,instruction_type,virtual_address,value\n");
            printf("Where:\n");
		printf("\tprocess_id: [0,3]\n");
		printf("\tinstruction_type: map,load,store\n");
		printf("\tvirtual_address: [0,63]\n");
		printf("\tvalue: [0,255]\n");
            //exit(1);
}


//From v address return offset
int get_offset(int virtual_address){
    return (virtual_address % 16);
}
//resets given index, incremnts all other pages in the TLB
void update_useage(int cache_index){
for(int i =0; i<TLB_size;i++){
if(i==cache_index){
TLB[cache_index].useage=0;
}
else{
TLB[i].useage++;
}
}
return;
}

//from v address & pid return page #
int get_page(int virtual_address,int pid){
return (virtual_address/16)+(4*pid);  
}

//get memory location
int get_mem(int frame,int offset){
return (16*frame)+offset;
}


//check pagenum if exists in TLB and return index
int check_TLB(int pagenum){
for(int i =0; i<TLB_size;i++){//check tlb for pagenum if exists
if(pagenum==TLB[i].pagenum){
return i;
//page already in the TLB
}
}

return -1;
}

// Writes given value into memory at given mem location
int writeIntoMem(int start, char* value){
    int num_bytes = strlen(value);
    int remainder = start%16;
    if ((remainder + num_bytes) >= 15){
        return -1; // Not enough room on the current page
    }

    int i;
    for(i = start; i < 16 + start; i++){
        if(value[i - start] != 0){
            memory[i] = value[i - start];
}
        else{
            break;
}
    }

    return (i - start); // Number of bytes written
}
//remap values from pagetables to physical memory
void REMAP(int frame_num,int pagenum){
char buffer[15] = "";
//int* values;

int mem = get_mem(frame_num,0);
for(int i =0;i<16;i++){
sprintf(buffer, "%d", TLB[check_TLB(pagenum)].values[i]);
}
writeIntoMem(mem, buffer);
//print_v_mem(pagenum);

}



//returns index of page that was least recently used in the Cache
int LRU(){
int useage =0;
int index=0; //if all were just recently used then remove the pagetable 0
for(int i = 0;i<TLB_size;i++){
if(useage>TLB[i].useage){
useage=TLB[i].useage;
index = i;
}
}
return index;
}

//saves value of page in the cache into the page table array, loads new page table in.
void Swap(int pagenum, int cache_index){
//save old value to ptable
int old_page = TLB[cache_index].pagenum;
ptable[old_page].useage=0;//reset useage
ptable[old_page].PID = TLB[cache_index].PID;
ptable[old_page].valid = TLB[cache_index].valid;
//ptable[old_page].frame_num = cache_index;
for(int i=0;i<16;i++){
ptable[old_page].values[i]=TLB[cache_index].values[i];
}
//print_cache_values();
//table to swap in: ptable[pagenum]

//load new table in
TLB[cache_index].PID = ptable[pagenum].PID;
//TLB[cache_index].frame_num = cache_index;
TLB[cache_index].valid = ptable[pagenum].valid;
TLB[cache_index].pagenum = pagenum; 
for(int i=0;i<16;i++){
TLB[cache_index].values[i]=ptable[pagenum].values[i];
}
printf("VALUES HAVE BEEN SWAPPED\n");
//print_cache_values();
//remap the physical and virtual memory
REMAP(cache_index,pagenum);

printf("Swapped frame %d to disk at swap slot %d\n",cache_index,pagenum);
return;
}




//insert a page table into TLB
void insert_TLB(int pagenum,int pid,int value){

int in_TLB = 0; //in tlb flag

if(TLB_size==0){//TLB is empty
//put the page into the TLB
TLB[0].pagenum=pagenum;
TLB[0].valid = value;
TLB[0].useage=0;
TLB[0].PID = pid;
TLB[0].frame_num = ptable[pagenum].frame_num;

for(int i=0;i<16;i++){
TLB[0].values[i]=ptable[pagenum].values[i];
}

TLB_size++;
}
else{//at least 1 page exists in TLB already!

for(int i =0; i<TLB_size;i++){//check tlb for pagenum if exists
if(pagenum==TLB[i].pagenum){
in_TLB =1;
break;
//page already in the TLB
}
}
//if not in cache and in the cache has room:
if(TLB_size<=3 && in_TLB==0){//insert into TLB
TLB[TLB_size].valid = value;
TLB[TLB_size].useage=0;
TLB[TLB_size].PID = pid;
TLB[TLB_size].frame_num = ptable[pagenum].frame_num;
TLB[TLB_size].pagenum = ptable[pagenum].pagenum;
for(int i=0;i<16;i++){
TLB[TLB_size].values[i]=ptable[pagenum].values[i];
}
TLB_size++;
}
else{
//SWAP pages
//get cache num
int cache_index = LRU();
//swap the pages
Swap(pagenum,cache_index);
//update TLB with the given args
TLB[cache_index].PID = pid;
TLB[cache_index].valid = value;
TLB[cache_index].frame_num = cache_index;
for(int i=0;i<16;i++){
TLB[cache_index].values[i]=ptable[pagenum].values[i];
}
printf("Put page table for PID %d into physical frame %d\n",pid,cache_index);
}

}

}



// Maps page to physical page
int map(int pid, int v_addr, int value){
//print_TLB();
int pagenum = get_page(v_addr,pid);
//printf("MAPPING PAGE NUMBER: %d\n",pagenum);
if(check_TLB(pagenum)==-1){ //page does not exist in TLB then
insert_TLB(pagenum,pid,value);
int cache_index = check_TLB(pagenum);
update_useage(cache_index);//reset usage counter

printf("Mapped virtual address %d (page %d) into physical frame %d with permissions: %d\n", v_addr, pagenum, TLB[cache_index].frame_num,TLB[cache_index].valid);
  return 0; 
}
else{//page exists
int cache_index = check_TLB(pagenum);
update_useage(cache_index);//reset useage counter
if(TLB[cache_index].valid != value){
TLB[cache_index].valid = value;
printf("Updating permissions for virtual page %d (frame %d)\n",pagenum,TLB[cache_index].frame_num);
}
else{

printf("Error: virtual page %d is already mapped with rw_bit=%d\n",pagenum,value);
}
}


//printf("ERROR: No free space, Memory is full!\n");
    return 0; // 
}

//REad memory (ie convert char to int from a given index)
int readIntoMem(int start){
    int read_val;
    char buffer[4];
    for(int i = 0; i < 4; i++){
        if(memory[start + i] != '~' && i != 3){
            buffer[i] = memory[start + i];
	}
        else{
            buffer[i] = '0';
            break;
        }
    }
    if(buffer[0] == '\0'){ 
return -1;
}
    else{
        sscanf(buffer, "%d", &read_val);
        return read_val;
    }
}


//load into memory
int load(int pid, int v_addr){
int pagenum = get_page(v_addr,pid);
printf("SEARCHING FOR PAGENUM: %d\n",pagenum);
//print_TLB();
if(check_TLB(pagenum)==-1){
printf("Corresponding Page Not Loaded!\n");
int cache_index = LRU();
Swap(pagenum,cache_index);//swap in the page
//insert_TLB(int pagenum,int pid,int value)
}

//get TLB page id
int cache_index = check_TLB(pagenum);
struct page_table loaded_table = TLB[cache_index];
update_useage(cache_index);//reset usage count

int mem = get_mem(loaded_table.frame_num,get_offset(v_addr));

if(loaded_table.values[get_offset(v_addr)]<0){
printf("The value (EMPTY) is virtual address %d (physical address %d)\n",v_addr,mem);
return(1);
}

printf("The value %d is virtual address %d (physical address %d)\n",loaded_table.values[get_offset(v_addr)],v_addr,mem);
return(0);
 
}

//refactor code
//TODO SAVE WRITES TO FILE

//TODO write read me
//TODO make better comments
//TODO tests

//store value into memory
int store(int pid, int v_addr, int value){
int pagenum = get_page(v_addr,pid);
//get TLB page id
if(check_TLB(pagenum)==-1){
printf("Corresponding Page Not Loaded!\n");
int cache_index = LRU();
Swap(pagenum,cache_index);//swap in the page

}
int cache_index = check_TLB(pagenum);

//struct page_table loaded_table = TLB[cache_index];
//reset useage counter
update_useage(cache_index);
if(TLB[cache_index].valid == 0){
 printf("ERROR: writes are not allowed to this page\n");
return 1;
}
//write to actual memory	
char buffer[10] = "";
     
	int mem = get_mem(TLB[cache_index].frame_num,get_offset(v_addr));
        sprintf(buffer, "%d", value);
        writeIntoMem(mem, buffer);
	//write to page sub memory
	TLB[cache_index].values[get_offset(v_addr)] = value;
	
        printf("Stored value %d at virtual address %d (physical address %d) for proccess %d\n", value, v_addr, mem,pid);
 //print_memory();
//print_cache_values();
    return 0; // Success
}



// The main function
int main(int argc, char *argv[]){
    int pid = 0; //the process id [0,3]
    int instType = 0; // Instruction type Map = 0 Load = 1 store = 2 Error =3
    int v_addr = 0; // Virtual address [0,63]
    int value = 0; // Value [0,255]

    int is_end = 0; // EOF flag
    char buffer[20]; // Buffer
    char cmd_seq[20]; // The command sequence read from stdin
    char* cmd_array[4]; // Holds the commands read from file
    char* token;

    init_structs(); //init structs
//print_pTable();
//start the loop
    while (is_end != 1) {
        printf("Instruction?: \n");
        // get input from file
        if (argc <= 1){
            // Read sequence from file
            if (fgets(buffer, sizeof(buffer), stdin) == NULL){
                is_end = 1;
                printf("Done!\n"); //end of file
                break;
            }
            //buffer[strcspn(buffer, "\n")] = 0; //Remove newline
            strncpy(cmd_seq, &buffer[0], sizeof(cmd_seq));
            printf("Given Args: %s\n", cmd_seq);

            // Parse sequence
            token = strtok(cmd_seq, ","); //remove comma
            int i = 0;
            while (token != NULL){
                if (i >= 4){
                    printf("ERROR: Too many arguments!\n");
		    print_error();
                    break;
                }
                cmd_array[i] = token;
                i++;
                token = strtok(NULL, ",");
            }
		//printf("%d\n",i);
		if(i !=4){
 			printf("ERROR: Too few arguments!\n");
			print_error();
                    break; //break to return
		}

            // args into variables
            pid = atoi(cmd_array[0]);
	    instType = valid_instruction(&cmd_array[1]);
            v_addr = atoi(cmd_array[2]);
            value = atoi(cmd_array[3]);
		//check args
		if(check_args(pid,instType,v_addr,value)==1){
print_error();
break;
}
        }

        
        else{
            print_error();
		exit(1);
        }

        if (is_end == 1){
	 break; // Break if EOF
	}
        if (instType == 0){
	//map
            map(pid, v_addr, value);
	//	print_TLB();
        }
        else if (instType == 1){//load
		load(pid, v_addr);
            
        }
        else if (instType == 2){//store
            store(pid, v_addr, value);
        }
	//print_memory();
	
    }
    return 0;//let me go home
}
