# dump
A small C program for dumping the contents of files into standard output.

## Usage

*dump*: `<Path> <Range> <Format>`

### Explanation
#### Path

This is simply the path to the file to dump. For example, to dump some random bytes (on a Linux machine), the path argument would be `/dev/urandom/`. To dump a local file (i.e., in the same directory as the **dump** executable), the path argument would simply be the name of the file.

#### Range
The range argument has two components: offset and amount. The optional offset component specifies the offset in the file. This is essentially a starting location in the file. An offset can also be negative. A negative offset specifies a location from the *end* of the file. The amount component specifies the *maximum* number of bytes to dump. It is possible that an execution may dump less than the specified amount, but it will never dump more than the specified amount. 

When both components are specified, they must be separated by a comma (`,`). When one is detected, the value before it is interpreted as the offset, and the value after is interpreted as the amount. When no comma is detected, the value is treated as the amount (with an implicit offset of `0`).

##### Examples
A range argument with a value of ...
* `100` would dump 100 bytes starting at the beginning of the file.
* `16,256` would dump 256 bytes starting at the 17th byte in the file.
* `-64,128` would dump 128 bytes starting 64 bytes from the end of the file.

#### Format
The optional format argument specifies the formats to dump the data in. Currently, any combination of `b`, `o`, `d`, `x`, and `a` may be used (and their uppercase variants). These dump the contents in binary, octal, decimal, hexadecimal, and alphanumeric respectively. By default, the output format is decimal.

## Examples
* `dump example.txt 64,8 bx` would dump 64 bytes starting at the ninth byte from `example.txt` in the local directory, in binary and hexadecimal.

* `dump /dev/zero 10000` would dump 10,000 bytes (all zeroes) starting at the beginning of `/dev/zero` in decimal.
