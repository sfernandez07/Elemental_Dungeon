#include "score.h"

unsigned int  score;

void score_init(void) {
    score = 0;
}

void score_add(unsigned int pts) {
    score += pts;
}
