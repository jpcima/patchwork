#include "pwendpoint.h"
#include "pwnode.h"
#include "pwconnector.h"
#include "pwareascene.h"
#include <QtGlobal>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <boost/cast.hpp>

class PwEndpointPrivate {
  Q_DECLARE_PUBLIC(PwEndpoint);

  PwEndpointPrivate(PwEndpoint *p) : q_ptr(p) {}
  PwEndpoint *q_ptr {};
  QString id;
  PwEndpoint::Attributes attr {};
  PwEndpoint::Placement placement {};
  QGraphicsRectItem *hoverIndicator {};
};

PwEndpoint::PwEndpoint(const QString &id, Attributes attr,
                       Placement placement,
                       QGraphicsItem *parent)
    : QGraphicsRectItem(parent), d_ptr(new PwEndpointPrivate(this)) {
  Q_D(PwEndpoint);

  d->id = id;

  Q_ASSERT_X(bool(attr & Input) ^ bool(attr & Output),
             "PwEndpoint", "The endpoint must be either Input or Output");
  d->attr = attr;

  d->placement = placement;

  const int width = 10;
  const int height = 8;

  this->setRect(-width / 2, -height / 2, width, height);
  this->setPen(QPen(Qt::black));
  this->setBrush(QBrush(QColor(0xffff99)));

  d->hoverIndicator = new QGraphicsRectItem(this);
  d->hoverIndicator->setRect(this->rect());
  d->hoverIndicator->setPen(QPen(Qt::blue, 2));
  d->hoverIndicator->setVisible(false);

  this->setAcceptHoverEvents(true);
}

PwEndpoint::~PwEndpoint() {
  for (PwConnector *conn : this->incomingConnectors())
    delete conn;
  for (PwConnector *conn : this->outgoingConnectors())
    delete conn;
}

const QString &PwEndpoint::id() const {
  Q_D(const PwEndpoint);

  return d->id;
}

auto PwEndpoint::attributes() const -> Attributes {
  Q_D(const PwEndpoint);

  return d->attr;
}

auto PwEndpoint::placement() const -> Placement {
  Q_D(const PwEndpoint);

  switch (d->placement) {
    case TopPlacement: case BottomPlacement:
    case LeftPlacement: case RightPlacement:
      return d->placement;
    case DefaultPlacementForType:
      if (d->attr & Input)
        return TopPlacement;
      else
        return BottomPlacement;
    default:
      Q_ASSERT(false);
  }
}

void PwEndpoint::setPlacement(Placement placement) {
  Q_D(PwEndpoint);

  Placement old = this->placement();
  d->placement = placement;

  if (this->placement() != old)
    emit placementChanged();
}

PwNode *PwEndpoint::parentNode() const {
  QGraphicsItem *parentItem = this->parentItem();
  while (parentItem) {
    if (PwNode *node = dynamic_cast<PwNode *>(parentItem))
      return node;
    parentItem = parentItem->parentItem();
  }
  return nullptr;
}

QList<PwConnector *> PwEndpoint::incomingConnectors() const {
  PwAreaScene *scene = boost::polymorphic_downcast<PwAreaScene *>(this->scene());
  Q_ASSERT(scene);

  QList<PwConnector *> result;
  for (PwConnector *conn : scene->connectors())
    if (conn->targetEndpoint() == this)
      result.append(conn);
  return result;
}

QList<PwConnector *> PwEndpoint::outgoingConnectors() const {
  PwAreaScene *scene = boost::polymorphic_downcast<PwAreaScene *>(this->scene());
  Q_ASSERT(scene);

  QList<PwConnector *> result;
  for (PwConnector *conn : scene->connectors())
    if (conn->sourceEndpoint() == this)
      result.append(conn);
  return result;
}

void PwEndpoint::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  Q_D(PwEndpoint);

  d->hoverIndicator->setVisible(true);
}

void PwEndpoint::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  Q_D(PwEndpoint);

  d->hoverIndicator->setVisible(false);
}
