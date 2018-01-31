#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

unsigned long **sys_call_table;

//references to old sys calls
asmlinkage long (*ref_sys_cs3013_syscall1)(void);
asmlinkage long (*ref_sys_read)(int filedes, void *buf, size_t nbytes);
asmlinkage long (*ref_sys_open)(const char *filename, int flags, mode_t mode);
asmlinkage long (*ref_sys_close)(int filedes);

//Dr Who reference from the given template 
asmlinkage long new_sys_cs3013_syscall1(void) {
  printk(KERN_INFO "\"'Hello world?!' More like 'Goodbye, world!' EXTERMINATE!\" -- Dalek");
  return 0;
}

//SYS_OPEN
asmlinkage long new_sys_open(const char *filename, int flags, mode_t mode){
    int id = current_uid().val; //Find the id of the process
    if (id >= 1000) { //print out if it is the user
        printk(KERN_INFO "user %d is opening the file: %s\n", id, filename);
        //printk(KERN_INFO "its working\n");
      }
      return ref_sys_open(filename, flags, mode); //actually open the file
}

//SYS_CLOSE
asmlinkage long new_sys_close(int filedes){
    int id = current_uid().val; //find the id of the process
    if (id >= 1000){ //print out if it is the user
      printk(KERN_INFO "user %d has closed file descriptor: %d\n", id, filedes);
    }
    return ref_sys_close(filedes); //actually close the file
}

//SYS_READ
asmlinkage long new_sys_read(int filedes, void *buf, size_t nbytes){
    //no matter what we run the desired read
    int filesize = ref_sys_read(filedes, buf, nbytes);
    int id = current_uid().val;
    if(id < 1000){ //if we arent the user just finish the read
      return filesize;
    }
    if (!filesize) { //if we dont read anything dont try to look for virus
        printk("Nothing read from file descriptor: %d", filedes);
        return filesize;
    }
    else { //look for the word virus in the read, and report if it was discovered
	
       if (strstr((char *)buf, "VIRUS")){ //print out if the virus was discovered
         printk("User %d is reading file descriptor %d, but that read contained malicious code!\n", id, filedes);
       }
        return filesize;
    }
}

static unsigned long **find_sys_call_table(void) {
    unsigned long int offset = PAGE_OFFSET;
    unsigned long **sct;

    while (offset < ULLONG_MAX) {
        sct = (unsigned long **)offset;

        if (sct[__NR_close] == (unsigned long *) sys_close) {
            printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX",
                   (unsigned long) sct);
            return sct;
        }

        offset += sizeof(void *);
    }

    return NULL;
}

static void disable_page_protection(void) {
    /*
     Control Register 0 (cr0) governs how the CPU operates.

     Bit #16, if set, prevents the CPU from writing to memory marked as
     read only. Well, our system call table meets that description.
     But, we can simply turn off this bit in cr0 to allow us to make
     changes. We read in the current value of the register (32 or 64
     bits wide), and AND that with a value where all bits are 0 except
     the 16th bit (using a negation operation), causing the write_cr0
     value to have the 16th bit cleared (with all other bits staying
     the same. We will thus be able to write to the protected memory.

     It's good to be the kernel!
     */
    write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
    /*
     See the above description for cr0. Here, we use an OR to set the
     16th bit to re-enable write protection on the CPU.
     */
    write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
    /* Find the system call table */
    if(!(sys_call_table = find_sys_call_table())) {
        /* Well, that didn't work.
         Cancel the module loading step. */
	printk("This shouldnt happen!\n");
        return -1;
    }

    /* Store a copy of all the existing functions */
    
    ref_sys_cs3013_syscall1 = (void *)sys_call_table[__NR_cs3013_syscall1];
    //copy the new commands added
    ref_sys_open = (void *)sys_call_table[__NR_open];
    ref_sys_close = (void *)sys_call_table[__NR_close];
    ref_sys_read = (void *)sys_call_table[__NR_read];


    /* Replace the existing system calls */
    disable_page_protection();

    printk("Congrats, we reached sys_cal_table\n");
    sys_call_table[__NR_cs3013_syscall1] = (unsigned long *)new_sys_cs3013_syscall1;
    sys_call_table[__NR_open] = (unsigned long *)new_sys_open;
    sys_call_table[__NR_close] = (unsigned long *)new_sys_close;
    sys_call_table[__NR_read] = (unsigned long *)new_sys_read;

    enable_page_protection();

    /* And indicate the load was successful */
    printk(KERN_INFO "Loaded interceptor!");

    return 0;
}

static void __exit interceptor_end(void) {
    /* If we don't know what the syscall table is, don't bother. */
    if(!sys_call_table)
        return;

    /* Revert all system calls to what they were before we began. */
    disable_page_protection();
    sys_call_table[__NR_cs3013_syscall1] = (unsigned long *)ref_sys_cs3013_syscall1;
    //include sys_open,read,and close
    sys_call_table[__NR_open] = (unsigned long *)ref_sys_open;
    sys_call_table[__NR_close] = (unsigned long *)ref_sys_close;
    sys_call_table[__NR_read] = (unsigned long *)ref_sys_read;

    enable_page_protection();

    printk(KERN_INFO "Unloaded interceptor!");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
