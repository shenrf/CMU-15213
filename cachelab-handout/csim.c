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
    long min;
    long max;
    int seq;
};

void postProcess(struct Set sets[], int n, int idx) {
	for(int i=0; i<n; ++i) {
		if(i != idx) {
			sets[i].seq += 1;
		}
	}
}

int main(int argc, char** argv) {
    int s = 0;
    int E = 0;
    char* t = 0;
    int c = 0; 
    int b = 0;
    long miss = 0;
    long hit = 0;
    long eviction = 0;
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
    int S = 0x1 << s;
    struct Set sets[S][E];
    for(int i=0; i<S; ++i) {
        for(int j=0; j<E; ++j) {
            sets[i][j].valid = false;
            sets[i][j].seq = -1;
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
        unsigned long temp1 = 1;
        for(int i=0;i<b-1;++i){
            temp1=(temp1<<1) + 1;
        }
        unsigned long temp2 = 1;
        for(int i=0;i<s-1;++i){
            temp2=(temp2<<1) + 1;
        }
        unsigned long temp3 = 1;
        for(int i=0;i<31-b-s;++i){
            temp3=(temp3<<1) + 1;
        }
        unsigned long op_idx = (address >> b) & temp2;
        unsigned long op_tag = (address >> (b + s)) & temp3;

        unsigned long op_min = address & temp1;
        unsigned long op_max = address | (~temp1);
        // printf("%d, %d, %d, %d\n", op_idx, op_tag, op_min, op_max);
        bool flag = false;
    	int temp_idx = -1;
    	int temp_seq = -1;
        if (access_type == 'M') {
        	for(int j=0; j<E; ++j) {
        		if(sets[op_idx][j].valid && sets[op_idx][j].tag == op_tag && address <= sets[op_idx][j].max && address >= sets[op_idx][j].min){
        			hit += 2;
        			sets[op_idx][j].seq = 0;
        			temp_idx = j;
        			flag = true;
        			break;
        		} else if(!sets[op_idx][j].valid){
        			temp_idx = j;
        			//temp_seq = 0;
        			break;
        		} else {
        			if(temp_seq < sets[op_idx][j].seq){
        				temp_seq = sets[op_idx][j].seq;
        				temp_idx = j;
        			}
        		}
        	}
        	if(!flag){
        		if(!sets[op_idx][temp_idx].valid) {
        			miss += 1;
        			hit += 1;
        			sets[op_idx][temp_idx].valid = true;
        			sets[op_idx][temp_idx].seq = 0;
        			sets[op_idx][temp_idx].tag = op_tag;
        			sets[op_idx][temp_idx].min = op_min;
        			sets[op_idx][temp_idx].max = op_max;
        		}else {
        			miss += 1;
        			hit += 1;
        			eviction += 1;
        			sets[op_idx][temp_idx].seq = 0;
        			sets[op_idx][temp_idx].tag = op_tag;
        			sets[op_idx][temp_idx].min = op_min;
        			sets[op_idx][temp_idx].max = op_max;
        		}
        	}
        	postProcess(sets[op_idx], E, temp_idx);
        	// if(!sets[op_idx][0].valid) {
         //        miss += 1;
         //        hit += 1;
         //        sets[op_idx][0].valid = true;
         //        sets[op_idx][0].tag = op_tag;
         //        sets[op_idx][0].min = op_min;
		       //  sets[op_idx][0].max = op_max;
         //    }else {
         //        if(sets[op_idx][0].tag == op_tag) {
         //            if(address <= sets[op_idx][0].max && address >= sets[op_idx][0].min){
         //        		hit += 2;
         //        	}else {
         //        		miss += 1;
         //        		hit += 1;
         //        		eviction += 1;
		       //          sets[op_idx][0].tag = op_tag;
		       //          sets[op_idx][0].min = op_min;
		       //          sets[op_idx][0].max = op_max;
         //        	}
         //        }else {
         //            miss += 1;
         //            hit += 1;
         //            eviction += 1;
         //            sets[op_idx][0].valid = true;
	        //         sets[op_idx][0].tag = op_tag;
	        //         sets[op_idx][0].min = op_min;
	        //         sets[op_idx][0].max = op_max;
         //        }
            //}
        }else if(access_type == 'L' || access_type == 'S') {
        	for(int j=0; j<E; ++j) {
        		if(sets[op_idx][j].valid && sets[op_idx][j].tag == op_tag && address <= sets[op_idx][j].max && address >= sets[op_idx][j].min){
        			hit += 1;
        			sets[op_idx][j].seq = 0;
        			temp_idx = j;
        			flag = true;
        			break;
        		} else if(!sets[op_idx][j].valid){
        			//temp_seq = 0;
        			temp_idx = j;
        			break;
        		} else {
        			if(temp_seq < sets[op_idx][j].seq){
        				temp_seq = sets[op_idx][j].seq;
        				temp_idx = j;
        			}
        		}
        	}
        	if(!flag){
        		if(!sets[op_idx][temp_idx].valid) {
        			miss += 1;
        			sets[op_idx][temp_idx].valid = true;
        			sets[op_idx][temp_idx].seq = 0;
        			sets[op_idx][temp_idx].tag = op_tag;
        			sets[op_idx][temp_idx].min = op_min;
        			sets[op_idx][temp_idx].max = op_max;
        		}else {
        			miss += 1;
        			eviction += 1;
        			sets[op_idx][temp_idx].seq = 0;
        			sets[op_idx][temp_idx].tag = op_tag;
        			sets[op_idx][temp_idx].min = op_min;
        			sets[op_idx][temp_idx].max = op_max;
        		}
        	}
        	postProcess(sets[op_idx], E, temp_idx);
        }
        //printf("%ld index, %d subset, %ld, %ld, %ld, \n", op_idx, temp_idx, op_tag, op_min, op_max);
        //printSummary(hit, miss, eviction);
        //printf("\n");
       // printf("%c", access_type);
       // printf("%lx", address);
       // printf("%d", size);
       // printf("\n");
    }
    fclose(pFile); /* always close file when done */
    printSummary(hit, miss, eviction);
    return 0;
}