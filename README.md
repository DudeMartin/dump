# dump
A small C program for dumping the contents of files into standard output.

## Usage

*dump*: `<Path> <Range> <Format>`

### Explanation
#### Path

This is simply the path to the file to dump. For example, to dump some random bytes (on a Linux machine), the path argument would be `/dev/urandom/`. To dump a local file (i.e., in the same directory as the **dump** executable), the path argument would simply be the name of the file.

#### Range
The range argument has two components, the latter of which is optional. These two components are to be separated by a single comma (`,`). The first component is the amount, which specifies the *maximum* number of bytes to dump. It is possible that an execution may dump less than the specified amount, but it will never dump more than the specified amount. The second *optional* component is the offset in the file. This essentially specifies a starting location in the file.

##### Examples
A range argument with a value of ...
* `100` would dump 100 bytes starting at the beginning of the file.
* `100,20` would dump 100 bytes starting at the 21st byte in the file.

#### Format
The optional format argument specifies the formats to dump the data in. Currently, any combination of `b`, `d`, and `x` may be used (and their uppercase variants). These dump the contents in binary, decimal, and hexadecimal, respectively. By default, the output format is decimal.

## Examples
* `dump example.txt 64,8 bx` would dump 64 bytes starting at the ninth byte from `example.txt` in the local directory, in binary and hexadecimal.

* `dump /dev/zero 10000` would dump 10,000 bytes (all zeroes) starting at the beginning of `/dev/zero` in decimal.
