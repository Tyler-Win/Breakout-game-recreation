// -------------------------------------------------
// Copyright (c) 2023 HiBit <https://www.hibit.dev>
// -------------------------------------------------

#include "pitches.h"
#include "LaunchPad.h"
#include "clock.h"
#include "booster.h"


int small_world_melody[] = {
  NOTE_E4, NOTE_F4, NOTE_G4, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_D4,
  NOTE_E4, NOTE_F4, NOTE_D5, NOTE_B4, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_G4, NOTE_E4, NOTE_F4, 
  NOTE_G4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_A4, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_E5, 
  NOTE_D5, NOTE_G4, NOTE_F5, NOTE_E5, NOTE_D5, NOTE_C5, NOTE_C5, REST, NOTE_C5, REST, NOTE_C5, NOTE_E5, 
  NOTE_C5, NOTE_D5, REST, NOTE_D5, NOTE_D5, NOTE_D5, REST, NOTE_D5, NOTE_F5, NOTE_D5, NOTE_E5, REST, NOTE_E5, 
  NOTE_E5, NOTE_E5, REST, NOTE_E5, NOTE_G5, NOTE_E5, NOTE_F5, REST, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_D5, NOTE_G4, 
  NOTE_B4, NOTE_C5, NOTE_C5, REST
};

int small_world_durations[] = {
  8, 8, 4, 4, 4, 8, 8, 4, 4, 4, 8, 8, 4, 4, 4, 8, 8, 4, 4, 4, 8, 8, 4, 8, 8, 4, 8, 8, 4, 
  8, 8, 4, 8, 8, 4, 4, 4, 4, 2, 4, 4, 4, 8, 8, 4, 4, 4, 8, 8, 2, 4, 8, 8, 4, 4, 4, 8, 8, 
  2, 4, 8, 8, 4, 4, 4, 8, 8, 4, 8, 8, 2, 2, 2, 4, 4
};



void smallworld()
{
  int size = sizeof(small_world_durations) / sizeof(int);
  int max = 0;
  for (int note = 0; note < size; note++) {
    if (small_world_melody[note] > max) {
      max = small_world_melody[note];
    }
  }

  for (int note = 0; note < size; note++) {
    //to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int small_world_duration = 1000 / small_world_durations[note];
    int duration = 1000 / small_world_durations[note];
    int equalized_melody = small_world_melody[note] * NOTE_DS8 / max;
    int red = ((equalized_melody >> 8) & 0xFF) * 100 / 0xFF;
    int green = ((equalized_melody >> 4) & 0xFF) * 100 / 0xFF;
    int blue = ((equalized_melody >> 0) & 0xFF) * 100 / 0xFF;

   
    buzzer_tone(small_world_melody[note]);
    rgb_dc(red, green, blue);

    //to distinguish the notes, set a minimum time between them.
    //the note's duration + 30% seems to work well:
    int pauseBetweenNotes = small_world_duration * 1.30;
    msec_delay(pauseBetweenNotes);

  }
}