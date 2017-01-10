#include "pwconnector.h"
#include "pwendpoint.h"
#include "pwnode.h"
#include <QPen>
#include <QEvent>
#include <QTimer>
#include <QDebug>

class PwConnectorPrivate {
  Q_DECLARE_PUBLIC(PwConnector);

  PwConnectorPrivate(PwConnector *p) : q_ptr(p) {}
  PwConnector *q_ptr {};
  PwEndpoint *src {};
  PwEndpoint *dst {};
  void updatePath();
  void updatePathLater();
};

PwConnector::PwConnector(PwEndpoint *src, PwEndpoint *dst,
                         QGraphicsItem *parent)
    : QGraphicsPathItem(), d_ptr(new PwConnectorPrivate(this)) {
  Q_D(PwConnector);

  Q_ASSERT_X((src->attributes() & PwEndpoint::Output) &&
             (dst->attributes() & PwEndpoint::Input),
             "PwConnector", "Mismatched endpoint types for connector");

  d->src = src;
  d->dst = dst;

  this->setPen(QPen(Qt::blue, 2));

  d->updatePathLater();

  QObject::connect(src->parentNode(), &PwNode::onArrangedEndpoints,
                   this, [d]() { d->updatePathLater(); });
  QObject::connect(dst->parentNode(), &PwNode::onArrangedEndpoints,
                   this, [d]() { d->updatePathLater(); });
  QObject::connect(src->parentNode(), &PwNode::itemPositionChanged,
                   this, [d]() { d->updatePathLater(); });
  QObject::connect(dst->parentNode(), &PwNode::itemPositionChanged,
                   this, [d]() { d->updatePathLater(); });

  this->setZValue(-1);

  this->setFlag(QGraphicsItem::ItemIsSelectable);
}

PwConnector::~PwConnector() {
}

PwEndpoint *PwConnector::sourceEndpoint() const {
  Q_D(const PwConnector);

  return d->src;
}

PwEndpoint *PwConnector::targetEndpoint() const {
  Q_D(const PwConnector);

  return d->dst;
}

void PwConnectorPrivate::updatePath() {
  Q_Q(PwConnector);

  QPointF src = this->src->scenePos();
  QPointF dst = this->dst->scenePos();

  QRectF r(src, dst);
  QPointF c = r.center();

  QPainterPath path(src);

  QPointF c1 = src;
  c1.setY(c.y());
  path.quadTo(c1, c);

  QPointF c2 = c1;
  c2.setX(dst.x());
  path.quadTo(c2, dst);

  q->setPath(path);
}

void PwConnectorPrivate::updatePathLater() {
  QTimer::singleShot(0, [this]() { this->updatePath(); });
}
