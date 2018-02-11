#include "cachelab.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

struct Set {
    int tag;
    bool valid;
    int min;
    int max;
};

int main(int argc, char** argv) {
    int s = 0;
    int E = 0;
    char* t = 0;
    int c = 0; 
    int b = 0;
    int miss = 0;
    int hit = 0;
    int eviction = 0;
    while((c = getopt(argc, argv, "s:E:t:b:")) != -1) {
        switch(c) {
            case 'E':
                E = atoi(optarg);
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                t = optarg;
                break;
            case 'b':
                b = atoi(optarg);
                break;
            default:
                s = 1;
                E = 1;
                b = 1;
                break;
        }
    }
    printf("%d%d%d\n",s, E, b);
    FILE *pFile; /* pointer to FILE object */
    printf("%s\n", t);
    struct Set sets[s][E];
    for(int i=0; i<s; ++i) {
        for(int j=0; j<E; ++j) {
            sets[i][j].valid = false;
        }
    }
    pFile = fopen(t,"r"); /* open trace file for reading */
    /* verify that pFile is non-NULL! */
    char access_type;
    unsigned long address;
    int size;
    /* line format is “ S 2f,1” or “ L 7d0,3” */
    /* so we need to read a character, a hex number, and a decimal number */
    /* put those in the format string along with the fixed formatting */
    while (fscanf(pFile," %c %lx,%d", &access_type, &address, &size) > 0) {
        //printf("%#010x\n", (int)address);
        //int temp1 = ~((0x1<<31)>>(31-b));
        //int op_offset = address & temp1;
        int temp2 = ~((0x1<<31)>>(31-s));
        int op_idx = (address >> b) & temp2;
        int temp3 = ~((0x1<<31)>>(b+s-1));
        int op_tag = (address >> (b + s)) & temp3;
        int op_min = address & 0xfffffff0;
        int op_max = address | 0x0000000f;
        printf("%d, %d, %d, %d\n", op_idx, op_tag, op_min, op_max);
        if (access_type == 'M') {
        	if(!sets[op_idx][0].valid) {
                miss += 1;
                hit += 1;
                sets[op_idx][0].valid = true;
                sets[op_idx][0].tag = op_tag;
                sets[op_idx][0].min = op_min;
		        sets[op_idx][0].max = op_max;
            }else {
                if(sets[op_idx][0].tag == op_tag) {
                    if(address <= sets[op_idx][0].max && address >= sets[op_idx][0].min){
                		hit += 1;
                	}else {
                		miss += 1;
                		hit += 1;
                		eviction += 1;
		                sets[op_idx][0].valid = true;
		                sets[op_idx][0].tag = op_tag;
		                sets[op_idx][0].min = op_min;
		                sets[op_idx][0].max = op_max;
                	}
                }else {
                    miss += 1;
                    hit += 1;
                    eviction += 1;
                    sets[op_idx][0].valid = true;
	                sets[op_idx][0].tag = op_tag;
	                sets[op_idx][0].min = op_min;
	                sets[op_idx][0].max = op_max;
                }
            }
        }else if(access_type == 'L') {
            if(!sets[op_idx][0].valid) {
                miss += 1;
                sets[op_idx][0].valid = true;
                sets[op_idx][0].tag = op_tag;
                sets[op_idx][0].min = op_min;
                sets[op_idx][0].max = op_max;
            }else {
                if(sets[op_idx][0].tag == op_tag) {
                	if(address <= sets[op_idx][0].max && address >= sets[op_idx][0].min){
                		hit += 1;
                	}else {
                		miss += 1;
		                sets[op_idx][0].valid = true;
		                sets[op_idx][0].tag = op_tag;
		                sets[op_idx][0].min = op_min;
		                sets[op_idx][0].max = op_max;
                	}
                }else {
                    miss += 1;
                    eviction += 1;
                    sets[op_idx][0].valid = true;
	                sets[op_idx][0].tag = op_tag;
	                sets[op_idx][0].min = op_min;
	                sets[op_idx][0].max = op_max;
                }
            }
        } else {
        	if(!sets[op_idx][0].valid) {
        		sets[op_idx][0].valid = true;
        		sets[op_idx][0].tag = op_tag;
        		sets[op_idx][0].min = op_min;
		        sets[op_idx][0].max = op_max;
        		miss += 1;
        		// hit += 1;
        	}else {
        		if(sets[op_idx][0].tag == op_tag){
        			if(address <= sets[op_idx][0].max && address >= sets[op_idx][0].min){
                		hit += 1;
                	}else {
                		miss += 1;
		                sets[op_idx][0].valid = true;
		                sets[op_idx][0].tag = op_tag;
		                sets[op_idx][0].min = op_min;
		                sets[op_idx][0].max = op_max;
                	}
        		}else{
        			miss += 1;
        			eviction += 1;
        			sets[op_idx][0].valid = true;
	                sets[op_idx][0].tag = op_tag;
	                sets[op_idx][0].min = op_min;
	                sets[op_idx][0].max = op_max;
        		}
        	}
        }
        printSummary(hit, miss, eviction);
       // printf("%c", access_type);
       // printf("%lx", address);
       // printf("%d", size);
       // printf("\n");
    }
    fclose(pFile); /* always close file when done */
    printSummary(hit, miss, eviction);
    return 0;
}