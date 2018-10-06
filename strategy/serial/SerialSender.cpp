
#include "SerialSender.hpp"

SerialSender::SerialSender(const char *serialPath, unsigned int baud) {

	this->serialDescriptor = open(serialPath, O_RDWR | O_NOCTTY | O_NDELAY);

    if (this->serialDescriptor < 0) {
         throw std::runtime_error("Port Failed to Open");
    }
	else {
		struct termios options;
		// Sets the read() function to return NOW and not wait for data to enter buffer if there isn't anything there.
		fcntl(this->serialDescriptor, F_SETFL, FNDELAY);

		tcgetattr(this->serialDescriptor, &options); //Gets the current options for the port

		// Selection of BAUD Rate
		switch (baud) {
			case 2400:
				cfsetispeed(&options, B2400);	//Sets the Input Baud Rate
				cfsetospeed(&options, B2400);	//Sets the Output Baud Rate
				break;
			case 4800:
				cfsetispeed(&options, B4800);
				cfsetospeed(&options, B4800);
				break;
			case 9600:
				cfsetispeed(&options, B9600);
				cfsetospeed(&options, B9600);
				break;
			case 19200:
				cfsetispeed(&options, B19200);
				cfsetospeed(&options, B19200);
				break;
			case 38400:
				cfsetispeed(&options, B38400);
				cfsetospeed(&options, B38400);
				break;
			case 57600:
				cfsetispeed(&options, B57600);
				cfsetospeed(&options, B57600);
				break;
			case 115200:
				cfsetispeed(&options, B115200);
				cfsetospeed(&options, B115200);
				break;
			default:
				cfsetispeed(&options, B9600);
				cfsetospeed(&options, B9600);
				break;
		}

		// Options for 8N1 serial operations
		options.c_cflag |= (CLOCAL | CREAD);		// turn on READ & ignore ctrl lines
		options.c_cflag &= ~PARENB;					// No parity bit
		options.c_cflag &= ~CSTOPB;					// One stop bit only
		options.c_cflag &= ~CSIZE;					// 8bit size
		options.c_cflag |= CS8;

		tcsetattr(this->serialDescriptor, TCSANOW, &options);			//Set the new options for the port "NOW"

	}
}

void SerialSender::serialclose() {
	if (this->serialDescriptor >= 0) {
        close(this->serialDescriptor);
    }
}

SerialSender::~SerialSender()
{
    this->serialclose();
}

void SerialSender::send(int vel_1l, int vel_1r, int vel_2l, int vel_2r, int vel_3l, int vel_3r) {
    dprintf(this->serialDescriptor, "[%d,%d,%d,%d,%d,%d]\n", vel_1l, vel_1r, vel_2l, vel_2r, vel_3l, vel_3r);
#ifdef DEBUG_PRINT
	printf("Sent %d %d %d %d %d %d\n", vel_1l, vel_1r, vel_2l, vel_2r, vel_3l, vel_3r);
#endif
}

int main() {
	SerialSender sender("/dev/ttyS5", 57600);
	sleep(1);

	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::minstd_rand0 randGenerator(seed);

	for (int i = 0; i < 100; i++) {
		int v = randGenerator()%256;
		sender.send(v, v, v, v, v, v);
		usleep(2000);
	}
    return 0;
}
