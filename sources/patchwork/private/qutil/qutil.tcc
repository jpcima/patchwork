#pragma once
#include "qutil.h"
#include <QObject>
#include <utility>

namespace qutil {

namespace detail {

template <class F>
class FunctionalEventFilter : public QObject {
  F f;
 public:
  FunctionalEventFilter(F f, QObject *p) : QObject(p), f(std::move(f)) {}
  bool eventFilter(QObject *, QEvent *e) override { return f(e); }
};

}  // namespace detail

template <class F>
QObject *createFunctionalEventFilter(F f, QObject *p) {
  return new detail::FunctionalEventFilter<F>(std::move(f), p);
}

template <class F>
void installFunctionalEventFilter(QObject *obj, F f) {
  obj->installEventFilter(createFunctionalEventFilter(f, obj));
}

}  // namespace qutil
