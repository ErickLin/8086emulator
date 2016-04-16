#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Return 1 if opcode matches the format of some instruction, 0 if not
short exec_instr(char* opcode, short width) {
    short ret = 0;
    short reg32;
    char w, oo[3], mmm[4], rrr[4], sss[4];
    *oo = '\0';
    *mmm = '\0';
    *rrr = '\0';
    *sss = '\0';
    if (width == 8) {
        printf("%s", opcode);
        if (memcmp(opcode, "00110111", 8) == 0) {
            ret = 1;
            printf("  AAA\n");
        } else if (memcmp(opcode, "11101011", 8) == 0) {
            ret = 1;
            printf("  JMP Short");
        } else if (memcmp(opcode, "00", 2) == 0 && memcmp(opcode + 5, "110", 3) == 0) {
            memcpy(sss, &opcode[2], 3);
            sss[3] = '\0';
            ret = 1;
            printf("  POP SegOld  sss: %s", sss);
        } else if (memcmp(opcode, "01001", 5) == 0) {
            memcpy(rrr, &opcode[5], 3);
            rrr[3] = '\0';
            ret = 1;
            printf("  DEC RegWord  rrr: %s", rrr);
        } else {
            ret = 1;
        }
    }
    if (*oo != '\0') {
        if (memcmp(oo, "00", 2) == 0) {
        } else if (memcmp(oo, "01", 2) == 0) {
        } else if (memcmp(oo, "10", 2) == 0) {
        } else if (memcmp(oo, "11", 2) == 0) {
        }
    }
    if (*mmm != '\0') {
        if (memcmp(mmm, "000", 3) == 0) {
            printf("/%s", "DS:[BX+SI]");
        } else if (memcmp(mmm, "001", 3) == 0) {
            printf("/%s", "DS:[BX+DI]");
        } else if (memcmp(mmm, "010", 3) == 0) {
            printf("/%s", "SS:[BP+SI]");
        } else if (memcmp(mmm, "011", 3) == 0) {
            printf("/%s", "SS:[BP+DI]");
        } else if (memcmp(mmm, "100", 3) == 0) {
            printf("/%s", "DS:[SI]");
        } else if (memcmp(mmm, "101", 3) == 0) {
            printf("/%s", "DS:[DI]");
        } else if (memcmp(mmm, "110", 3) == 0) {
            printf("/%s", "SS:[BP]");
        } else if (memcmp(mmm, "111", 3) == 0) {
            printf("/%s", "DS:[BX]");
        }
    }
    if (*rrr != '\0') {
        if (memcmp(rrr, "000", 3) == 0) {
            printf("/%s", reg32 ? "EAX" : w ? "AX" : "AL");
        } else if (memcmp(rrr, "001", 3) == 0) {
            printf("/%s", reg32 ? "ECX" : w ? "CX" : "CL");
        } else if (memcmp(rrr, "010", 3) == 0) {
            printf("/%s", reg32 ? "EDX" : w ? "DX" : "DL");
        } else if (memcmp(rrr, "011", 3) == 0) {
            printf("/%s", reg32 ? "EBX" : w ? "BX" : "BL");
        } else if (memcmp(rrr, "100", 3) == 0) {
            printf("/%s", reg32 ? "ESP" : w ? "SP" : "AH");
        } else if (memcmp(rrr, "101", 3) == 0) {
            printf("/%s", reg32 ? "EBP" : w ? "BP" : "CH");
        } else if (memcmp(rrr, "110", 3) == 0) {
            printf("/%s", reg32 ? "ESI" : w ? "SI" : "DH");
        } else if (memcmp(rrr, "111", 3) == 0) {
            printf("/%s", reg32 ? "EDI" : w ? "DI" : "BH");
        }
    }
    if (*sss != '\0') {
        if (memcmp(sss, "000", 3) == 0) {
            printf("/%s", "ES");
        } else if (memcmp(sss, "001", 3) == 0) {
            printf("/%s", "CS");
        } else if (memcmp(sss, "010", 3) == 0) {
            printf("/%s", "SS");
        } else if (memcmp(sss, "011", 3) == 0) {
            printf("/%s", "DS");
        } else if (memcmp(sss, "100", 3) == 0) {
            printf("/%s", "FS");
        } else if (memcmp(sss, "101", 3) == 0) {
            printf("/%s", "GS");
        } else {
            printf("/%s", "INVALID");
        }
    }
    return ret;
}

int exec(char* fname) {
	FILE *fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't open input file %s\n", fname);
		exit(1);
	}
    /*
	int bytes_read;
	size_t nbytes = 100;
	char *buf = (char*) malloc(nbytes + 1);
	bytes_read = getline(&buf, &nbytes, fp);
    */
    char opcode[24] = {0};
    short pos = 0;
    // The currently scanned 4-letter hexadecimal word
	char hexword[4];
	while (fscanf(fp, "%s", hexword) != EOF) {
        for (int i = 0; i < strlen(hexword); i++) {
            printf("%c ", hexword[i]);
            // Buffer holding next four binary digits, initialized to 0
            short val;
            if (hexword[i] >= 'A' && hexword[i] <= 'F') {
                val = hexword[i] - 'A' + 10;
            } else if (hexword[i] >= 'a' && hexword[i] <= 'f') {
                val = hexword[i] - 'a' + 10;
            } else {
                val = hexword[i] - '0';
            }
            for (int j = 3; j >= 0; j--) {
                if (val & (1 << j)) {
                    opcode[pos] = '1';
                } else {
                    opcode[pos] = '0';
                }
                printf("%c", opcode[pos]);
                pos++;
                // Check if we have a complete instruction
                if (pos > 0 && !(pos % 8)) {
                    printf(" ");
                    if (exec_instr(opcode, pos)) {
                        memset(opcode, 0, sizeof(opcode));
                        pos = 0;
                    }
                }
            }
            printf("\n");
        }
	}
	fclose(fp);
	return 0;
}

int main() {
	//exec("DOSDEF.COM");
	exec("hello.com");
	exit(0);
}
