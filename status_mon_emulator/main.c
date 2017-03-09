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
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

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

        //if (tcsetattr (fd, TCSANOW, &tty) != 0)
                //error_message ("error %d setting term attributes", errno);
}

int open_serial(const char* portname)
{
	int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);

	if (fd < 0) {
		return -1;
	}

	set_interface_attribs(fd, B9600, 0);
	set_blocking (fd, 0);

	return fd;
}

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

int main(int argc, char* argv[])
{
	char* portname = "/dev/ttyUSB0";

	if (argc > 1) {
		portname = argv[1];
	}

	int serial_fd = open_serial(portname);

	if (serial_fd < 0) {
		printf("Failed to open %s\n", portname);
		exit(1);
	}

	simulate();


	while (1) {
//		printf("Sending request...\n");

//		unsigned char cmd = 0x1C;

//		write(serial_fd, &cmd, sizeof(cmd));

		char buf[4] = {0};

		read(serial_fd, &buf, sizeof(buf));
		printf("Read: \n%s\n", buf);

//		read(serial_fd, &buf, sizeof(buf));
//		printf("Read: %s\n", buf);


//		unsigned int num;
//		memcpy(&num, buf, 4);
//		unsigned int num_mant;
//		memcpy(&num_mant, buf + 4, 4);

//		printf("Read data: %u\n\n", num);

		sleep(1);
	}

	return 0;
}


