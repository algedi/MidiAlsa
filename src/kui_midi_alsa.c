#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <argp.h>
#include <alsa/asoundlib.h>
#include <signal.h>
#include <pthread.h>
// Linux-specific
#include <linux/serial.h>
#include <linux/ioctl.h>
#include <asm/ioctls.h>

#define FALSE 0
#define TRUE  1

int zRun;
int nIdPortOutA, nIdPortOutB, nIdPortOutC;
int nIdPortInA,  nIdPortInB,  nIdPortInC;
snd_seq_t* t_seq;

void exit_cli(int sig)
{
	zRun = FALSE;
	printf("\t KUI_MIDI_ALSA shutdown ... ");
}

void OpenSeq() 
{
	int nIdPort;

	nIdPort = snd_seq_open(&t_seq, "default", SND_SEQ_OPEN_DUPLEX, 0); 
	if ( nIdPort < 0 ) { printf("Error opening ALSA sequencer.\n"); exit(1); }

	snd_seq_set_client_name(t_seq, "KUI_midi_alsa");

	nIdPortOutA = snd_seq_create_simple_port(t_seq, "MIDI_A", SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_APPLICATION); 
	if ( nIdPortOutA < 0 ) { printf( "Error creating sequencer port.\n"); }
	nIdPortOutB = snd_seq_create_simple_port(t_seq, "MIDI_B", SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_APPLICATION); 
	if ( nIdPortOutB < 0 ) { printf( "Error creating sequencer port.\n"); }
	nIdPortOutC = snd_seq_create_simple_port(t_seq, "MIDI_C", SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_APPLICATION); 
	if ( nIdPortOutC < 0 ) { printf( "Error creating sequencer port.\n"); }
	nIdPortInA = snd_seq_create_simple_port(t_seq, "MIDI_A",	SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,	SND_SEQ_PORT_TYPE_APPLICATION); 
	if ( nIdPortInA < 0 )	{ printf("Error creating sequencer port.\n");}
	nIdPortInB = snd_seq_create_simple_port(t_seq, "MIDI_B",	SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,	SND_SEQ_PORT_TYPE_APPLICATION); 
	if ( nIdPortInB < 0 )	{ printf("Error creating sequencer port.\n");}
	nIdPortInC = snd_seq_create_simple_port(t_seq, "MIDI_C",	SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,	SND_SEQ_PORT_TYPE_APPLICATION); 
	if ( nIdPortInC < 0 )	{ printf("Error creating sequencer port.\n");}

	return;
}

