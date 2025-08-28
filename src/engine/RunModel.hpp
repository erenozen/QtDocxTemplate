#pragma once
#include <QString>
#include <vector>

namespace QtDocxTemplate { namespace engine {

// TODO Phase C: Model runs and support pattern matching across split runs
class RunModel {
public:
    struct Segment { QString text; /* TODO reference to underlying XML node */ };
    void build(/* TODO pass XML root */) { /* placeholder */ }
    QString concatenated() const { return {}; }
};

}} // namespace QtDocxTemplate::engine
