#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//MACROS
#define MAXMEM 64//maximum amount of system memory
#define PAGEMEM 16//size of a page
#define VMAX 64//virtual max memory
#define WRITEABLE 1//if a page is writeable
#define READABLE 0
#define PTE 4//size of a page table entry
#define NUMPAGES 4//total number of pages
#define VPN 0//virtual page number byte offset in a PTE
#define PFN 1//PFN byte offset in a PTE
#define PROTECTION 2//protection bits offset in a PTE
#define VALPRES 3//VALID bits offset in a PTE

//Prototypes of functions
int findpte(int pid, int vaddr);
int findfree();
int pageToFile(unsigned char *buffer,int linenum, int base);
int pageFromFile(unsigned char *buffer, int linenum);
void swaptomem(unsigned char * temp,int base);
int loadPageTable(int pid);
void removePageTable(int pid);
int loadPage(int pid, int vaddr);
void removePage(int pid, int vaddr);
int convert(unsigned char vaddr, unsigned char pid);
void robin();
void freespace(int n, int pid);
void command(char command, int pid, int vaddr, int val);
int store(unsigned char pid, unsigned char vaddr,unsigned char val );
int map(unsigned char pid, unsigned char vaddr,unsigned char val );
int load(unsigned char pid, unsigned char vaddr,unsigned char val );

//PAGE TABLE WRITEUP!
/*Page tables take up 16 bytes(1 page) and are structured as such:

byte 0: VPN 0, byte 1: PFN 0, byte 2: PROTECTION BITS(for vpn 0), byte 3: VALID/PRESENT(for vpn 0) and this repeats 3 more times. Therefore page tables will generally look like the following form. Bytes will be seperated by lines and page table entries by spaces.

0|16|1|2 1|0|0|0 2|32|0|2 3|3|1|1

Therefore a page table entry lookes like this:

Page table Entry{
	unsigned char vpn;//in range 0-3
	unsigned char pfn;//in range 0,16,32,48 or if a file number 0,1,2...
	unsigned char protection;//if writeable = 1, otherwise 0
	unsigned char valpres;//if valid and present = 2, if valid but in disk = 1, if hasn't been allocated = 0
}*/

//hardware pointer holds and address and whether that points to memory or disk
struct hp{
	int address;//address to memory or line in file
	int inmem;//1 if in memory, 0 if in file, -1 otherwise
};

//the memory, 64 bytes long
unsigned char mem[MAXMEM];

//freelist contains the free pages, 1 if the page is free 0 otherwise
int freelist[4];

//pages contains the information on what process holds a certain page
int pages[4];

int ispagetable[4];

//ptable in the form vpn1: mem[n], pfn1: mem[n+1], val1:mem[n+2], valid/present1: mem[n+3], vpn2[n+4]...

//these are hardware pointers to the page table for each process
//assigned as page tables are created dynamically
struct hp hardware[4];

int linecount;
int roundrobin;


//find pte takes a pid and a virtual address and returns the page table entry for that specific
//virtual page, returns a spot in memory
int findpte(int pid, int vaddr){
	int v  = vaddr;
	int vpage = 0;
	while(v > 16){
		v-=16;
		vpage++;
	}
	if(v==16){
		vpage++;
	}
	if(hardware[pid].inmem==1){
		return (hardware[pid].address + (PTE*vpage));//return the spot in memory
	}
	if(hardware[pid].inmem==0){
		return -1;
	}
	return -2;//return an error if it could not be found

}

//findfree() uses the freelist to return an address for the start of a free page in memory
int findfree(){
	for(int i =0; i < NUMPAGES; i++){
		if(freelist[i]==1){//this spot is free
			freelist[i] = 0;//remove this page from the freelist
			return i * PAGEMEM;//return the physical address
		}
	}
	return -1;//error because there is no space left
}


//takes a page and copies to file.
//this function returns the file line number to access with later
int pageToFile(unsigned char *buffer,int linenum, int base){
	FILE* f = fopen("swapspace.txt","a");//open the file
	fprintf(f,"%d ",linenum);//print out linenum
	for(int i= 0; i < PAGEMEM;i++){//copy in data
		fprintf(f,"%u ",buffer[base + i]);
	}
	fprintf(f,"%c ",'\n');
	fclose(f);//close
	return linenum;
}

