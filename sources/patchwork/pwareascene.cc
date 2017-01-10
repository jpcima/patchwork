#include "pwareascene.h"
#include "pwarea.h"
#include "pwnode.h"
#include "pwconnector.h"
#include <QDebug>
#include <boost/cast.hpp>

class PwAreaScenePrivate {
  Q_DECLARE_PUBLIC(PwAreaScene);

  PwAreaScenePrivate(PwAreaScene *p) : q_ptr(p) {}
  PwAreaScene *q_ptr {};

  QList<PwNode *> nodes;
  QList<PwConnector *> connectors;

  void addNode(PwNode *node);
  void removeNode(PwNode *node);
  void addConnector(PwConnector *conn);
  void removeConnector(PwConnector *conn);
};

PwAreaScene::PwAreaScene(QObject *parent)
    : QGraphicsScene(parent), d_ptr(new PwAreaScenePrivate(this)) {
}

PwAreaScene::~PwAreaScene() {
}

const QList<PwNode *> &PwAreaScene::nodes() const {
  Q_D(const PwAreaScene);

  return d->nodes;
}

const QList<PwConnector *> &PwAreaScene::connectors() const {
  Q_D(const PwAreaScene);

  return d->connectors;
}

PwNode *PwAreaScene::getNode(const QString &id) {
  Q_D(const PwAreaScene);

  for (PwNode *node : d->nodes)
    if (node->id() == id)
      return node;
  return nullptr;
}

PwConnector *PwAreaScene::getConnector(PwEndpoint *src, PwEndpoint *dst) const {
  Q_D(const PwAreaScene);

  for (PwConnector *conn : d->connectors)
    if (conn->sourceEndpoint() == src && conn->targetEndpoint() == dst)
      return conn;
  return nullptr;
}

PwEndpoint *PwAreaScene::endpointAt(QPointF pos) const {
  for (QGraphicsItem *item : this->items(pos))
    if (PwEndpoint *ep = dynamic_cast<PwEndpoint *>(item))
      return ep;
  return nullptr;
}

PwNode *PwAreaScene::createNode(const QString &id) {
  Q_D(PwAreaScene);

  PwNode *node = new PwNode(id);
  d->addNode(node);
  return node;
}

void PwAreaScene::removeSelection() {
  Q_D(PwAreaScene);

  for (QGraphicsItem *item : this->selectedItems()) {
    if (PwConnector *conn = dynamic_cast<PwConnector *>(item))
      d->removeConnector(conn);
  }

  for (QGraphicsItem *item : this->selectedItems()) {
    if (PwNode *node = dynamic_cast<PwNode *>(item))
      d->removeNode(node);
  }
}

PwConnector *PwAreaScene::createConnector(PwEndpoint *src, PwEndpoint *dst) {
  Q_D(PwAreaScene);

  if (getConnector(src, dst))
    return nullptr;

  PwConnector *conn = new PwConnector(src, dst);
  d->addConnector(conn);
  return conn;
}

void PwAreaScenePrivate::addNode(PwNode *node) {
  Q_Q(PwAreaScene);

  Q_ASSERT_X(!q->getNode(node->id()), "PwAreaScene",
             "This identifier conflicts with an existing node");

  this->nodes.append(node);
  QObject::connect(node, &QObject::destroyed,
                   [this, node]() { this->nodes.removeOne(node); });

  q->addItem(node);
}

void PwAreaScenePrivate::removeNode(PwNode *node) {
  delete node;
}

void PwAreaScenePrivate::addConnector(PwConnector *conn) {
  Q_Q(PwAreaScene);

  Q_ASSERT_X(!q->getConnector(conn->sourceEndpoint(), conn->targetEndpoint()),
             "PwAreaScene",
             "This source and target conflict with an existing connector");

  this->connectors.append(conn);
  QObject::connect(conn, &QObject::destroyed,
                   [this, conn]() { this->connectors.removeOne(conn); });

  q->addItem(conn);
}

void PwAreaScenePrivate::removeConnector(PwConnector *conn) {
  delete conn;
}
