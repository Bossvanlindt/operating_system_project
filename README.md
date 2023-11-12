# Operating Systems Project
Implementation of a terminal cli, memory paging mechanism, and task scheduling with various scheduling policies. 

## Notes from tutorial
### Part 1
1. Backing store for paging
    1. The current directory
    2. Essentially: cp prog1 backingStoreDirectory
    3. Copy files into backing store directory
2. Partition shell memory
    1. Variable store: vars stored via set command
    2. Frame store stores the scripts
        1. Each frame contains 3 lines of shell memory entries/lines
3. Code loading
4. Creating the page table
    1. Each program has its own page table, where the index means the page number and the value at that index means the frame number (so, maps page number to frame number)

### Part 2
1. Code loading
    1. Load 2 pages aka 2*3 = 6 lines from each input prog into frame store
    2. Update page table accordingly to keep track
    3. Can maybe use mod 3 logic to know if a frame is done and whether to check page table to see where next frame is for that prog
        1. And if no next frame, handle page fault

### Part 3
1. LRU cache: a list storing order of pages
    1. Initialize queue to 0123… (first frame is the most recent)
    2. Loading a frame puts it at top of list (so, it counts as the most recently used)

Notes on TA implementation
* Use system command to create backing store directory & move files
* Frame store: all code assumes only three lines per frame, write logic accordingly
* Page table inside PCB struct (a list of integers)
* Also a page table index and program counter in PCB
    * Page table index: which element of the page table we’re in right now (keeps track of which frame of file we’re at right now)
    * Program counter: which lines inside frame (as we might stop in the middle of it)
