# Concurrent HashMap in C

The goal of this Concurrent HashMap is to provide a thread-safe hash map that supports many concurrent readers and writers.

## API

The API consists of four functions.

### `void *newtable();`

Creates a new hash table and returns a pointer to it. This pointer is used by the other functions to identify the table.

### `int put(void *table, uint64_t key, uint64_t value);`

Inserts or updates a key-value pair in the table.

Parameters:

* `table` - Pointer to the hash table.
* `key` - The key to insert or update.
* `value` - The value associated with the key.

Returns:

* `0` on success.
* `-1` if `table` is `NULL`.

Notes:

* In the current implementation, calling `put()` with `value == 0` is treated as a delete operation.
* Do not use a key value of `0`. Doing so may result in inconsistent behavior. Fixing this limitation is one of the next planned improvements.

### `uint64_t get(void *table, uint64_t key);`

Retrieves the value associated with a key.

Parameters:

* `table` - Pointer to the hash table.
* `key` - The key to look up.

Returns:

* The value associated with the key.
* `0` if the key does not exist.
* `0` if `table` is `NULL`.

### `int destroytable(void **t);`

Destroys an existing table and frees all associated memory. This function should be called when the table is no longer needed.

Parameters:

* `t` - Pointer to the table pointer.

Returns:

* `0` on success.
* `-1` if the table pointer is already `NULL`.

On success, the function frees all allocated memory and sets the table pointer to `NULL`.

## Future Plans

This concurrent hash map will eventually be used as part of a persistent append-only log key-value store that is currently under development.
;