//pageFromFile takes a buffer to load the data into and a linenumber that is return from pagetofile
//this will load the data from the line into the buffer
//if the line isnt found it returns -1, otherwise 1
int pageFromFile(unsigned char *buffer, int linenum){
	if(linenum > linecount){//line hasn't been used in disk
		printf("Error:  line %d not found in disk \n",linenum);
		return -1;
	}
	FILE * f = fopen("swapspace.txt","r");//open
	fseek(f,0,SEEK_SET);//make sure were at the top of the file
	char * token;//setup tokenize
	char line[128];//setup line
	int ln = 0;//line number interal count
	int i = 0;//iterator
	fgets(line,128,f);//grab the line
	token = strtok(line," ");//check the line number
	ln = atoi(token);
	while(ln!=linenum){//iterate until we find the line, can loop forever if not careful
		fgets(line,128,f);
		token = strtok(line," ");
		ln = atoi(token);
	}
	token = strtok(NULL," ");//get the first data number
	while(token!=NULL){//load over to the temp array
		buffer[i++] = (unsigned char) atoi(token);
		token = strtok(NULL," ");
	}
	fclose(f);
	return 1;
}

//note this functions is scary as it doesn't check page protections therefore this should only be
//used once the proper checks have been done or it will lead to unexpected behavior.
//this function simply takes a size 16 unsigned char array and copies it into memory
void swaptomem(unsigned char * temp,int base){
	for(int i =0; i < PAGEMEM; i++){
		mem[base + i] = temp[i];
	}
}

int loadPageTable(int pid){
	int line = hardware[pid].address;
	int free = findfree();
	unsigned char temp[16];
	pageFromFile(temp,line);
	swaptomem(temp,free);
	pages[free/PAGEMEM] = pid;
	hardware[pid].address = free;
	hardware[pid].inmem = 1;
	ispagetable[free/PAGEMEM] = 1;
	printf("Page table for process %d loaded from disk and stored at address %d\n",pid,free);
	return free;
}

int loadPage(int pid, int vaddr){
	int line = mem[findpte(pid,vaddr) + 1];
	int free = findfree();
	unsigned char temp[16];
	pageFromFile(temp,line);
	swaptomem(temp,free);
	pages[free/PAGEMEM] = pid;
	mem[findpte(pid,vaddr) + 1] = free;
	mem[findpte(pid,vaddr) + 3] = 2;
	printf("Page %d for process %d loaded from disk and stored at address %d\n",vaddr/16,pid,free);
	return free;
}

void removePageTable(int pid){
	int pt = findpte(pid,0);
	hardware[pid].address = pageToFile(mem,linecount++,pt);
	hardware[pid].inmem = 0;
	freelist[pt/PAGEMEM] = 1;
	ispagetable[pt/PAGEMEM] = 0;
	for(int i =0 ; i < PAGEMEM;i++){
		mem[pt + i] = 0;
	}
	pages[pt/PAGEMEM] = -1;
	printf("Page table for process %d was swapped to disk \n",pid);
}

void removePage(int pid, int paddr){
	int pte = findpte(pid,0);
	int vp = 0;
	for(int i =0; i < 16; i++){
		if(mem[pte+i]==paddr){
			vp = mem[pte+i-1];
		}
	}
	int fadr = pageToFile(mem,linecount++,paddr);
	mem[pte+(4 * vp) +PFN] = fadr;
	mem[pte+(4 * vp)+ VALPRES] = 1;
	for(int i =0;i < PAGEMEM; i++){
		mem[paddr+i] = 0;
	}
	freelist[paddr/PAGEMEM] = 1;
	pages[paddr/PAGEMEM] = -1;
}



//takes virtual address and process id and returns the physical index found
int convert(unsigned char vaddr, unsigned char pid){
	if(hardware[pid].inmem==1){//its in memory!
		int v  = vaddr;
		int vpage = 0;
		while(v > 16){
			v-=16;
			vpage++;
		}
		if(v==16){
			vpage++;
		}
		if(mem[hardware[pid].address + (PTE * vpage) + VALPRES]==0){
			return -1;
		}
		int paddr = mem[(hardware[pid].address + (PTE * vpage) + PFN)];
		int offset = vaddr - (16 * vpage);
		paddr+= offset;
		return paddr;//return physical memory index
	}
	return -1;//return 1 as this address isn't allocated
}

//increments roundrobin counter
void robin(){
	roundrobin++;
	roundrobin = roundrobin % 4;
}

