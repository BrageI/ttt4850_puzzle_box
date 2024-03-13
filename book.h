#pragma once
#include <vector>
#include "ultrasound_reader.h"

struct Book {
    using Identifier = unsigned int;
    Book(
        Identifier,
        float thickness,
        unsigned int ultrasound_trig_pin,
        unsigned int ultrasound_echo_pin,
        float taken_threshold,
        float returned_threshold
    );
    
    const Identifier id;
    const float thickness; // [cm]

    enum class Status {
        UNKNOWN,
        INSERTED,
        TAKEN_OUT,
    } status;

    /* To begin with, each book has a designated slot.
     * Since slot and book belong together, slot related info is stored
     * here. If they are split in the future, slot info/slot-book mapping
     * should be split out from this class accordingly.*/

    HCSR04Driver ultrasound_driver;
    const float taken_threshold; // [cm]
    const float returned_threshold; // [cm]
    const unsigned int consecutive_reading_requirement{3};
    unsigned int consecutive_taken_reading{0};
    unsigned int consecutive_returned_reading{0};
};
