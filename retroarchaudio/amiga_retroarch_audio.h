#ifndef AMIGARETROARCHAUDIO_H_
#define AMIGARETROARCHAUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>


void *amiga_audio_init(const char *device,
	unsigned rate, unsigned latency,
	unsigned block_frames, unsigned *new_rate); // device and block_frames can be ignored, latency potentially too, new_rate should at the end contain the rate which was actually used, fixing things to 22050 also is fine

size_t amiga_audio_write(void *data, const void *s, size_t len); // Assume 16 Bit Samples
bool amiga_audio_stop(void *data); // Stops replay
bool amiga_audio_start(void *data, bool is_shutdown); // starts replay
void amiga_audio_free(void *data);

#ifdef __cplusplus
}
#endif

#endif
