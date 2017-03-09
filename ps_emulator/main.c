#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
 
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing

        tty.c_oflag = 0;                // no remapping, no delays

        tty.c_cc[VMIN]  = 3;            // read doesn't block
        tty.c_cc[VTIME] = 0;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

		tcflush(fd, TCIFLUSH);

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                //error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tcsetattr (fd, TCSANOW, &tty);
                //error_message ("error %d setting term attributes", errno);
}

int open_serial(const char* portname)
{
	int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);

	if (fd < 0) {
		return -1;
	}

	set_interface_attribs(fd, B9600, 0);
//	set_blocking (fd, 0);

	return fd;
}

/*
void simulate()
{
	unsigned long v_ref = 4.82;
	uint32_t adc_conv_s = 1024;
	uint32_t amper_volt_zero = adc_conv_s / 2;
	uint32_t v_per_amp = 185;

	uint32_t adc_res = 527; //adc read
	double real_ra_voltage = (adc_res * v_ref) / (double)adc_conv_s;

	uint32_t real_ra_voltage_intpart = (uint32_t)real_ra_voltage;
	uint32_t real_ra_voltage_decpart = (real_ra_voltage - real_ra_voltage_intpart) * 1000;

	printf("real_ra_voltage = %2.4f\n", real_ra_voltage);
	printf("real_ra_voltage_decpart = %u\n", real_ra_voltage_decpart);
}
*/

void read_buf(int fd, int need_to_read, char* buf)
{
	int buf_pos = 0;

	while (need_to_read) {
		size_t rd = read(fd, &buf[buf_pos], 1);

		need_to_read -= rd;
		buf_pos++;
	}
}

#define CHECKSUMM_MAGIC_CORRECTION 5
#define CHECKSUMM_SIMPLE(A, B) (A + B - CHECKSUMM_MAGIC_CORRECTION)

int main(int argc, char* argv[])
{
	char* portname = "/dev/ttyUSB1";

	if (argc > 1) {
		portname = argv[1];
	}

	int serial_fd = open_serial(portname);

	if (serial_fd < 0) {
		printf("Failed to open %s\n", portname);
		exit(1);
	}

	uint8_t buf[3] = {0};

	while (1) {
		printf("Waiting for PS_ACTIVATE cmd...\n");

		read(serial_fd, buf, sizeof(buf));

		printf("rcv: 0x%2X 0x%2X 0x%2X\n", buf[0], buf[1], buf[2]);

		if (CHECKSUMM_SIMPLE(buf[0], buf[1]) != buf[2]) {
			printf("Invalid checksum!\n");
			continue;
		} else {
			printf("Cheksum is correct\n");
		}

		if (buf[1] == 0x1A) {
			printf("PS ACTIVATED\n");
			break;
		} else {
			printf("Invalid command!\n");	
		}
	}

	memset(buf, 0, sizeof(buf));

	buf[0] = 0x1A;
	buf[1] = 0xAC;
	buf[2] = CHECKSUMM_SIMPLE(0x1A, 0xAC);

	printf("snd: 0x%2X 0x%2X 0x%2X\n", buf[0], buf[1], buf[2]);

	write(serial_fd, &buf, sizeof(buf));

////

	while (1) {
		printf("Waiting for PS_ENABLE cmd...\n");

		read(serial_fd, buf, sizeof(buf));

		printf("rcv: 0x%2X 0x%2X 0x%2X\n", buf[0], buf[1], buf[2]);

		if (CHECKSUMM_SIMPLE(buf[0], buf[1]) != buf[2]) {
			printf("Invalid checksum!\n");
			continue;
		} else {
			printf("Cheksum is correct\n");
		}

		if (buf[1] == 0x2C) {
			printf("PS ENABLED\n");
			break;
		} else {
			printf("Invalid command!\n");	
		}
	}

	memset(buf, 0, sizeof(buf));

	buf[0] = 0x2C;
	buf[1] = 0xAC;
	buf[2] = CHECKSUMM_SIMPLE(0x2C, 0xAC);

	printf("snd: 0x%2X 0x%2X 0x%2X\n", buf[0], buf[1], buf[2]);

	write(serial_fd, &buf, sizeof(buf));

	printf("\n");

/// ping pong
	while (1) {
		memset(buf, 0, sizeof(buf));

		read(serial_fd, buf, sizeof(buf));

		printf("rcv: 0x%2X 0x%2X 0x%2X\n", buf[0], buf[1], buf[2]);

		if (CHECKSUMM_SIMPLE(buf[0], buf[1]) != buf[2]) {
			printf("Invalid checksum!\n");
			continue;
		} else {
			printf("Cheksum is correct\n");
		}

		if (buf[1] == 0x15) {
			printf("Got PING\n");
		}
 
		memset(buf, 0, sizeof(buf));

		buf[0] = 0xAC;
		buf[1] = 0x16;
		buf[2] = CHECKSUMM_SIMPLE(0xAC, 0x16);

		printf("snd: 0x%2X 0x%2X 0x%2X\n", buf[0], buf[1], buf[2]);

		write(serial_fd, &buf, sizeof(buf));

		printf("Sent PONG\n");
	}

	return 0;
}


