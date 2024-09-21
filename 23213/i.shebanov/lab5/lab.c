#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

typedef struct Table {
    size_t size;
    size_t capacity;
    off_t * array;
} Table;

Table * createTable() {
    Table * table = (Table *)malloc(sizeof(Table));
    table->capacity = 2;
    table->size = 0;
    table->array = (off_t*)malloc(sizeof(off_t) * table->capacity);
    return table;
}

void addOffset(Table* table, off_t offset) {
    table->array[table->size] = offset;
    table->size += 1;
    if (table->size == table->capacity) {
    	table->capacity *= 2;
	table->array = (off_t*)realloc(table->array, sizeof(off_t) * table->capacity);
    }
}

void free_table(Table * table) {
    free(table->array);
    free(table);
}



int main(int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Filename is not provided");
	return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "File cannot be opened");
        return 1;
    }


    off_t file_size = lseek(fd, 0, SEEK_END);
    if(file_size == -1) {
    	fprintf(stderr, "Failed lseek");
	return 1;
    }
    if(lseek(fd, 0, SEEK_SET) == -1) {
    	fprintf(stderr, "Failed lseek");
	return 1;
    }


    Table * ot = createTable();
    addOffset(ot, 0);

    int BUFFER_SIZE = 4028;
    char buffer[BUFFER_SIZE];
    int buffers_read = 0;
    

    printf("WITH GODS HELP\n");
    int bytes_read = 0;
    do {
        bytes_read = read(fd, buffer, BUFFER_SIZE);
	for(int i = 0; i < bytes_read; i++){
	    if(buffer[i] == '\n'){
	    	addOffset(ot, (off_t)(i + buffers_read * BUFFER_SIZE));
	    }
	}
	buffers_read += 1;
    }while (bytes_read > 0);
    addOffset(ot, file_size);


    for(int i = 0; i < ot->size; i++){
        printf("%d\n", ot->array[i]);
    }   

    while(1) {
	int line_number = 0;
    	scanf("%d", &line_number);

	if(line_number == 0) {
	    free_table(ot);
	    close(fd);
	    return 0;
	} else if (line_number > 0 && line_number < ot->size){
		char * line_buffer = (char *)malloc(sizeof(char) * (ot->array[line_number] - ot->array[line_number - 1]));
                lseek(fd, ot->array[line_number - 1], SEEK_SET);
		read(fd, line_buffer, ot->array[line_number] - ot->array[line_number-1]);
	        printf("%s\n", line_buffer); 
	    
	} else {
	    fprintf(stderr, "Invalid line number");
	    return 1;
	}

    }
}

