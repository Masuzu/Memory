Playing with memory management. Re-implemented ``malloc`` and ``free`` under Windows.
``malloc`` allocates new memory blocks and split existing free blocks.
When ``free`` is called on a block, the latter is merged with its neighbouring blocks if they are free.
This is done adding metadata at the beginning of each block. These metadata are linked together as a doubly linked list, which allows fast block merging and splitting.