//generates n number of free frames, doesn't evict the process pid
//uses round robin eviction found in readme for more info
void freespace(int n, int pid){
	robin();
	int f = 0;
	for(int i= 0; i < NUMPAGES; i++){
		if(freelist[i]==1){
			f++;
		}
	}
	int removed[4];
	for(int i =0; i < 4; i++){
		removed[i] = 0;
	}
	while(f < n){
		int j = 0;
		if(roundrobin * PAGEMEM == hardware[pid].address && hardware[pid].inmem==1){
			robin();
			j++;
		}

		while(ispagetable[roundrobin]==1){
			robin();
			j++;
			if(j==4){
				break;
			}
		}
		if(removed[roundrobin]==0){
			removePage(pages[roundrobin], roundrobin * PAGEMEM);
			printf("Swapped frame %d to disk line %d \n",roundrobin,linecount);
			removed[roundrobin] = 1;
			f++;
			robin();
			j++;
		}
		else{
			robin();
			j++;
		}
		if(j==4){
			break;
		}
	}

	//above loop removes pages, this below loop removes page tables
	while(f < n){
		if(roundrobin * PAGEMEM == hardware[pid].address && hardware[pid].inmem==1){
			robin();
		}
		if (removed[roundrobin] == 0) {
			if (ispagetable[roundrobin] == 1) {
				removePageTable(pages[roundrobin]);
				robin();
				f++;
			} else {
				removePage(pages[roundrobin], roundrobin * PAGEMEM);
				printf("Swapped frame %d to disk line %d \n", roundrobin,
						linecount - 1);
				f++;
			}
		} else {
			robin();
		}
	}
}

//command is used to run commands store and load when the pages the page table for their process isn't found in memory
//pretty much does the same thing as load and store but just frees up space before hand
void command(char command, int pid, int vaddr, int val){
	int space = 0;
	for(int i =0; i < 4; i++){
		if(freelist[i]==1){
			space++;
		}
	}
	if(hardware[pid].inmem==0){
		if(space<1){
			freespace(1,pid);
		}
		loadPageTable(pid);
		int pte = findpte(pid,vaddr);
		if(mem[pte+3]==1){
			space--;
			if(space<1){
				freespace(1,pid);
			}
			loadPage(pid,vaddr);
		}
	}
	if(command=='s'){
		int pte = findpte(pid,vaddr);
		if(mem[pte+2]==WRITEABLE){
			int paddr = convert(vaddr,pid);
			mem[paddr] = val;
			printf("Stored %d at physical address %d \n",val,paddr);
		}
		else{
			printf("ERROR: Virtual Address %d is not writeable \n",vaddr);
		}
	}
	else{
			int paddr = convert(vaddr,pid);
			if(paddr>0){
				printf("The value at virtual address %d is: %d \n",vaddr,mem[paddr]);
			}
	}
}
//store takes a pid a virtual address and a value and stores the value at the physical location
//in memory. The return value can be ignored as it will print out errors anyways. Return value
//could be used to recover from a segfault as needed.
int store(unsigned char pid, unsigned char vaddr, unsigned char val){
	int pte = findpte(pid,vaddr);//located page table entry
	if(pte>=0){//branch for a valid page table entry
		int paddr = convert(vaddr,pid);
		if(paddr <0){
			printf("ERROR: Segfault, memory has not been allocated for virtual address %d \n",vaddr);
			return 0;
		}
		if(mem[pte+2]==WRITEABLE){//if the page can be edited store value
			mem[paddr] = val;
			printf("Stored %d at physical address %d \n",val,paddr);
			return 1;
		}
		else{//otherwise print error
			printf("ERROR: Virtual Address %d is not writeable \n",vaddr);
			return 0;
		}
	}
	else if(pte==-1){//if the page is in disk launch a seperate function to deal with it
		command('s',pid,vaddr,val);
		return 1;
	}
	else if(pte==-2){//segfault memory hasn't been allocated ever for this page
		printf("ERROR: Segfault, memory has not been allocated for virtual address %d \n",vaddr);
		return 0;
	}
	return 0;
}

