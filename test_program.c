#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define DEVICE_FILE_NAME "/dev/thanh_driver"

int main()
{
    int file_desc;
    char read_buffer[100];
    char write_buffer[] = "Testing write operation";

    file_desc = open(DEVICE_FILE_NAME, O_RDWR);
    if (file_desc < 0) {
        printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
        return -1;
    }

    if (read(file_desc, read_buffer, sizeof(read_buffer)) < 0) {
        printf("Failed to read from device file: %s\n", DEVICE_FILE_NAME);
        close(file_desc);
        return -1;
    }

    printf("Received data from device:\n%s\n", read_buffer);

    if (write(file_desc, write_buffer, sizeof(write_buffer)) < 0)
	{
        perror("Failed to write to device file");
        close(file_desc);
        return -1;
    }

    printf("Data written to device successfully\n");

    close(file_desc);

    return 0;
}
