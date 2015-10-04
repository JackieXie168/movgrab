#ifndef LIBUSEFUL_SOUND_H
#define LIBUSEFUL_SOUND_H

typedef struct
{
unsigned int Format;
unsigned int Channels;
unsigned int SampleRate;
unsigned int SampleSize;
unsigned int DataSize;
}TAudioInfo;

#define VOLUME_LEAVEALONE -1
#define PLAYSOUND_NONBLOCK 1

int PlaySoundFile(char *Path, int Vol, int Flags);
int SoundAlterVolume(char *Device, char *Channel, int delta);

#endif
