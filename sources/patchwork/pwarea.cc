#include "pwarea.h"
#include "pwareascene.h"
#include "pwendpoint.h"
#include <QGraphicsProxyWidget>
#include <QEvent>
#include <QDebug>
#include <boost/cast.hpp>

enum PwAreaInputState {
  NormalState,
  ConnectingState,
};

struct PwAreaConnectingInfo;

class PwAreaPrivate {
  Q_DECLARE_PUBLIC(PwArea);

  explicit PwAreaPrivate(PwArea *p) : q_ptr(p) {}
  PwArea *q_ptr {};

  ///
  PwAreaInputState inputstate = NormalState;

  ///
  bool mayHandleKeyEvents();

  ///
  bool beginConnecting(QPointF pos);
  void continueConnecting(QPointF pos);
  void endConnecting(QPointF pos);
  QScopedPointer<PwAreaConnectingInfo> connecting;
};

PwArea::PwArea(QWidget *parent)
    : QGraphicsView(parent), d_ptr(new PwAreaPrivate(this)) {
  PwAreaScene *scene = new PwAreaScene(this);
  this->setScene(scene);
  this->setBackgroundBrush(QBrush(Qt::black, Qt::Dense7Pattern));
  this->setRenderHints(QPainter::Antialiasing);
  this->setDragMode(QGraphicsView::RubberBandDrag);
}

PwArea::~PwArea() {
}

PwAreaScene *PwArea::scene() const {
  return boost::polymorphic_downcast<PwAreaScene *>(QGraphicsView::scene());
}

void PwArea::keyPressEvent(QKeyEvent *keyEvent) {
  Q_D(PwArea);

  PwAreaScene *scene = this->scene();

  if (d->mayHandleKeyEvents()) {
    if (keyEvent->key() == Qt::Key_Delete) {
      scene->removeSelection();
      return;
    }
  }
  QGraphicsView::keyPressEvent(keyEvent);
}

void PwArea::mousePressEvent(QMouseEvent *mouseEvent) {
  Q_D(PwArea);

  if (d->inputstate == NormalState &&
      mouseEvent->button() == Qt::LeftButton) {
    if (d->beginConnecting(this->mapToScene(mouseEvent->pos())))
      return;
  } else if (d->inputstate == ConnectingState &&
             mouseEvent->button() == Qt::LeftButton) {
    d->endConnecting(this->mapToScene(mouseEvent->pos()));
    return;
  }

  QGraphicsView::mousePressEvent(mouseEvent);
}

void PwArea::mouseReleaseEvent(QMouseEvent *mouseEvent) {
  QGraphicsView::mouseReleaseEvent(mouseEvent);
}

void PwArea::mouseMoveEvent(QMouseEvent *mouseEvent) {
  Q_D(PwArea);

  if (d->inputstate == ConnectingState) {
    d->continueConnecting(this->mapToScene(mouseEvent->pos()));
    // return;
  }

  QGraphicsView::mouseMoveEvent(mouseEvent);
}

bool PwAreaPrivate::mayHandleKeyEvents() {
  Q_Q(PwArea);
  PwAreaScene *scene = q->scene();

  if (this->inputstate != NormalState)
    return false;

  QGraphicsItem *focus = scene->focusItem();
  if (QGraphicsProxyWidget *focusProxy =
      dynamic_cast<QGraphicsProxyWidget *>(focus)) {
    if (QWidget *w = focusProxy->widget())
      if (w->focusWidget())
        return false;
  }

  return true;
}

struct PwAreaConnectingInfo {
  PwEndpoint *epBegin {};
  QPointF posBegin {};
  QGraphicsRectItem *itmBegin {};
  QGraphicsRectItem *itmEnd {};
  QGraphicsLineItem *itmLine {};
};

bool PwAreaPrivate::beginConnecting(QPointF pos) {
  // qDebug() << "beginConnecting" << pos;

  Q_Q(PwArea);
  PwAreaScene *scene = q->scene();

  Q_ASSERT(this->inputstate == NormalState);

  PwEndpoint *ep = scene->endpointAt(pos);
  if (!ep)
    return false;

  PwAreaConnectingInfo *conn = new PwAreaConnectingInfo;
  this->connecting.reset(conn);

  conn->epBegin = ep;
  conn->posBegin = pos;
  conn->itmBegin = new QGraphicsRectItem;
  conn->itmBegin->setRect(-5, -5, 10, 10);
  conn->itmBegin->setPos(pos);
  conn->itmEnd = new QGraphicsRectItem;
  conn->itmEnd->setRect(-5, -5, 10, 10);
  conn->itmEnd->setPos(pos);
  conn->itmLine = new QGraphicsLineItem;
  conn->itmLine->setLine(QLineF(pos, pos));

  scene->addItem(conn->itmBegin);
  scene->addItem(conn->itmEnd);
  scene->addItem(conn->itmLine);

  this->inputstate = ConnectingState;
  return true;
}

void PwAreaPrivate::continueConnecting(QPointF pos) {
  // qDebug() << "continueConnecting" << pos;

  Q_ASSERT(this->inputstate == ConnectingState);

  PwAreaConnectingInfo *conn = this->connecting.data();

  conn->itmBegin->setPos(pos);
  conn->itmEnd->setPos(pos);
  conn->itmLine->setLine(QLineF(conn->posBegin, pos));
}

void PwAreaPrivate::endConnecting(QPointF pos) {
  // qDebug() << "endConnecting" << pos;

  Q_Q(PwArea);
  PwAreaScene *scene = q->scene();

  Q_ASSERT(this->inputstate == ConnectingState);

  PwAreaConnectingInfo *conn = this->connecting.data();

  QGraphicsItem *items[] = { conn->itmBegin, conn->itmEnd, conn->itmLine };
  for (QGraphicsItem *itm : items)
    delete itm;

  PwEndpoint *src = conn->epBegin;
  PwEndpoint *dst = scene->endpointAt(pos);

  if (dst) {
    if (src->attributes() & PwEndpoint::Input)
      std::swap(src, dst);
    if ((src->attributes() & PwEndpoint::Output) &&
        (dst->attributes() & PwEndpoint::Input))
      scene->createConnector(src, dst);
  }

  this->connecting.reset();
  this->inputstate = NormalState;
}
