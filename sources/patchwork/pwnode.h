#pragma once
#include "pwboxitem.h"
#include "pwendpoint.h"

class PwConnector;
///

class PwNodePrivate;
class PwNode : public PwBoxItem {
  Q_OBJECT;

  Q_DECLARE_PRIVATE(PwNode);
  QScopedPointer<PwNodePrivate> d_ptr;

 public:
  PwNode(const QString &id, QGraphicsItem *parent = nullptr);
  virtual ~PwNode();

  const QString &id() const;

  PwEndpoint *createEndpoint(
      const QString &id, PwEndpoint::Attributes attr,
      PwEndpoint::Placement placement = PwEndpoint::DefaultPlacementForType);

  QList<PwConnector *> incomingConnectors() const;
  QList<PwConnector *> outgoingConnectors() const;

  const QList<PwEndpoint *> endpoints() const;
  PwEndpoint *findEndpoint(const QString &id) const;
  PwEndpoint *getEndpoint(const QString &id) const;

 signals:
  void onArrangedEndpoints();
};
