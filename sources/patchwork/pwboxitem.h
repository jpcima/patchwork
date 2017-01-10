#pragma once
#include <QGraphicsRectItem>

class QFrame;
///

class PwBoxItemPrivate;
class PwBoxItem : public QObject, public QGraphicsRectItem {
  Q_OBJECT;

  Q_DECLARE_PRIVATE(PwBoxItem);
  QScopedPointer<PwBoxItemPrivate> d_ptr;

 public:
  PwBoxItem(QGraphicsItem *parent = nullptr);
  ~PwBoxItem();

  void setTitle(const QString &title);
  void setCentralWidget(QWidget *w);
  QWidget *centralWidget() const;

 signals:
  void itemPositionChanged(const QPointF &pos);

 protected:
  // intented for internal use only
  void installContainerEventFilter(QObject *filter);
  void setEndpointBoundingSize(QSize size);
  void updateContainerSize();

 protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};
