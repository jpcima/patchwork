#pragma once
#include <QGraphicsRectItem>

class PwNode;
class PwConnector;
///

class PwEndpointPrivate;
class PwEndpoint : public QObject, public QGraphicsRectItem {
  Q_OBJECT;

  Q_DECLARE_PRIVATE(PwEndpoint);
  QScopedPointer<PwEndpointPrivate> d_ptr;

 public:
  enum Attribute {
    Input = 0x1,
    Output = 0x2,
  };
  enum Placement {
    DefaultPlacementForType,
    TopPlacement,
    BottomPlacement,
    LeftPlacement,
    RightPlacement,
  };

  typedef QFlags<Attribute> Attributes;

  PwEndpoint(const QString &id, Attributes attr,
             Placement placement = DefaultPlacementForType,
             QGraphicsItem *parent = nullptr);
  ~PwEndpoint();

  const QString &id() const;
  Attributes attributes() const;

  Placement placement() const;
  void setPlacement(Placement placement);

  PwNode *parentNode() const;

  QList<PwConnector *> incomingConnectors() const;
  QList<PwConnector *> outgoingConnectors() const;

 signals:
  void placementChanged();

 protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
};
