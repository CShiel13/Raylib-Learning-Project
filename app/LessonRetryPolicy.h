#pragma once
// Pure lesson navigation policy: a mistake costs a heart, not a question.
enum class LessonNextStep { Retry, Advance, Complete, OutOfHearts };
inline LessonNextStep AfterLessonAnswer(bool right, int hearts, int current, int total) {
    if (hearts <= 0) return LessonNextStep::OutOfHearts;
    if (!right) return LessonNextStep::Retry;
    return current + 1 >= total ? LessonNextStep::Complete : LessonNextStep::Advance;
}