void SendMidiCommand(int nIdPortOut, char *buf)
{
	/*
	   MIDI COMMANDS
	   -------------------------------------------------------------------
	   name                 status      param 1          param 2
	   -------------------------------------------------------------------
	   note off             0x80+C       key #            velocity
	   note on              0x90+C       key #            velocity
	   poly key pressure    0xA0+C       key #            pressure value
	   control change       0xB0+C       control #        control value
	   program change       0xC0+C       program #        --
	   mono key pressure    0xD0+C       pressure value   --
	   pitch bend           0xE0+C       range (LSB)      range (MSB)
	   system               0xF0+C       manufacturer     model
	   -------------------------------------------------------------------
	   C is the channel number, from 0 to 15;
	   -------------------------------------------------------------------
	   source: http://ftp.ec.vanderbilt.edu/computermusic/musc216site/MIDI.Commands.html
	
	   In this program the pitch bend range will be transmitter as 
	   one single 8-bit number. So the end result is that MIDI commands 
	   will be transmitted as 3 bytes, starting with the operation byte:
	
	   buf[0] --> operation/channel
	   buf[1] --> param1
	   buf[2] --> param2        (param2 not transmitted on program change or key press)


	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, nIdPortOut);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);

	snd_seq_ev_set_noteoff(&ev, 0, note, 60);
	
	snd_seq_event_output(t_seq, &ev);


   */

	snd_seq_event_t t_event;
	snd_seq_ev_clear(&t_event);
									//			printf("Serial  port: %u\n", nIdPortOut);
	snd_seq_ev_set_source(&t_event, nIdPortOutA);
	snd_seq_ev_set_subs(&t_event);
	snd_seq_ev_set_direct(&t_event);

	int operation, channel, param1, param2;

	operation = buf[0] & 0xF0;
	channel   = buf[0] & 0x0F;
	param1    = buf[1];
	param2    = buf[2];

	switch (operation)
	{
		case 0x80:
			printf("OUT   ==> 0x%x Note off           0x%x 0x%x 0x%x\n", operation, channel, param1, param2);
			snd_seq_ev_set_noteoff(&t_event, channel, param1, param2);
			break;
			
		case 0x90:
			printf("OUT   ==> 0x%x Note on            0x%x 0x%x 0x%x\n", operation, channel, param1, param2);
			snd_seq_ev_set_noteon(&t_event, channel, param1, param2);
			break;
			
		case 0xA0:
			printf("OUT   ==> 0x%x Pressure change    0x%x 0x%x 0x%x\n", operation, channel, param1, param2);
			snd_seq_ev_set_keypress(&t_event, channel, param1, param2);
			break;

		case 0xB0:
			printf("OUT   ==> 0x%x Controller change  0x%x 0x%x 0x%x\n", operation, channel, param1, param2);
			snd_seq_ev_set_controller(&t_event, channel, param1, param2);
			break;

		case 0xC0:
			printf("OUT   ==> 0x%x Program change     0x%x 0x%x 0x%x\n", operation, channel, param1);
			snd_seq_ev_set_pgmchange(&t_event, channel, param1);
			break;

		case 0xD0:
			printf("OUT   ==> 0x%x Channel change     0x%x 0x%x 0x%x\n", operation, channel, param1);
			snd_seq_ev_set_chanpress(&t_event, channel, param1);
			break;

		case 0xE0:
			param1 = (param1 & 0x7F) + ((param2 & 0x7F) << 7);
			printf("OUT   ==> 0x%x Pitch bend         %03u %05i\n", operation, channel, param1);
			snd_seq_ev_set_pitchbend(&t_event, channel, param1 - 8192); // in alsa MIDI we want signed int
			break;

		/* Not implementing system commands (0xF0) */
			
		default:
			printf("OUT       0x%x Unknown MIDI cmd   %03u %03u %03u\n", operation, channel, param1, param2);
			break;
	}

	snd_seq_event_output_direct(t_seq, &t_event);
	//snd_seq_drain_output(t_seq);
	//snd_seq_event_output(t_seq, &t_event);

}

void PrintMidiIn() 
{
	snd_seq_event_t* t_event;
	char bytes[] = {0x00, 0x00, 0xFF}; 

	do 
	{
		snd_seq_event_input(t_seq, &t_event);

		//printf("t_event->source.port: %u\n", t_event->source.port); 
		//printf("t_event->source.client: %u\n", t_event->source.client); 
		if     (t_event->data.addr.port == nIdPortInA) printf("PORTA AA\n");
		else if(t_event->data.addr.port == nIdPortInB) printf("PORTA BB\n");
		else if(t_event->data.addr.port == nIdPortInC) printf("PORTA CC\n");
		else printf("ev->data.addr.port: %u\n", t_event->data.addr.port); 
		//printf("ev->data.addr.client: %u\n", t_event->data.addr.client); 
		if     (t_event->dest.port == nIdPortInA) printf("PORTA A\n");
		else if(t_event->dest.port == nIdPortInB) printf("PORTA B\n");
		else if(t_event->dest.port == nIdPortInC) printf("PORTA C\n");
		else printf("t_event->dest.port: %u\n", t_event->data.addr.port); 

		switch (t_event->type) 
		{

			case SND_SEQ_EVENT_NOTEOFF: 
				bytes[0] = 0x80 + t_event->data.control.channel;
				bytes[1] = t_event->data.note.note;
				bytes[2] = t_event->data.note.velocity; 
				printf("IN     ==> 0x%x Note off           0x%x 0x%x 0x%x\n", bytes[0]&0xF0, bytes[0]&0xF, bytes[1], bytes[2]); 
				break; 

			case SND_SEQ_EVENT_NOTEON:
				bytes[0] = 0x90 + t_event->data.control.channel;
				bytes[1] = t_event->data.note.note;
				bytes[2] = t_event->data.note.velocity;        
				printf("IN     ==> 0x%x Note on            0x%x 0x%x 0x%x\n", bytes[0]&0xF0, bytes[0]&0xF, bytes[1], bytes[2]); 
				break;        

			case SND_SEQ_EVENT_KEYPRESS: 
				bytes[0] = 0x90 + t_event->data.control.channel;
				bytes[1] = t_event->data.note.note;
				bytes[2] = t_event->data.note.velocity;        
				printf("IN     ==> 0x%x Pressure change    0x%x 0x%x 0x%x\n", bytes[0]&0xF0, bytes[0]&0xF, bytes[1], bytes[2]); 
				break;       

			case SND_SEQ_EVENT_CONTROLLER: 
				bytes[0] = 0xB0 + t_event->data.control.channel;
				bytes[1] = t_event->data.control.param;
				bytes[2] = t_event->data.control.value;
				printf("IN     ==> 0x%x Controller change  0x%x 0x%x 0x%x\n", bytes[0]&0xF0, bytes[0]&0xF, bytes[1], bytes[2]); 
				break;   

			case SND_SEQ_EVENT_PGMCHANGE: 
				bytes[0] = 0xC0 + t_event->data.control.channel;
				bytes[1] = t_event->data.control.value;
				printf("IN     ==> 0x%x Program change     0x%x 0x%x 0x%x\n", bytes[0]&0xF0, bytes[0]&0xF, bytes[1], bytes[2]); 
				break;  

			case SND_SEQ_EVENT_CHANPRESS: 
				bytes[0] = 0xD0 + t_event->data.control.channel;
				bytes[1] = t_event->data.control.value;
				printf("IN     ==> 0x%x Channel change     0x%x 0x%x 0x%x\n", bytes[0]&0xF0, bytes[0]&0xF, bytes[1], bytes[2]); 
				break;  

			case SND_SEQ_EVENT_PITCHBEND:
				bytes[0] = 0xE0 + t_event->data.control.channel;
				t_event->data.control.value += 8192;
				bytes[1] = (int)t_event->data.control.value & 0x7F;
				bytes[2] = (int)t_event->data.control.value >> 7;
				printf("IN     ==> 0x%x Pitch bend         %03u %5d\n", bytes[0]&0xF0, bytes[0]&0xF, t_event->data.control.value);
				break;

			default:
				break;
		}

		snd_seq_free_event(t_event);

	} while (snd_seq_event_input_pending(t_seq, 0) > 0);
}


