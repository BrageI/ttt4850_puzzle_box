#include "book.h"

Book::Book(
    Identifier id,
    float thickness,
    unsigned int ultrasound_trig_pin,
    unsigned int ultrasound_echo_pin
):
    id{id},
    thickness{thickness},
    status{Status::UNKNOWN},
    ultrasound_driver{ultrasound_trig_pin, ultrasound_trig_pin} {}
