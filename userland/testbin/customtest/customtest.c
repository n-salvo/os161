/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Tests are created by making a folder in userland/testbin
 * Copy over the contents of an existing test into the new test directory
 * make sure to rename the source file to the new test name
 * edit the makefile PROG and SRCS to match the new test program
 * run bmake and bmake install within the new test directory
 * Edit the testbin's makefile and add the test directory's name to the SUBDIRS list
 * run bmake and bmake install in the testbin directory
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>


	    // case O_RDONLY:
		// canwrite=0;
		// break;
	    // case O_WRONLY:
	    // case O_RDWR:
		// canwrite=1;
		// break;
	    // default:
		// return EINVAL;
// 		#define O_RDONLY      0      /* Open for read */
// #define O_WRONLY      1      /* Open for write */
// #define O_RDWR        2      /* Open for read and write */
// /* then or in any of these: */
// #define O_CREAT       4      /* Create file if it doesn't exist */
// #define O_EXCL        8      /* With O_CREAT, fail if file already exists */
// #define O_TRUNC      16      /* Truncate file upon open */
// #define O_APPEND     32      /* All writes happen at EOF (optional feature) */
// #define O_NOCTTY     64      /* Required by POSIX, != 0, but does nothing */

// /* Additional related definition */
// #define O_ACCMODE     3      /* mask for O_RDONLY/O_WRONLY/O_RDWR */

// int close(int filehandle);

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	size_t i;
	printf("Test: hello world\n");
	printf("Pid: %d\n", getpid());

	//ssize_t __getcwd(char *buf, size_t buflen);
	//ssize_t write(int filehandle, const void *buf, size_t size);

	printf("1\n");
	size_t buflen = 50;
	char buff[buflen];

	// __getcwd(buff, buflen);
	getcwd(buff, buflen);
	
	// printf("cwd: %s\n", buff);
	printf("cwd: ");
	for(i = 0; i < buflen; i++) {
		printf("%c", buff[i]);
	}
	printf("\n");
	int descriptor_0 = open("ftest.txt", O_CREAT);
	//int descriptor_1 = open("test.txt", O_RDONLY);
	int descriptor_2 = open("ftest2.txt", O_CREAT | O_RDWR);

	printf("open file descriptor_0: %d\n", descriptor_0);
	//printf("open file descriptor_1: %d\n", descriptor_1);
	printf("open file descriptor_2: %d\n", descriptor_2);

	int descriptor_3 = 15;

	dup2(descriptor_2, descriptor_3);


	write(descriptor_2, "test write\n", 11);
	write(descriptor_3, "dupped write\n", 13);
	printf("Write finished\n");

	unsigned long long res = lseek(descriptor_2, 682468438438L, SEEK_CUR);
	printf("seek result: %llu\n", res);

	printf("close file descriptor_0: %d\n", descriptor_0);
	//printf("close file descriptor_1: %d\n", descriptor_1);
	printf("close file descriptor_2: %d\n", descriptor_2);

	printf("Calling lseek(4235, 682468438438L, 3)\n");

	printf("calling chdir(\"testbin\")\n");
	printf("chdir ret: %d, err: %d\n", chdir("testbin"), errno);
	for(i = 0; i < buflen; i++) {
		buff[i] = '\0';
	}
	getcwd(buff, buflen);
	printf("cwd: ");
	for(i = 0; i < buflen; i++) {
		printf("%c", buff[i]);
	}
	printf("\n");

	printf("calling chdir(\"testbin\")\n");
	printf("chdir ret: %d, err: %d\n", chdir("testbin"), errno);
	for(i = 0; i < buflen; i++) {
		buff[i] = '\0';
	}
	getcwd(buff, buflen);
	printf("cwd: ");
	for(i = 0; i < buflen; i++) {
		printf("%c", buff[i]);
	}
	printf("\n");

	printf("calling chdir(\"../\")\n");
	printf("chdir ret: %d, err: %d\n", chdir("../"), errno);
	for(i = 0; i < buflen; i++) {
		buff[i] = '\0';
	}
	getcwd(buff, buflen);
	printf("cwd: ");
	for(i = 0; i < buflen; i++) {
		printf("%c", buff[i]);
	}
	printf("\n");

	printf("calling chdir(\"testbin\")\n");
	printf("chdir ret: %d, err: %d\n", chdir("testbin"), errno);
	for(i = 0; i < buflen; i++) {
		buff[i] = '\0';
	}
	getcwd(buff, buflen);
	printf("cwd: ");
	for(i = 0; i < buflen; i++) {
		printf("%c", buff[i]);
	}
	printf("\n");

	printf("calling chdir(\"mu0:emu0:/testbin\")\n");
	printf("chdir ret: %d, err: %d\n", chdir("mu0:emu0:/testbin"), errno);
	for(i = 0; i < buflen; i++) {
		buff[i] = '\0';
	}
	getcwd(buff, buflen);
	printf("cwd: ");
	for(i = 0; i < buflen; i++) {
		printf("%c", buff[i]);
	}
	printf("\n");

	printf("calling chdir(\"mu0:emu0://testbin\")\n");
	printf("chdir ret: %d, err: %d\n", chdir("mu0:emu0://testbin"), errno);
	for(i = 0; i < buflen; i++) {
		buff[i] = '\0';
	}
	getcwd(buff, buflen);
	printf("cwd: ");
	for(i = 0; i < buflen; i++) {
		printf("%c", buff[i]);
	}
	printf("\n");

	printf("calling chdir(\"./testbin\")\n");
	printf("chdir ret: %d, err: %d\n", chdir("./testbin"), errno);
	for(i = 0; i < buflen; i++) {
		buff[i] = '\0';
	}
	getcwd(buff, buflen);
	printf("cwd: ");
	for(i = 0; i < buflen; i++) {
		printf("%c", buff[i]);
	}
	printf("\n");

	return 0;
}
