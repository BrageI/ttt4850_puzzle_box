#include <stdio.h>
#include <alsa/asoundlib.h>

void playSound(const char* wavFilePath) {
    // Open PCM device for playback
    snd_pcm_t *pcm_handle;
    int err;
    if ((err = snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        return;
    }

    // Set PCM format
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_hw_params_alloca(&hw_params);
    if ((err = snd_pcm_hw_params_any(pcm_handle, hw_params)) < 0) {
        printf("Can not configure this PCM device: %s\n", snd_strerror(err));
        return;
    }
    if ((err = snd_pcm_hw_params_set_access(pcm_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        printf("Error setting access: %s\n", snd_strerror(err));
        return;
    }
    if ((err = snd_pcm_hw_params_set_format(pcm_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        printf("Error setting format: %s\n", snd_strerror(err));
        return;
    }

    // Set sample rate
    unsigned int sample_rate = 44100;
    if ((err = snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &sample_rate, 0)) < 0) {
        printf("Error setting sample rate: %s\n", snd_strerror(err));
        return;
    }

    // Apply HW parameters
    if ((err = snd_pcm_hw_params(pcm_handle, hw_params)) < 0) {
        printf("Error setting HW params: %s\n", snd_strerror(err));
        return;
    }

    // Prepare PCM device
    if ((err = snd_pcm_prepare(pcm_handle)) < 0) {
        printf("Error preparing PCM device: %s\n", snd_strerror(err));
        return;
    }

    // Open .wav file
    FILE *file = fopen(wavFilePath, "rb");
    if (!file) {
        printf("Error opening file.\n");
        return;
    }

    // Read and play .wav file
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    int read_count;
    while ((read_count = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        if ((err = snd_pcm_writei(pcm_handle, buffer, read_count)) != read_count) {
            printf("Error writing to PCM device: %s\n", snd_strerror(err));
        }
    }

    // Close PCM device and file
    snd_pcm_close(pcm_handle);
    fclose(file);
}
