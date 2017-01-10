#include "qutil.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QGraphicsPixmapItem>
#include <QFile>

namespace qutil {

QHBoxLayout *hboxNoMargins() {
  QHBoxLayout *l = new QHBoxLayout;
  l->setContentsMargins(QMargins());
  return l;
}

QVBoxLayout *vboxNoMargins() {
  QVBoxLayout *l = new QVBoxLayout;
  l->setContentsMargins(QMargins());
  return l;
}

void setStyleSheetFromFile(QWidget *w, const QString &file) {
  QFile input(file);
  input.open(QFile::ReadOnly);
  QString filedata = QString::fromUtf8(input.readAll());
  w->setStyleSheet(filedata);
}

QRectF getOpaqueBoundingRect(const QPixmap &p) {
  QGraphicsPixmapItem item(p);
  return item.opaqueArea().boundingRect();
}

}  // namespace qutil
