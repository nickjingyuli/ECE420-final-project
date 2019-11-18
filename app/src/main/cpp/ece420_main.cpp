//
// Created by daran on 1/12/2017 to be used in ECE420 Sp17 for the first time.
// Modified by dwang49 on 1/1/2018 to adapt to Android 7.0 and Shield Tablet updates.
//

#include <cmath>
#include "ece420_main.h"

// Student Variables
#define FRAME_SIZE 128
#define SOUND_LENGTH (256000/2)

int8_t flag = 0;
int16_t counter = 0;
int16_t output_counter = 0;
int8_t output_flag;
float_t ref_signal[SOUND_LENGTH] = {};
float_t primary_signal[SOUND_LENGTH] = {};
//float_t error_signal[SOUND_LENGTH] = {};
volatile int8_t loop = 1;

void ece420ProcessFrame(sample_buf *dataBuf) {
    // Keep in mind, we only have a small amount of time to process each buffer!
    struct timeval start;
    gettimeofday(&start, NULL);

    // Using {} initializes all values in the array to zero
    int16_t bufferIn[FRAME_SIZE] = {};
    int16_t bufferOut[FRAME_SIZE] = {};



    // Your buffer conversion (unpacking) here
    // Fetch data sample from dataBuf->buf_[], unpack and put into bufferIn[]
    // ******************** START YOUR CODE HERE ******************** //
    for (int i = 0; i < FRAME_SIZE; i++) {
        uint16_t unpacked = 0;
        unpacked += dataBuf->buf_[2*i+1];
        unpacked = unpacked << 8;
        unpacked += dataBuf->buf_[2*i];

        bufferIn[i] = unpacked;
    }

    //fill either reference array or primary array

    if (!flag && counter < SOUND_LENGTH/FRAME_SIZE) {
        for (int i = 0; i < FRAME_SIZE; i++) {
            ref_signal[FRAME_SIZE*counter + i] = (float_t)bufferIn[i];
        }
        counter++;
    }

    else if (!flag) {
        flag++;
        counter = 0;
        LOGD("Reference signal filled");
        for (int i = 0; i < 20000000; i++) {
        }
    }

    else if (flag == 1 && counter < SOUND_LENGTH/FRAME_SIZE) {
        for (int i = 0; i < FRAME_SIZE; i++) {
            primary_signal[FRAME_SIZE*counter + i] = (float_t)bufferIn[i];
        }
        counter++;
    }

    else if (flag == 1) {
        flag++;
        counter = 0;
        LOGD("Primary signal filled");
        for (int i = 0; i < 20000000; i++) {
        }
    }


    // ********************* END YOUR CODE HERE ********************* //

    // Loop code provided as a suggestion. This loop simulates sample-by-sample processing.
    for (int sampleIdx = 0; sampleIdx < FRAME_SIZE; sampleIdx++) {
        int16_t output;
        // if both signals filled, write to output, otherwise do not play sound
        if (flag < 2) {
            output = 0;
            output_flag = 0;
        }
        else {
            output = (int16_t)primary_signal[output_counter*FRAME_SIZE + sampleIdx];
            output_flag = 1;
        }
        // Grab result and put into bufferOut[]
        bufferOut[sampleIdx] = output;
    }

    if (output_flag) {
        output_counter++;
        if (output_counter > SOUND_LENGTH/FRAME_SIZE) {
            counter = 0;
            flag = 0;
            LOGD("Processed signal outputted.");
            for (int i = 0; i < 20000000; i++) {
            }
        }
    }

    // Your buffer conversion (packing) here
    // Fetch data sample from bufferOut[], pack them and put back into dataBuf->buf_[]
    // ******************** START YOUR CODE HERE ******************** //
    for (int i = 0; i < FRAME_SIZE; i++) {
        uint8_t temp_high = (bufferOut[i] >> 8);
        uint8_t temp_low = (bufferOut[i] & 0x00FF);
        dataBuf->buf_[2*i] = temp_low;
        dataBuf->buf_[2*i+1] = temp_high;

    }
    // ********************* END YOUR CODE HERE ********************* //

	// Log the processing time to Android Monitor or Logcat window at the bottom
    struct timeval end;
    gettimeofday(&end, NULL);
//    LOGD("Loop timer: %ld us",  ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

}
