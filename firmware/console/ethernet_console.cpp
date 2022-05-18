#include "pch.h"

#if EFI_ETHERNET

#include "lwipthread.h"

#include "lwip/sockets.h"

#include "thread_controller.h"
#include "tunerstudio.h"

static int listenerSocket = -1;
static int connectionSocket = -1;

static void do_connection() {
	if (connectionSocket != -1) {
		auto localCopy = connectionSocket;
		connectionSocket = -1;

		lwip_close(localCopy);
	}

	sockaddr_in remote;
	socklen_t size = sizeof(remote);
	connectionSocket = lwip_accept(listenerSocket, (sockaddr*)&remote, &size);
}

class EthernetChannel : public TsChannelBase {
public:
	EthernetChannel()
		: TsChannelBase("Ethernet")
	{
	}

	bool isReady() const override {
		return connectionSocket != -1;
	}

	void write(const uint8_t* buffer, size_t size, bool isEndOfPacket) override {
		// If not the end of a packet, set the MSG_MORE flag to indicate to the transport
		// that we have more to add to the buffer before queuing a flush.
		auto flags = isEndOfPacket ? 0 : MSG_MORE;
		lwip_send(connectionSocket, buffer, size, flags);
	}

	size_t readTimeout(uint8_t* buffer, size_t size, int /*timeout*/) override {
		auto result = lwip_recv(connectionSocket, buffer, size, /*flags =*/ 0);

		if (result == -1) {
			do_connection();
			return 0;
		}

		return result;
	}
};

static EthernetChannel ethChannel;

struct EthernetThread : public TunerstudioThread {
	EthernetThread() : TunerstudioThread("Ethernet Console") { }

	TsChannelBase* setupChannel() override {
		lwipInit(nullptr);

		sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_port = htons(29000);
		address.sin_addr.s_addr = INADDR_ANY;

		listenerSocket = lwip_socket(AF_INET, SOCK_STREAM, 0);
		lwip_bind(listenerSocket, (sockaddr*)&address, sizeof(address));
		lwip_listen(listenerSocket, 1);

		do_connection();

		return &ethChannel;
	}
};

static EthernetThread ethernetConsole;

void startEthernetConsole() {
#ifndef STM32H7
	// TODO: why does this break H7? I thought the pins were the same?
	efiSetPadMode("ethernet",  Gpio::A1, PAL_MODE_ALTERNATE(0xb));
	efiSetPadMode("ethernet",  Gpio::A2, PAL_MODE_ALTERNATE(0xb));
	efiSetPadMode("ethernet",  Gpio::A7, PAL_MODE_ALTERNATE(0xb));

	efiSetPadMode("ethernet",  Gpio::C1, PAL_MODE_ALTERNATE(0xb));
	efiSetPadMode("ethernet",  Gpio::C4, PAL_MODE_ALTERNATE(0xb));
	efiSetPadMode("ethernet",  Gpio::C5, PAL_MODE_ALTERNATE(0xb));

	efiSetPadMode("ethernet",  Gpio::D5, PAL_MODE_ALTERNATE(0xb));

	efiSetPadMode("ethernet", Gpio::G11, PAL_MODE_ALTERNATE(0xb));
	efiSetPadMode("ethernet", Gpio::G13, PAL_MODE_ALTERNATE(0xb));
	efiSetPadMode("ethernet", Gpio::G14, PAL_MODE_ALTERNATE(0xb));
#endif // STM32H7

	ethernetConsole.Start();
}

#endif // EFI_ETHERNET
