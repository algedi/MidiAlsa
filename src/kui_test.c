/*
exec gcc $0 -Wall -O2 -o miditest -lasound
*/

#include <stdio.h>
#include <stdlib.h>
//#include <time.h>
#include <alsa/asoundlib.h>

int dest_client = 128;
int dest_port = 0;
snd_seq_t* t_seq;
int nIdPortOut; 
int nIdPort;

snd_seq_t* Connect(int *source_port) 
{
	printf("\n****\nConnect ...\n");
	snd_seq_t* s;
	
	if (snd_seq_open(&t_seq, "hw", SND_SEQ_OPEN_OUTPUT, 0) < 0) {
		printf("Error: snd_seq_open\n");
		return 0;
	}
	printf("\t snd_seq_open ==> OK\n");
	
	snd_seq_set_client_name(t_seq, "KUI_TEST");
	
	nIdPort = snd_seq_create_simple_port(t_seq, "KUI-out-midi", SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_MIDI_GENERIC /*|	SND_SEQ_PORT_TYPE_APPLICATION*/);	
	if (nIdPort < 0) { printf("Error: snd_seq_create_simple_port\n"); snd_seq_close(t_seq);	return 0;}
	
	printf("\t snd_seq_create_simple_port ==> OK prima\n");
	return t_seq;
	// printf("\t snd_seq_create_simple_port ==> OK dopo\n");

	/*
	if ((nIdPortOut = snd_seq_create_simple_port(*seq, "MIDI out",
					SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
					SND_SEQ_PORT_TYPE_APPLICATION)) < 0) 
	{
		fprintf(stderr, "Error creating sequencer port.\n");
	}

	int n = snd_seq_create_simple_port(s, "andrea",
		SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_MIDI_GENERIC |
		SND_SEQ_PORT_TYPE_APPLICATION);
		
	if (n < 0) 
	{
		printf("Error: snd_seq_create_simple_port\n");
		snd_seq_close(s);
		return 0;
	}
	 */


	/*
	if (snd_seq_connect_to(s, *source_port, dest_client, dest_port) < 0) 
	{
		printf("Error: snd_seq_connect_to\n");
		snd_seq_close(s);
		return 0;
	}
	 */
}

/*
int RandNum(int n) 
{
	printf("RandNum");
	return (int)(rand()/(RAND_MAX+1.0)*n);
}
 */

snd_seq_event_t ev;
snd_seq_t * seq;

void SendEv() 
{
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, nIdPort);
	//snd_seq_ev_set_dest(&ev, dest_client, dest_port);
	snd_seq_event_output(t_seq, &ev);
}

void NoteOn(int note) 
{
	printf("NOTA ON : %u \n", note);

	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, nIdPortOut);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);
	//ev.type = SND_SEQ_EVENT_NONE;

	snd_seq_ev_set_noteon(&ev, 0, note, 60);

	snd_seq_event_output(t_seq, &ev);
	/*
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, port);
	snd_seq_event_output(seq, &ev);
	*/
	//SendEv();
}

void NoteOff(int note) 
{
	printf("NOTA OFF: %u \n", note);

	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, nIdPortOut);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);

	snd_seq_ev_set_noteoff(&ev, 0, note, 60);
	
	snd_seq_event_output(t_seq, &ev);
	/*
	snd_seq_ev_set_direct(&ev);
	snd_seq_ev_set_source(&ev, port);
	snd_seq_event_output(seq, &ev);
	 */

	//SendEv();
}

int main(int argc, char ** argv) 
{
	// srand(time(0));
	
	printf("init ...");
	t_seq = Connect(&nIdPort);
	printf("\t Connect returned\n");
	
	if (!t_seq) 
	{
		printf("errore");
		return 1;
	}

	printf("INIT... while");
	while(1)
	{
		printf("    ... \n");
		NoteOn(0x3C);
		usleep(100000);
		NoteOn(0x3D);
		usleep(100000);
		NoteOn(0x3E);
		usleep(100000);
		snd_seq_drain_output(t_seq);
		usleep(100000);
		NoteOn(0x3C);
		usleep(100000);
		NoteOn(0x3D);
		usleep(100000);
		NoteOn(0x3E);
		snd_seq_drain_output(t_seq);
		usleep(100000);
	}
	snd_seq_close(t_seq);
	return 0;
}
