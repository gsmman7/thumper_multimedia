

// Started from http://elinux.org/Interfacing_with_I2C_Devices
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>

using namespace std;

enum Qt1070Register {
  CHIP_ID = 0,
  FIRMWARE = 1,
  KEY_STATUS = 3,
  RESET = 57
};

enum TouchBerryPiShieldKey {
  UP = 0x04,
  DOWN = 0x08,
  LEFT = 0x01,
  RIGHT = 0x02,
  A = 0x20,
  B = 0x10,
  X = 0x40
};


bool write_register_address(int file, char registerAddress) {
  char writeBuffer[] = { registerAddress };

  // First write address
  if (write(file, writeBuffer, sizeof(writeBuffer)) != sizeof(writeBuffer)) {
      printf("Failed to write to the i2c bus.\r\n");
      return false;
  }

  return true;
}


bool read_register(int file, char registerAddress, char * value) {

    if (!write_register_address(file, registerAddress)) {
      return false;
    }

    sleep(1);

    char readBuffer[1];
    if (read(file, readBuffer, sizeof(readBuffer)) != sizeof(readBuffer)) {
        printf("Failed to read from the i2c bus.\r\n");
        return false;
    } else {
        *value = readBuffer[0];
    }
    return true;
}

bool reset_device(int file) {
    char writeBuffer[] = { RESET };

    // First write address
    if (write(file, writeBuffer, sizeof(writeBuffer)) != sizeof(writeBuffer)) {
        printf("Failed to write to the i2c bus.\r\n");
        return false;
    }
    return true;
}

void send_command(string command){
  ofstream file;
  file.open ("/tmp/touch/thumper");
  file << command << endl;
file.close();
}




int main(void) {
    int file;
    char filename[40];
    int slaveAddress = 0x1B;        // The 7-bit I2C address of the qt1070 emulator device

    sprintf(filename, "/dev/i2c-1");
    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    } else {
      printf("Succesfully openend i2c-1 bus\r\n");
    }

    if (ioctl(file, I2C_SLAVE, slaveAddress) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    } else {
      printf("Succesfully aquired i2c-1 bus\r\n");
    }

    sleep(1);

    printf("Writing register address\r\n");
    write_register_address(file, FIRMWARE);

    sleep(1);


    char chipid = 0x00;
    if (!read_register(file, CHIP_ID, &chipid)) {
      printf("Failed to read chip id\r\n");
      return -1;
    }
    printf("Chip id = 0x%x\r\n", chipid);

    //return 0;
    // BREAK ///////////////////////////////////////////////////////////////////

    sleep(1);

    char firmware = 0x00;
    if (!read_register(file, FIRMWARE, &firmware)) {
      printf("Failed to read firmware version\r\n");
      return -1;
    }
    printf("Firmware version = 0x%x\r\n", firmware);

    sleep(1);

    if (!reset_device(file)) {
      printf("Failed to reset device\r\n");
      return -1;
    }
    printf("Device is resetting\r\n");
    sleep(1);

    bool keepReading = true;
    while (keepReading) {
      char keys = 0x00;
      if (!read_register(file, KEY_STATUS, &keys)) {
        printf("Failed to read firmware version\r\n");
      }
      // printf("Key status = 0x%x\r\n", keys);

        switch (keys) {
          case UP:
          printf("UP ");
          send_command("double_effect");
          break;

          case LEFT:
          printf("LEFT ");
          send_command("left");
          break;
          case DOWN:
          printf("DOWN ");
          send_command("reverse");
          break;
          case RIGHT:
          printf("RIGHT ");
          send_command("right");
          break;

          case A:
          printf("A ");
          send_command("strobe");
          break;

          case B:
          printf("B ");
          send_command("dim");
          break;
          case X:
          printf("X ");
          send_command("special_effect");
          break;
        }
      fflush(stdout);

      usleep(100000);
    }

    printf("Done\r\n");

    return 0;
}
