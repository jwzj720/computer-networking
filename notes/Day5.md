Look up shared memory structures
- Going to have 3 threads, a main I/O, and then a send/receive. That way send/receive is happening at the same time, and then writes to the main data structure. 