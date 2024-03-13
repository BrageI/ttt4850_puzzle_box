#include "book.h"

Book::Book(
    Identifier id,
    float thickness,
    unsigned int ultrasound_trig_pin,
    unsigned int ultrasound_echo_pin,
    float taken_threshold,
    float returned_threshold
):
    id{id},
    thickness{thickness},
    status{Status::UNKNOWN},
    ultrasound_driver{ultrasound_trig_pin, ultrasound_echo_pin},
    taken_threshold{taken_threshold},
    returned_threshold{returned_threshold}
{
    ultrasound_driver.init();
}
