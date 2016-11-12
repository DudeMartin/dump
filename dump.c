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
#define PRINT_ALPHA (1 << 3)

static void printDump(int, char *, int, int);

int main(int argc, char *argv[]) {
	if (argc < 3) {
		puts("Usage: <file name> <[offset,]amount> [format: bdxa]");
		return EXIT_FAILURE;
	}
	int offset = 0;
	int amount;
	if (strchr(argv[2], ',') != NULL) {
		char *offsetToken = strtok(argv[2], ",");
		offset = atoi(offsetToken);
		char *amountToken = strtok(NULL, ",");
		amount = (amountToken != NULL) ? atoi(amountToken) : 0;
	} else {
		amount = atoi(argv[2]);
	}
	if (amount < 0) {
		fputs("Cannot read a negative amount.\n", stderr);
		return EXIT_FAILURE;
	}
	char *data = malloc(amount);
	if (data == NULL) {
		fprintf(stderr, "Memory allocation error: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	int printFormats = 0;
	if (argc > 3) {
		int inputCount = strlen(argv[3]);
		while (--inputCount >= 0) {
			char format = argv[3][inputCount];
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
			case 'a':
			case 'A':
				printFormats |= PRINT_ALPHA;
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
		free(data);
		return EXIT_FAILURE;
	}
	int bytesRead = pread(file, data, amount, offset);
	if (bytesRead < 0) {
		fprintf(stderr, "Reading error: %s\n", strerror(errno));
		free(data);
		close(file);
		return EXIT_FAILURE;
	}
	close(file);
	printDump(offset, data, bytesRead, printFormats);
	free(data);
	return EXIT_SUCCESS;
}

static char *binaryString(char value) {
	static char bits[9] = { [8] = '\0' };
	int i;
	for (i = 0; i < 8; i++) {
		bits[7 - i] = '0' + ((value >> i) & 1);
	}
	return bits;
}

static void printFormat(int baseOffset, char *data, int amount, int lineLength, int format) {
	const int padding = log10(amount) + 4;
	int offset = 0;
	while (amount > 0) {
		int toPrint = (amount >= lineLength) ? lineLength : amount;
		int i;
		printf("%0*d: ", padding, baseOffset + offset);
		for (i = 0; i < toPrint; i++) {
			unsigned char value = data[offset + i];
			switch (format) {
			case PRINT_BINARY:
				printf("%s ", binaryString(value));
				break;
			case PRINT_DECIMAL:
				printf("%3d ", value);
				break;
			case PRINT_HEX:
				printf("%02X ", value);
				break;
			case PRINT_ALPHA:
				printf("%c ", (value >= ' ' && value <= '~') ? value : '.');
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
		formats &= ~PRINT_BINARY;
		puts("Binary");
		printFormat(baseOffset, data, amount, 8, PRINT_BINARY);
		if (formats) {
			putchar('\n');
		}
	}
	if ((formats & PRINT_DECIMAL) != 0) {
		formats &= ~PRINT_DECIMAL;
		puts("Decimal");
		printFormat(baseOffset, data, amount, 18, PRINT_DECIMAL);
		if (formats) {
			putchar('\n');
		}
	}
	if ((formats & PRINT_HEX) != 0) {
		formats &= ~PRINT_HEX;
		puts("Hexadecimal");
		printFormat(baseOffset, data, amount, 24, PRINT_HEX);
		if (formats) {
			putchar('\n');
		}
	}
	if ((formats & PRINT_ALPHA) != 0) {
		puts("Alphanumeric");
		printFormat(baseOffset, data, amount, 36, PRINT_ALPHA);
	}
}
