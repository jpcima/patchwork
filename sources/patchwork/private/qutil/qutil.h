#pragma once
#include <QRectF>

class QHBoxLayout;
class QVBoxLayout;
class QWidget;
class QGraphicsItem;
class QString;
class QObject;
class QString;
class QPixmap;
///

namespace qutil {

QHBoxLayout *hboxNoMargins();
QVBoxLayout *vboxNoMargins();

void setStyleSheetFromFile(QWidget *w, const QString &file);

template <class F>
QObject *createFunctionalEventFilter(F f, QObject *p);

template <class F>
void installFunctionalEventFilter(QObject *obj, F f);

QRectF getOpaqueBoundingRect(const QPixmap &p);

}  // namespace qutil

#include "qutil.tcc"
