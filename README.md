# ext2 Filesystem Reader: Project Documentation

## 1. Objective of the project

This is a C program that reads an ext2 diskimage (`disk-backpup.img`) from the raw bytes, without needing to mount it. 

I completed the first three tasks from the problem statement:

1. Read the superblock and block group descriptors
2. Traverse directories recursively from the root
3. Read and output the contents of a file

I did not attempt Task 4 or either of the two bonus tasks.

## 2. Input

The program takes a mode as its first argument. The image `disk-backpup.img` is hardcoded in teh code, so it needs to be present in the directory the program is run from.

| Input | What it does |
|---|---|
| `./main 0` | Prints the superblock and block group descriptors |
| `./main 1` | Prints the directory layout starting from root |
| `./main 2 <filename>` | Finds <filename> and prints its contents |

## 3. File layout

| File | Purpose |
|---|---|
| `main.c` | Takes input from command line, calls the other functions |
| `definitions.h` | Contains struct definitions for the superblock, group descriptor, inode and directory entry |
| `printfunc.c / .h` | Contains printSuper(), which handles mode 0 |
| `readDirectories.c / .h` | Contains `readDirectories()`, `readSubDirectories()`, used by modes 1 and 2 |
| `readFiles.c / .h` | Contains `readFiles()`, `readBlock()`, `readSingly`, `readDoubly`, `readTriply`, prints a file's data blocks |

## 4. Structs from `definitions.h`

- All the data structures needed for the functioning of the filesystem is stored in definitions.h
- `struct superblock`: the definition of the superblock fields
- `struct group_descriptor`: the definition of each group descriptor table
- `struct inode`: the definition for an inode
- `struct directory`: the definition of a directory.
- The directory definition uses `#pragma pack(push, 1)` so the compiler doesn't insert padding bytes. Since we're reading the bytes into the directory struct in 2 separate parts, this ensures that there are no padding bytes added to fill the rest of the struct after the fisrt read. 

## 5. How each task works

### Task 1: Reading and printing the superblock and block group descriptors

Called using `./main 0`

`printSuper()` in `printfunc.c` does the following:

1. Opens the image and seeks to byte offset 1024, which is where the superblock is always stored. 
2. Reads it into the superblock struct.
3. Finds the number of block groups, rounded up:
   `num_groups = (s_blocks_count + s_blocks_per_group - 1) / s_blocks_per_group`
4. Reads the group descriptor table. The table sits in the block right after the superblock's block, so each descriptor `i` is at:
   `(s_first_data_block + 1) * block_size + sizeof(struct group_descriptor) * i`. 
   The block size is calculated as `1024 << s_log_block_size`.
5. Prints every superblock field, then prints the block bitmap, inode bitmap, inode table, free block count, free inode count and directory count for each group.



### Task 2: Directory traversal

Called using `./main 1`

`readDirectories()` sets things up and `readSubDirectories()` does the recursion.

**Setup in `readDirectories()`**

1. It reads the superblock and group descriptors again, the same way as above.
2. It finds the root inode using the first group's inode table:
  `gdt[0].bg_inode_table * block_size + (1 * 256)`
  The `1 * 256` is offset for the root directory which is at inode 2, which is at index 1 (inodes start counting from 1). The inode size of 256 is hardcoded, since that is the inode size on this image.
3. It reads that inode and passes it into `readSubDirectories()` with depth 0.

**Walking a directory in `readSubDirectories()`**

1. It loops over the first 12 entries of `i_block` (the direct blocks), skipping any that are 0.
2. For each block it seeks to `block_number * block_size`, reads the first entry (the `.` entry) and prints it.
3. An `increase` variable tracks how many bytes into the block we are. It starts at the `rec_len` of the first entry. An inner loop then keeps going until `increase` reaches the end of the block:
   - seek to `offset + increase`
   - read the 8-byte entry header, then read `name_len` bytes for the name
   - put a `\0` at the end of the name, to ensure that printf and strcmp work properly
   - add `rec_len` to `increase` to jump to the next entry
4. Each name is printed with one tab per level of depth, so subdirectory contents show up indented.
5. If the entry is a directory (`file_type == 2`) and isn't `.` or `..`, the program finds that directory's inode and calls `readSubDirectories()` on it with `depth + 1`. The inode is located with:
   - group index: `(inode - 1) / s_inodes_per_group`
   - offset: `gdt[index].bg_inode_table * block_size + ((inode - 1) % s_inodes_per_group) * 256`
6. After finishing each block, it prints an empty line, but only in mode 1, otherwise the output formatting gets messed up.

The `.` and `..` check is there so the recursion doesn't loop back on itself.

### Task 3: Reading a file (`./main 2 <filename>`)

Mode 2 reuses the same traversal from Task22. Nothing is printed for directories, but for every entry with `file_type == 1`, the program compares its name against the input filename using `strcmp`. When it matches, the file's inode is located using the same group index and offset calculation as before, and that inode is passed to `readFiles()` in `readFiles.c`.

**`readFiles()`**

1. Stores `i_size` in `bytes_toread`, which counts down as data is printed, and works out the block size as `1024 << s_log_block_size`.
2. Loops over the 12 direct block pointers in `i_block`. It stops early once `bytes_toread` reaches 0, and skips any pointer that is 0.
3. For each direct block, calls `readBlock()` and stores the updated `bytes_toread` it returns.
4. If bytes are still left after the direct blocks, it calls `readSingly()` with `i_block[12]`, then `readDoubly()` with `i_block[13]`, then `readTriply()` with `i_block[14]`. Each call only happens if there is still data left to read.

**`readBlock()`**

1. Allocates a buffer of `block_size + 1` bytes.
2. Seeks to `block_id * block_size`.
3. Reads a full block if more than a block of data remains, otherwise reads only the remaining bytes.
4. Subtracts the bytes read from `bytes_toread`, null-terminates the buffer at `block[block_size]`, and prints it with `printf("%s\n", block)`.
5. Frees the buffer and returns the new `bytes_toread`.

## 6. Known issues

- Inode size is hardcoded to 256
- File contents are printed as text rather than raw bytes
- Since I'm using printf to print the file contents, there could be additional garbage bytes at the end of the output if there is no "\0" character to mark the end of the output. I think I've managed to stop this by adding \0 at teh end of each buffer variable, but I could have missed something.
- Every file with a matching name is printed, since the function is not stopped at the first match
- No checks on input - I've assumed that disk-backpup.img contains valid input