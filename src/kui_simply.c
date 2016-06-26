/* compile with -lasound */
#include <linux/input.h>
#include <alsa/asoundlib.h>
#include <fcntl.h>
#include <stdio.h>

#define INPUT_DEVICE "/dev/input/by-path/platform-i8042-serio-0-event-kbd"
#define CHANNEL 0
#define NOTE_A 48
#define NOTE_B 52
#define CONTROLLER_X 92

int main(void)
{
	//struct input_event ie;
	int nIdPortA;
	int nIdPortB;
	snd_seq_t *seq;
	snd_seq_event_t ev;

	/*
	fd = open(INPUT_DEVICE, O_RDONLY);
	if (fd == -1) {
		perror(INPUT_DEVICE);
		return 1;
	}
	 */
	
	int n = snd_seq_open(&seq, "default", SND_SEQ_OPEN_OUTPUT, 0);
	if (n < 0)	{ printf("cannot open sequencer\n"); return 1; }
	
	snd_seq_set_client_name(seq, "KUI_simply");
	nIdPortA = snd_seq_create_simple_port(seq, "Porta-A", SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_MIDI_GENERIC /* | SND_SEQ_PORT_TYPE_SOFTWARE*/);
	nIdPortB = snd_seq_create_simple_port(seq, "Porta-B", SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_MIDI_GENERIC /* | SND_SEQ_PORT_TYPE_SOFTWARE*/);
	
	n = 1;

	printf("ENTRY WHILE\n");
	while(1) 
	{
		//ev.type = SND_SEQ_EVENT_NONE;

		if(n == 1)
		{
			snd_seq_ev_clear(&ev);
			snd_seq_ev_set_source(&ev, nIdPortA);
			snd_seq_ev_set_subs(&ev);
			snd_seq_ev_set_direct(&ev);
			snd_seq_ev_set_noteon(&ev, 1, 0x3C, 0x7F);
			snd_seq_event_output_direct(seq, &ev);
		}
		else if(n == 2)
		{
			snd_seq_ev_clear(&ev);
			snd_seq_ev_set_source(&ev, nIdPortA);
			snd_seq_ev_set_subs(&ev);
			snd_seq_ev_set_direct(&ev);
			snd_seq_ev_set_noteoff(&ev, 1, 0x3C, 0x40);
			snd_seq_event_output_direct(seq, &ev);
		}
		else if(n == 3)
		{
			snd_seq_ev_clear(&ev);
			snd_seq_ev_set_source(&ev, nIdPortB);
			snd_seq_ev_set_subs(&ev);
			snd_seq_ev_set_direct(&ev);
			snd_seq_ev_set_noteon(&ev, 2, 0x3D, 0x7F);
			snd_seq_event_output_direct(seq, &ev);
		}
		else if(n == 4)
		{
			snd_seq_ev_clear(&ev);
			snd_seq_ev_set_source(&ev, nIdPortB);
			snd_seq_ev_set_subs(&ev);
			snd_seq_ev_set_direct(&ev);
			snd_seq_ev_set_noteoff(&ev, 2, 0x3D, 0x40); // CANALE, NOTA, VELOCITA'
			snd_seq_event_output_direct(seq, &ev);
		}
		else
			n = 0;
		
		usleep(100000);
		n++;
		printf("note \n");

		//if (ev.type != SND_SEQ_EVENT_NONE)
	}
	snd_seq_close(seq);
	return 0;
}