void* ReadMidi(void* seq) 
{
	int nPfd;
	struct pollfd* t_pfd;

	nPfd = snd_seq_poll_descriptors_count(t_seq, POLLIN);
	t_pfd = (struct pollfd*) alloca(nPfd * sizeof(struct pollfd));
	snd_seq_poll_descriptors(t_seq, t_pfd, nPfd, POLLIN);	

	while (zRun) 
		if (poll(t_pfd, nPfd, 100) > 0) 
			PrintMidiIn();

	printf("\n[IN  ALSA]->process communication shutdown ...\n");
}

void* WriteMidi(void* seq) 
{
	char buf[3];
	buf[0] = 0x80;
	buf[1] = 0x20;
	buf[2] = 0x00;
	while(zRun)
	{
		buf[0] = 0x80;
		buf[1] = 0x40;
		buf[2] = 0x11;
		SendMidiCommand(nIdPortOutA, buf);
		sleep(1);

		buf[0] = 0x90;
		buf[1] = 0x40;
		buf[2] = 0x22;
		SendMidiCommand(nIdPortOutA, buf);
		sleep(1);
		int n;
		for(n = 0; n < 127; n++)
		{
			buf[0] = 0xB0;
			buf[1] = 100;
			buf[2] = n;
			SendMidiCommand(nIdPortOutA, buf);
			usleep(10000);
		}
	}

	printf("\n[OUT ALSA]->process communication stoping ...");
}

main(int argc, char** argv)
{
	printf("INIT ... \n");
	OpenSeq();

	pthread_t pThreadMidiOutA, pThreadMidiInA;
	int nIretOut, nIretIn;
	zRun = TRUE;
	nIretOut = pthread_create(&pThreadMidiOutA, NULL, WriteMidi,     (void*) t_seq);
	nIretIn  = pthread_create(&pThreadMidiInA,  NULL, ReadMidi,		 (void*) t_seq);

	signal(SIGINT,  exit_cli);
	signal(SIGTERM, exit_cli);

	while (zRun)
	{   
		printf("WHILE\n");
		sleep(100);
	}

	// Attesa del termine dei thread
	void* pStatusOut;
	pthread_join(pThreadMidiOutA, &pStatusOut);
	void* pStatusIn;
	pthread_join(pThreadMidiInA, &pStatusIn);

	printf("\nFATTO!\n");
}

