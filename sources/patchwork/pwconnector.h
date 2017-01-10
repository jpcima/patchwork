#pragma once
#include <QGraphicsPathItem>

class PwEndpoint;
///

class PwConnectorPrivate;
class PwConnector : public QObject, public QGraphicsPathItem {
  Q_OBJECT;

  Q_DECLARE_PRIVATE(PwConnector);
  QScopedPointer<PwConnectorPrivate> d_ptr;

 public:
  PwConnector(PwEndpoint *src, PwEndpoint *dst,
              QGraphicsItem *parent = nullptr);
  ~PwConnector();

  PwEndpoint *sourceEndpoint() const;
  PwEndpoint *targetEndpoint() const;
};
