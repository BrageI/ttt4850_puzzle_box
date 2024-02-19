#pragma once

struct Book {
    using Identifier = unsigned int;
    Book(Identifier, float thickness);
    
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
    // const unsigned int sensor_in;
    // const unsigned int sensor_out;
};
