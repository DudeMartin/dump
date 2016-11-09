#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>

#define PRINT_BINARY (1 << 0)
#define PRINT_DECIMAL (1 << 1)
#define PRINT_HEX (1 << 2)

static void printDump(int, char *, int, int);

int main(int argc, char *argv[]) {
	if (argc < 3) {
		puts("Usage: <file name> <amount[,offset]> [format: bdx]");
		return EXIT_FAILURE;
	}
	char *amountToken = strtok(argv[2], ",");
	int amount = atoi(amountToken);
	char *offsetToken = strtok(NULL, ",");
	int offset = (offsetToken != NULL) ? atoi(offsetToken) : 0;
	if (amount < 0) {
		fputs("Cannot read a negative amount.\n", stderr);
		return EXIT_FAILURE;
	}
	errno = 0;
	char *data = malloc(amount);
	if (data == NULL) {
		fprintf(stderr, "Memory allocation error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	int printFormats;
	if (argc > 3) {
		int formatCount = strlen(argv[3]);
		while (--formatCount >= 0) {
			char format = argv[3][formatCount];
			switch (format) {
			case 'b':
			case 'B':
				printFormats |= PRINT_BINARY;
				break;
			case 'd':
			case 'D':
				printFormats |= PRINT_DECIMAL;
				break;
			case 'x':
			case 'X':
				printFormats |= PRINT_HEX;
				break;
			default:
				printf("Unrecognized print format: %c\n", format);
				break;
			}
		}
	} else {
		printFormats = PRINT_DECIMAL;
	}
	char *fileName = argv[1];
	int file = open(fileName, O_RDONLY);
	if (file < 0) {
		fprintf(stderr, "Failed to open %s: %s\n", fileName, strerror(errno));
		return EXIT_FAILURE;
	}
	errno = 0;
	int bytesRead = pread(file, data, amount, offset);
	if (errno) {
		fprintf(stderr, "Reading error: %s\n", strerror(errno));
		close(file);
		return EXIT_FAILURE;
	}
	close(file);
	printDump(offset, data, bytesRead, printFormats);
	return EXIT_SUCCESS;
}

static const char *binaryString(char value) {
	static char bits[9] = { [8] = '\0' };
	int i;
	for (i = 0; i < 8; i++) {
		bits[i] = '0' + ((value >> i) & 1);
	}
	return bits;
}

static void printFormat(int baseOffset, char *data, int amount, int lineLength, int format) {
	const int padding = log10(amount) + 4;
	int offset = 0;
	while (amount > 0) {
		int toPrint = (amount >= lineLength) ? lineLength : amount;
		int i;
		printf("%0*d ", padding, baseOffset + offset);
		for (i = 0; i < toPrint; i++) {
			unsigned char value = data[offset + i];
			switch (format) {
			case PRINT_BINARY:
				printf("%s ", binaryString(value));
				break;
			case PRINT_DECIMAL:
				printf("%4d ", value);
				break;
			case PRINT_HEX:
				printf("%02X ", value);
				break;
			}		
		}
		putchar('\n');
		amount -= toPrint;
		offset += toPrint;
	}
}

static void printDump(int baseOffset, char *data, int amount, int formats) {
	if ((formats & PRINT_BINARY) != 0) {
		puts("Binary");
		printFormat(baseOffset, data, amount, 8, PRINT_BINARY);
		putchar('\n');
	}
	if ((formats & PRINT_DECIMAL) != 0) {
		puts("Decimal");
		printFormat(baseOffset, data, amount, 14, PRINT_DECIMAL);
		putchar('\n');
	}
	if ((formats & PRINT_HEX) != 0) {
		puts("Hexadecimal");
		printFormat(baseOffset, data, amount, 24, PRINT_HEX);
	}
}
