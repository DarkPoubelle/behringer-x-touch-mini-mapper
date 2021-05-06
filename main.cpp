/*
    Copyright (C) 2004 Ian Esten

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <jack/jack.h>
#include <jack/midiport.h>

//MIDI FRAMES
#define NOTE_ON 154
#define NOTE_OFF 138
#define CC 186

//NOTES AND CCs on controller
#define BT_LOAD 22
#define BT_SAVE 23
#define BT_EQ 16
#define BT_CHORUS 17
#define BT_PHASER 18
#define BT_TREMOLO 19
#define BT_REVERB 20
#define BT_DELAY 21
#define MASTER_FADER 9
#define GAIN_POT 1
#define COMP_POT 3
#define NOISE_GATE_POT 2
#define VOLUME_POT 4
#define BT_NOISE_GATE 9
#define BT_COMP 10
#define BT_VOLUME 11
#define GAIN_RST 0
#define NOISE_GATE_RST 1
#define COMP_RST 2
#define VOLUME_RST 3

//RETURNS SIGNALS
#define RT_POT_1 9
#define RT_POT_2 10
#define RT_POT_3 11
#define RT_POT_4 12
#define RT_POT_5 13
#define RT_POT_6 14
#define RT_POT_7 15
#define RT_POT_8 16
#define RT_BT_1 0
#define RT_BT_2 1
#define RT_BT_3 2
#define RT_BT_4 3
#define RT_BT_5 4
#define RT_BT_6 5
#define RT_BT_7 6
#define RT_BT_8 7
#define RT_BT_9 8
#define RT_BT_10 9
#define RT_BT_11 10
#define RT_BT_12 11
#define RT_BT_13 12
#define RT_BT_14 13
#define RT_BT_15 14
#define RT_BT_16 15

//GX CCs
#define GX_GAIN 1
#define GX_NOISE_GATE 2
#define GX_NOISE_GATE_SW 3
#define GX_COMP 4
#define GX_COMP_SW 5
#define GX_VOLBOOST 6
#define GX_VOLBOOST_SW 7
#define GX_MASTER_VOLUME 8


//DEFAULTS VALUES
#define DEFAULT_GAIN 69
#define DEFAULT_VOLUME 69

enum mode{MAIN,EQ,CHORUS,TREMOLO,PHASER,REVERB,DELAY};
mode current_mode = MAIN;


//Settings variables

//Main parameters
uint8_t s_gain = DEFAULT_GAIN;
uint8_t s_gain_rst;
uint8_t s_noise_gate;
uint8_t s_noise_gate_rst;
uint8_t s_comp;
uint8_t s_comp_rst;
uint8_t s_main_sw;
uint8_t s_noise_gate_sw;
uint8_t s_comp_sw;
uint8_t s_vol;
uint8_t s_vol_sw;
uint8_t s_vol_rst;
uint8_t s_master_vol = DEFAULT_VOLUME;

//FX1 - Eq
uint8_t s_eq_sw1;
uint8_t s_eq_sw2;
uint8_t s_eq_sw3;
uint8_t s_eq_sw4;

uint8_t s_eq_trim1;
uint8_t s_eq_trim2;
uint8_t s_eq_trim3;
uint8_t s_eq_trim4;

uint8_t s_eq_trim1_rst;
uint8_t s_eq_trim2_rst;
uint8_t s_eq_trim3_rst;
uint8_t s_eq_trim4_rst;

//FX2 - Chorus
uint8_t s_chorus_sw1;
uint8_t s_chorus_sw2;
uint8_t s_chorus_sw3;
uint8_t s_chorus_sw4;

uint8_t s_chorus_trim1;
uint8_t s_chorus_trim2;
uint8_t s_chorus_trim3;
uint8_t s_chorus_trim4;

uint8_t s_chorus_trim1_rst;
uint8_t s_chorus_trim2_rst;
uint8_t s_chorus_trim3_rst;
uint8_t s_chorus_trim4_rst;


//FX3 - Phaser
uint8_t s_phaser_sw1;
uint8_t s_phaser_sw2;
uint8_t s_phaser_sw3;
uint8_t s_phaser_sw4;

uint8_t s_phaser_trim1;
uint8_t s_phaser_trim2;
uint8_t s_phaser_trim3;
uint8_t s_phaser_trim4;

uint8_t s_phaser_trim1_rst;
uint8_t s_phaser_trim2_rst;
uint8_t s_phaser_trim3_rst;
uint8_t s_phaser_trim4_rst;

//FX4 - Tremolo
uint8_t s_trem_sw1;
uint8_t s_trem_sw2; //incremental
uint8_t s_trem_sw3;
uint8_t s_trem_sw4;

uint8_t s_trem_trim1;
uint8_t s_trem_trim2;
uint8_t s_trem_trim3;
uint8_t s_trem_trim4;

uint8_t s_trem_trim1_rst;
uint8_t s_trem_trim2_rst;
uint8_t s_trem_trim3_rst;
uint8_t s_trem_trim4_rst;

//FX5 - Reverb
uint8_t s_reverb_sw1;
uint8_t s_reverb_sw2;
uint8_t s_reverb_sw3;
uint8_t s_reverb_sw4;

uint8_t s_reverb_trim1;
uint8_t s_reverb_trim2;
uint8_t s_reverb_trim3;
uint8_t s_reverb_trim4;

uint8_t s_reverb_trim1_rst;
uint8_t s_reverb_trim2_rst;
uint8_t s_reverb_trim3_rst;
uint8_t s_reverb_trim4_rst;

//FX6 - Delay
uint8_t s_delay_sw1;
uint8_t s_delay_sw2; // incremental
uint8_t s_delay_sw3;
uint8_t s_delay_sw4;

uint8_t s_delay_trim1;
uint8_t s_delay_trim2;
uint8_t s_delay_trim3;
uint8_t s_delay_trim4;

uint8_t s_delay_trim1_rst;
uint8_t s_delay_trim2_rst;
uint8_t s_delay_trim3_rst;
uint8_t s_delay_trim4_rst;



jack_port_t *mapper_in_port; //input
jack_port_t *mapper_output_port; //output
jack_port_t *guitarix_send_port; //output
jack_port_t *guitarix_return_port; // input

jack_default_audio_sample_t ramp=0.0;
jack_default_audio_sample_t note_on;

jack_client_t *client;



int process(jack_nframes_t nframes, void *arg)
{


    uint8_t midi_signal;
    uint8_t midi_note_cc;
    uint8_t midi_value;

    jack_midi_event_t in_map_event;
    jack_midi_event_t in_gx_event;

    jack_nframes_t map_event_count;
    jack_nframes_t gx_event_count;

    jack_position_t pos;
    jack_nframes_t i;


    //Buffers
    void * mapper_in = jack_port_get_buffer(mapper_in_port, nframes);
    void * mapper_out = jack_port_get_buffer(mapper_output_port, nframes);
    void * guitarix_send = jack_port_get_buffer(guitarix_send_port, nframes);
    void * guitarix_return = jack_port_get_buffer(guitarix_return_port, nframes);


    //Nb d'events dans les buffers d'entrées
    map_event_count = jack_midi_get_event_count(mapper_in);
    gx_event_count = jack_midi_get_event_count(guitarix_return);

    jack_transport_query(client, &pos);

    midi_signal = 0;
    midi_note_cc = 0;
    midi_value = 0;

    jack_midi_clear_buffer(mapper_out);
    jack_midi_clear_buffer(guitarix_send);

    //Events on
    for (i = 0 ; i < map_event_count; i++)
    {
        jack_midi_event_get(&in_map_event, mapper_in, i);

        midi_signal = in_map_event.buffer[0];
        midi_note_cc = in_map_event.buffer[1];
        midi_value = in_map_event.buffer[2];



       // printf("Midi in- %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);



        jack_midi_clear_buffer(mapper_out);
        jack_midi_clear_buffer(guitarix_send);



        //Not related to modes
        if(midi_signal == CC && midi_note_cc == MASTER_FADER) // General master volume
        {

            const unsigned char data_out[3] = {midi_signal,GX_MASTER_VOLUME,midi_value}; //transparent send
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));

        }
        else if(midi_signal == CC && midi_note_cc == GAIN_POT) // Gain pot change
        {
            printf("GAIN TRIM - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            s_gain = midi_value;
            const unsigned char data_out[3] = {midi_signal,GX_GAIN,s_gain}; //transparent send
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == NOTE_ON && midi_note_cc == GAIN_RST) // Gain reset
        {
            printf("GAIN RST- %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            s_gain = midi_value;
            const unsigned char data_out[3] = {CC,GAIN_POT,DEFAULT_GAIN}; //update gain pot
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == CC && midi_note_cc == NOISE_GATE_POT) // Noise gate pot change
        {
            printf("Noise gate pot - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            s_noise_gate = midi_value;
            const unsigned char data_out[3] = {midi_signal,GX_NOISE_GATE,s_noise_gate}; //transparent send
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == NOTE_ON && midi_note_cc == NOISE_GATE_RST) // Noise gate rst
        {
            printf("Noise gate rst - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            s_noise_gate = midi_value;
            const unsigned char data_out[3] = {CC,NOISE_GATE_POT,69}; //update gain pot
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == NOTE_OFF && midi_note_cc == BT_NOISE_GATE) // Noise gate toggle
        {
            printf("Noise gate toggle - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            if(s_noise_gate_sw == 0)
            {
                s_noise_gate_sw = 127;
            }
            else
            {
                s_noise_gate_sw = 0;
            }
            const unsigned char data_out[3] = {CC,GX_NOISE_GATE_SW,s_noise_gate_sw}; //update gain pot
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == CC && midi_note_cc == COMP_POT) // Compressor pot change
        {
            printf("Comp pot - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            s_comp = midi_value;
            const unsigned char data_out[3] = {midi_signal,GX_COMP,s_comp}; //transparent send
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == NOTE_ON && midi_note_cc == COMP_RST) // Compressor rst
        {
            printf("Comp rst - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            s_comp = midi_value;
            const unsigned char data_out[3] = {CC,GX_COMP,69}; //update gain pot
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == NOTE_OFF && midi_note_cc == BT_COMP) // Compressor toggle
        {
            printf("Comp toggle - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            if(s_comp_sw == 0)
            {
                s_comp_sw = 127;
            }
            else
            {
                s_comp_sw = 0;
            }
            const unsigned char data_out[3] = {CC,GX_COMP_SW,s_comp_sw}; //update gain pot
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == CC && midi_note_cc == VOLUME_POT) // volume boost pot change
        {
            printf("Volume boost pot - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            s_vol = midi_value;
            const unsigned char data_out[3] = {midi_signal,GX_VOLBOOST,s_vol}; //transparent send
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == NOTE_ON && midi_note_cc == VOLUME_RST) // volume  rst
        {
            printf("Volume boost rst - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            s_vol = midi_value;
            const unsigned char data_out[3] = {CC,GX_VOLBOOST,69}; //update gain pot
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == NOTE_OFF && midi_note_cc == BT_VOLUME) // Volume boost toggle
        {
            printf("Volume toggle - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);
            if(s_vol_sw == 0)
            {
                s_vol_sw = 127;
            }
            else
            {
                s_vol_sw = 0;
            }
            const unsigned char data_out[3] = {CC,GX_VOLBOOST_SW,s_vol_sw}; //update gain pot
            int a = jack_midi_event_write(guitarix_send,i,data_out,sizeof(data_out));
        }

        jack_midi_clear_buffer(mapper_in);

    }


    for (int i = 0 ; i < gx_event_count; i++)
    {
        jack_midi_event_get(&in_gx_event, guitarix_return, i);


        midi_signal= in_gx_event.buffer[0];
        midi_note_cc= in_gx_event.buffer[1];
        midi_value= in_gx_event.buffer[2];


        jack_midi_clear_buffer(mapper_out);
        jack_midi_clear_buffer(guitarix_send);


       // printf("GX EVENT COUNT - %d\n",gx_event_count);


        //printf("GX INPUT Midi - %d - %d - %d\n",midi_signal,midi_note_cc,midi_value);

        //LEDS RINGS FOR MAIN CTRLS (1 TO 4)
        if(midi_signal == 176 && midi_note_cc == GX_GAIN) // Gain leds
        {
            const unsigned char data_out[3] = {176,RT_POT_1,13*midi_value/127}; //transparent send
            printf("SENDING TO Midi - %d - %d - %d\n",175,9,13*midi_value/127);
            int a = jack_midi_event_write(mapper_out,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == 176 && midi_note_cc == GX_NOISE_GATE) // Noise gate leds
        {
            const unsigned char data_out[3] = {176,RT_POT_2,13*midi_value/127}; //transparent send
            printf("SENDING TO Midi - %d - %d - %d\n",175,9,13*midi_value/127);
            int a = jack_midi_event_write(mapper_out,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == 176 && midi_note_cc == GX_COMP) // Comp leds
        {
            const unsigned char data_out[3] = {176,RT_POT_3,13*midi_value/127}; //transparent send
            printf("SENDING TO Midi - %d - %d - %d\n",175,9,13*midi_value/127);
            int a = jack_midi_event_write(mapper_out,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == 176 && midi_note_cc == GX_VOLBOOST) // volume booster leds
        {
            const unsigned char data_out[3] = {176,RT_POT_4,13*midi_value/127}; //transparent send
            printf("SENDING TO Midi - %d - %d - %d\n",175,9,13*midi_value/127);
            int a = jack_midi_event_write(mapper_out,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == 176 && midi_note_cc == GX_NOISE_GATE_SW) // NG Switch
        {
            s_noise_gate_sw = midi_value;
            const unsigned char data_out[3] = {0x90,RT_BT_2,s_noise_gate_sw/127}; //transparent send
            printf("SENDING TO Midi - %d - %d - %d\n",0x90,RT_BT_2,midi_value/127);
            int a = jack_midi_event_write(mapper_out,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == 176 && midi_note_cc == GX_COMP_SW) // Comp leds
        {
            s_comp_sw = midi_value;
            const unsigned char data_out[3] = {0x90,RT_BT_3,midi_value/127}; //transparent send
            printf("SENDING TO Midi - %d - %d - %d\n",0x90,RT_BT_3,midi_value/127);
            int a = jack_midi_event_write(mapper_out,i,data_out,sizeof(data_out));
        }
        else if(midi_signal == 176 && midi_note_cc == GX_VOLBOOST_SW) // Comp leds
        {

            s_vol_sw = midi_value;
            const unsigned char data_out[3] = {0x90,RT_BT_4,midi_value/127}; //transparent send
            printf("SENDING TO Midi - %d - %d - %d\n",0x90,RT_BT_4,midi_value/127);
            int a = jack_midi_event_write(mapper_out,i,data_out,sizeof(data_out));
        }

        jack_midi_clear_buffer(guitarix_return);

    }



    return 0;
}

int srate(jack_nframes_t nframes, void *arg)
{
    printf("the sample rate is now %" PRIu32 "/sec\n", nframes);

    return 0;
}

void jack_shutdown(void *arg)
{
    exit(1);
}

int main(int narg, char **args)
{


    if ((client = jack_client_open ("XTouch Mini Mapper", JackNullOption, NULL)) == 0)
    {
        fprintf(stderr, "jack server not running?\n");
        return 1;
    }



    jack_set_process_callback (client, process, 0);

    jack_set_sample_rate_callback (client, srate, 0);

    jack_on_shutdown (client, jack_shutdown, 0);

    mapper_in_port = jack_port_register (client, "Mapper In", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    mapper_output_port = jack_port_register (client, "Mapper Out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
    guitarix_send_port = jack_port_register (client, "GX Send", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
    guitarix_return_port = jack_port_register (client, "GX Return", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);



    if (jack_activate (client))
    {
        fprintf(stderr, "cannot activate client");
        return 1;
    }

    /* run until interrupted */
    while(1)
    {
        sleep(1);
    }
    jack_client_close(client);
    exit (0);
}
