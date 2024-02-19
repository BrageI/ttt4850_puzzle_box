#include "book.h"

Book::Book(Identifier id, float thickness):
    id{id},
    thickness{thickness},
    status{Status::UNKNOWN} {}
