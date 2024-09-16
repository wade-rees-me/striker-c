#ifndef MEMORY_H
#define MEMORY_H

// Structure to store HTTP response
struct MemoryStruct {
    char *memory;
	size_t size;
};

size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp);

#endif // MEMORY_H