//map does as outlined in the assigment instructions
int map(unsigned char pid,unsigned char vaddr,unsigned char val){
	if(findpte(pid,vaddr)==-2){
		freespace(2,pid);
		int free = findfree();
		printf("Page table for process %d created at address %d\n",pid,free);
		hardware[pid].address = free;
		hardware[pid].inmem = 1;
		pages[free/PAGEMEM] = pid;
		ispagetable[free/PAGEMEM] = 1;
		for(int i =0; i <NUMPAGES; i++){
			mem[hardware[pid].address + (PTE * i)] = i;
			mem[hardware[pid].address + (PTE*i) + VALPRES] = 0;
		}
	}

	else if(findpte(pid,vaddr)==-1){
		freespace(1,pid);
		int pt = loadPageTable(pid);
		pages[pt/16] = pid;
		ispagetable[pt/16] = 1;
		printf("Loaded page table for process %d at address %d\n",pid,pt);
	}

	int pte = findpte(pid,vaddr);
	if(mem[pte + VALPRES]==2){
		if(mem[pte + 2] == val){
			printf("ERROR: Page already has value %d \n",val);
		}
		else{
			printf("Altered Page value\n");
			mem[pte+2] = val;
		}
		return 1;
	}

	int ppage = findfree();
	if(ppage < 0){
		freespace(1,pid);
		ppage = findfree();
	}
	pages[ppage/16] = pid;
	printf("Physical page for VPN %d allocated at: %d\n",mem[pte],ppage);
	mem[pte + PFN] = ppage;
	mem[pte + PROTECTION] = val;
	mem[pte + VALPRES] = 2;
	return 1;	
}


int load(unsigned char pid, unsigned char vaddr, unsigned char val){
	int pte = findpte(pid,vaddr);
	if(pte>=0){
		if(mem[pte+3]==1){
			int f = 0;
			for(int i =0; i < 4; i++){
				if(freelist[i]==1){
					f = 1;
				}
			}
			if(f){
				loadPage(pid,vaddr);
			}
			else{
				freespace(1,pid);
				loadPage(pid,vaddr);
			}
		int paddr = convert(vaddr,pid);
		if(paddr>0){
			printf("The value at virtual address %d is: %d \n",vaddr,mem[paddr]);
			return 1;
		}
		}
		else if(mem[pte+3]==2){
			int paddr = convert(vaddr,pid);
			printf("The value at virtual address %d is: %d \n",vaddr,mem[paddr]);
			return 1;
		}
		else{
			int paddr = convert(vaddr,pid);
			printf("The virtual address hasn't been allocated undeterministic behavior! Value found at %d was %d\n",vaddr,mem[paddr]);
			return 0;
		}
	}

	else if(pte==-1){
		command('l',pid,vaddr,val);
		return 1;
	}
	else if(pte==-2){
		printf("ERROR: Segfault memory hasn't been allocated for virtual address %d \n",vaddr);
		return 0;
	}
	return 0;
}

int main(){
	printf("Starting Memory Simulation \n");
	FILE * temp = fopen("swapspace.txt","w");
	fclose(temp);
	for(int i= 0; i < NUMPAGES; i++){
		hardware[i].inmem = -1;
		freelist[i] = 1;
		pages[i] = -1;
	}
	roundrobin = 0;
	unsigned char pid, choice = -1, vaddr, val = -1;
	char str[20];
	while(1){
		char * token;
		printf("Instruction?: ");
		if(fgets(str, 20, stdin)==NULL){
			printf("\n");
			return 0;
		}
		while(str[0] == '\n' || str[0] == '\0' || str[1]=='\0' || str[1]=='\n'){
			printf("Instruction?: ");
			if(fgets(str, 20, stdin)==NULL){
			printf("\n");
			return 0;
		}
		}
		token = strtok(str, ",");
		pid = token[0];
		pid-=48;
		token = strtok(NULL,",");
		if(token[0]=='m'){
			choice = 0;
		}	
		else if(token[0]=='s'){
			choice = 1;
		}
		else if(token[0]=='l'){
			choice = 2;
		}
		token = strtok(NULL,",");
		vaddr = atoi(token);
		token = strtok(NULL,",");
		val = atoi(token);
		token = NULL;
		printf("\n");
		if(pid>3 || pid < 0){
			printf("Error process id is not valid, try again \n");
			choice = 3;
		}
		if(vaddr<0 || vaddr>63){
			printf("Virtual address is not valid, try again \n");
			choice = 3;
		}
		if(val<0 || val > 255){
			printf("Incorrect value, try again");	
			choice = 3;
		}
		switch(choice){
			case 0:
				map(pid,vaddr,val);
				//print out
				break;
			case 1:
				store(pid,vaddr,val);
				//print out
				break;
			case 2:
				load(pid,vaddr,val);
				break;
			case 3:
				break;
			default:
				printf("Error input was not proper try again\n");
		}
		/*for(int i =0; i < MAXMEM;i++){
			if(i%16==0 && i>0){
				printf("| ");
			}
			printf("%d ",mem[i]);
		}*/
		printf("\n");
	}
	return 0;
}

