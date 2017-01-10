#pragma once
#include <QGraphicsScene>

class PwArea;
class PwNode;
class PwConnector;
class PwEndpoint;
///

class PwAreaScenePrivate;
class PwAreaScene : public QGraphicsScene {
  Q_OBJECT;

  Q_DECLARE_PRIVATE(PwAreaScene);
  QScopedPointer<PwAreaScenePrivate> d_ptr;

 public:
  explicit PwAreaScene(QObject *parent = nullptr);
  ~PwAreaScene();

  const QList<PwNode *> &nodes() const;
  const QList<PwConnector *> &connectors() const;

  PwNode *getNode(const QString &id);
  PwConnector *getConnector(PwEndpoint *src, PwEndpoint *dst) const;

  PwEndpoint *endpointAt(QPointF pos) const;

  PwNode *createNode(const QString &id);
  void removeSelection();

  PwConnector *createConnector(PwEndpoint *src, PwEndpoint *dst);

  void autoLayout();
};
