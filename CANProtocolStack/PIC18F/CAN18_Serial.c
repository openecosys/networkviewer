#include "CAN18_Serial.h"
#include "CAN18_Device.h"
#include "CAN18_Shared.h"
#include "CAN18_Utils.h"
#include <usart.h>
#include <delays.h>


//memory buffer
unsigned char g_recvDataBytes[RX_BUFFER_SIZE];

unsigned int g_availableBytes = 0;
unsigned int g_readIndex = 0;
unsigned int g_writeIndex = 0;


unsigned char serial_calculate_checksum(const CANSerialMessage *message)
{
	unsigned char checksum = 0;
	int i = 0;

	//simple accumulation of bytes from start until checksum (excluded)
	for (i = 0; i < (sizeof(CANSerialMessage) - 1); i++)
	{
		checksum += message->messageBytes[i];
	}
	
	return checksum;
	
}


void serial_usart_interrupt_handler(void)
{
		//TODO, HANDLE UART1 & UART2

	    //When RCREG is read it will automatically clear the RCIF flag
        unsigned char value = getcUSART();

		//store the data
        g_recvDataBytes[g_writeIndex++] = value;

		//circular buffer
		if (g_writeIndex >= RX_BUFFER_SIZE)
		{
			g_writeIndex = 0;
		}
        
		//One more byte available
		g_availableBytes++;
}

unsigned int serial_bytes_available(void)
{
	unsigned int available = 0;
	INTCONbits.GIEH = 0; //disable interrupts
	available = g_availableBytes;
	INTCONbits.GIEH = 1; //enable interrupts
	return available;
}

//////////////////////////////////////////////////////////////////////
//   can_send_message
//////////////////////////////////////////////////////////////////////
//
//   Description: Fills a TX buffer with a modified message and sends
//                a Request To Send.
//
//   Input: object_id specifying the TX buffer to use
//   Output: NONE
//   Input/Output: message
//   Returned value: NONE
//
//////////////////////////////////////////////////////////////////////

unsigned char can_send_message(CAN_MESSAGE *message)
{
    unsigned char i=0;

    //Need to transform a TxMessageBuffer into a CAN_MESSAGE
    CANSerialMessage buf;

/**
		unsigned char start_byte;
		unsigned char pri_boot_rtr;
		unsigned char type;
		unsigned char cmd;
		unsigned char dest;
		unsigned char data_length_iface;
		unsigned char data[8];
		unsigned char checksum;
*/

	//Set fields
	buf.start_byte = START_BYTE;
	buf.pri_boot_rtr = (message->msg_priority << 5) | (message->msg_read_write << 4) | (message->msg_eeprom_ram << 3) | (message->msg_remote);
	buf.type = message->msg_type;
	buf.cmd = message->msg_cmd;
	buf.dest = message->msg_dest;
	buf.data_length_iface = (message->msg_data_length <<4);

    //copy data
    for (i = 0 ; i < 8; i++)
    {
        buf.data[i] = message->msg_data[i];
    }

	//calculate checksum
	buf.checksum = serial_calculate_checksum(&buf);


    //Transmit on serial
    //Right now it will be synchronous, need to be event based with interrupts
    for (i = 0; i < sizeof(CANSerialMessage); i++)
    {
        putcUSART(buf.messageBytes[i]);
    }

    return 1;
}

//////////////////////////////////////////////////////////////////////
//   can_recv_message
//////////////////////////////////////////////////////////////////////
//
//   Description: Extract RX buffer message and put it in a message
//                structure.
//
//   Input: object_id specifying the TX buffer to use
//   Output: NONE
//   Input/Output: message
//   Returned value: NONE
//
//////////////////////////////////////////////////////////////////////

unsigned char can_recv_message(CAN_MESSAGE *message)
{
    unsigned char i = 0;
    CANSerialMessage buf;

	if(serial_bytes_available() >= sizeof(CANSerialMessage))
	{
		//Fill CANSerialMessage with available bytes
		//TODO Memory boundaries verification?
		//Here we assume that we have enough buffer to avoid corruption
		if (g_recvDataBytes[g_readIndex] == START_BYTE)
		{
			for (i = 0; i < sizeof(CANSerialMessage); i++)
			{
				//Copy byte
				buf.messageBytes[i] = g_recvDataBytes[g_readIndex];

				//Increment read index
				g_readIndex = (g_readIndex + 1) % RX_BUFFER_SIZE;
			}


			//One less message available
			INTCONbits.GIEH = 0; //disable interrupts
			g_availableBytes -= sizeof(CANSerialMessage);
			INTCONbits.GIEH = 1; //enable interrupts


			//Verify checksum
			if (buf.checksum == serial_calculate_checksum(&buf))
			{
				/**
					unsigned char start_byte;
					unsigned char pri_boot_rtr;
					unsigned char type;
					unsigned char cmd;
					unsigned char dest;
					unsigned char data_length_iface;
					unsigned char data[8];
					unsigned char checksum;
				*/

				//Convert data structures
				message->msg_priority = (buf.pri_boot_rtr >> 5) & 0x07;
				message->msg_type = buf.type;
				message->msg_eeprom_ram = (buf.pri_boot_rtr >> 4) & 0x01;
				message->msg_read_write = (buf.pri_boot_rtr >> 3) & 0x01;
				message->msg_cmd = buf.cmd;
				message->msg_dest = buf.dest;
				message->msg_remote = (buf.pri_boot_rtr & 0x01);
				message->msg_data_length = (buf.data_length_iface) & 0x0F;
				
				//copy data
				for(i = 0; i < 8; i++)
				{
				   message->msg_data[i] = buf.data[i];
				}
				
				//message valid
				return 1;
			}
			else //CHECKSUM ERROR
			{
				return 0;
			}
		}
		else //not START_BYTE
		{
			//Will try next byte next time...
			g_readIndex = (g_readIndex + 1) % RX_BUFFER_SIZE;
			
			//One less byte available
			INTCONbits.GIEH = 0; //disable interrupts
			g_availableBytes--;
			INTCONbits.GIEH = 1; //enable interrupts

			return 0;
		}
	}//enough byte available
	
	return 0;
}







