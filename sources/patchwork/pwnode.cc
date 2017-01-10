#include "pwnode.h"
#include "pwconnector.h"
#include "pwendpoint.h"
#include "pwareascene.h"
#include "qutil/qutil.h"
#include <QFrame>
#include <QVBoxLayout>
#include <QEvent>
#include <QTimer>
#include <QDebug>
#include <boost/cast.hpp>

class PwNodePrivate {
  Q_DECLARE_PUBLIC(PwNode);

  PwNodePrivate(PwNode *p) : q_ptr(p) {}
  PwNode *q_ptr {};
  QString id;
  QList<PwEndpoint *> endpoints;
  void arrangeEndpoints();
};

PwNode::PwNode(const QString &id, QGraphicsItem *parent)
    : PwBoxItem(parent), d_ptr(new PwNodePrivate(this)) {
  Q_D(PwNode);

  d->id = id;

  QWidget *wc = new QWidget;
  wc->setMinimumHeight(40);
  QVBoxLayout *l = qutil::vboxNoMargins();
  wc->setLayout(l);
  this->setCentralWidget(wc);

  QObject *evf = qutil::createFunctionalEventFilter([d](QEvent *ev) -> bool {
    if (ev->type() == QEvent::LayoutRequest)
      QTimer::singleShot(0, [d]() { d->arrangeEndpoints(); });
    return false;
  }, this);
  this->installContainerEventFilter(evf);

  this->setAcceptHoverEvents(true);
}

PwNode::~PwNode() {
  Q_D(PwNode);

  while (!d->endpoints.isEmpty())
    delete d->endpoints.takeFirst();
}

const QString &PwNode::id() const {
  Q_D(const PwNode);

  return d->id;
}

PwEndpoint *PwNode::createEndpoint(const QString &id,
                                   PwEndpoint::Attributes attr,
                                   PwEndpoint::Placement placement) {
  Q_D(PwNode);

#if !defined(QT_NO_DEBUG)
  for (PwEndpoint *ep : d->endpoints)
    Q_ASSERT_X(id != ep->id(), "PwNode",
               "This identifier conflicts with an existing endpoint");
#endif

  PwEndpoint *ep = new PwEndpoint(id, attr, placement, this);

  d->endpoints.append(ep);
  QObject::connect(ep, &PwEndpoint::destroyed, [d, ep]() {
    d->endpoints.removeOne(ep);
  });
  QObject::connect(ep, &PwEndpoint::placementChanged, [d]() {
    d->arrangeEndpoints();
  });

  d->arrangeEndpoints();
  return ep;
}

QList<PwConnector *> PwNode::incomingConnectors() const {
  PwAreaScene *scene = boost::polymorphic_downcast<PwAreaScene *>(this->scene());
  Q_ASSERT(scene);

  QList<PwConnector *> result;
  for (PwConnector *conn : scene->connectors())
    if (conn->targetEndpoint()->parentNode() == this)
      result.append(conn);
  return result;
}

QList<PwConnector *> PwNode::outgoingConnectors() const {
  PwAreaScene *scene = boost::polymorphic_downcast<PwAreaScene *>(this->scene());
  Q_ASSERT(scene);

  QList<PwConnector *> result;
  for (PwConnector *conn : scene->connectors())
    if (conn->sourceEndpoint()->parentNode() == this)
      result.append(conn);
  return result;
}

const QList<PwEndpoint *> PwNode::endpoints() const {
  Q_D(const PwNode);

  return d->endpoints;
}

PwEndpoint *PwNode::findEndpoint(const QString &id) const {
  Q_D(const PwNode);

  for (PwEndpoint *ep : d->endpoints)
    if (ep->id() == id)
      return ep;
  return nullptr;
}

PwEndpoint *PwNode::getEndpoint(const QString &id) const {
  PwEndpoint *ep = this->findEndpoint(id);
  Q_ASSERT_X(ep, "PwEndpoint", "Cannot find an endpoint with the requested id");
  return ep;
}

void PwNodePrivate::arrangeEndpoints() {
  Q_Q(PwNode);

  const unsigned hspacing = 20;
  const unsigned vspacing = 20;

  unsigned numt = 0, numb = 0, numl = 0, numr = 0;
  for (PwEndpoint *ep : this->endpoints) {
    switch (ep->placement()) {
      case PwEndpoint::TopPlacement: ++numt; break;
      case PwEndpoint::BottomPlacement: ++numb; break;
      case PwEndpoint::LeftPlacement: ++numl; break;
      case PwEndpoint::RightPlacement: ++numr; break;
      default: Q_ASSERT(false);
    }
  }

  unsigned numhoriz = qMax(numt, numb);
  unsigned numvert = qMax(numl, numr);

  QSize minsize(hspacing * (1 + numhoriz), vspacing * (1 + numvert));
  q->setEndpointBoundingSize(minsize);
  QTimer::singleShot(0, [q]() { q->updateContainerSize(); });

  numt = 0, numb = 0, numl = 0, numr = 0;
  for (PwEndpoint *ep : this->endpoints) {
    float xpos, ypos;
    const float offset = 2;
    switch (ep->placement()) {
      case PwEndpoint::TopPlacement:
        xpos = hspacing * (1 + numt);
        ypos = -offset;
        ++numt;
        break;
      case PwEndpoint::BottomPlacement:
        xpos = hspacing * (1 + numb);
        ypos = q->boundingRect().height() + offset - 1;
        ++numb;
        break;
      case PwEndpoint::LeftPlacement:
        xpos = -offset;
        ypos = vspacing * (1 + numl);
        ++numl;
        break;
      case PwEndpoint::RightPlacement:
        xpos = q->boundingRect().width() + offset - 1;
        ypos = vspacing * (1 + numr);
        ++numr;
        break;
      default: Q_ASSERT(false);
    }
    ep->setPos(xpos, ypos);
  }

  emit q->onArrangedEndpoints();
}
