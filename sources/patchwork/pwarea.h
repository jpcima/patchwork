#pragma once
#include <QGraphicsView>

class PwAreaScene;
class PwNode;
class PwConnector;
///

class PwAreaPrivate;
class PwArea : public QGraphicsView {
  Q_OBJECT;

  Q_DECLARE_PRIVATE(PwArea);
  QScopedPointer<PwAreaPrivate> d_ptr;

 public:
  explicit PwArea(QWidget *parent = nullptr);
  ~PwArea();
  PwAreaScene *scene() const;

 protected:
  void keyPressEvent(QKeyEvent *keyEvent) override;
  void mousePressEvent(QMouseEvent *mouseEvent) override;
  void mouseReleaseEvent(QMouseEvent *mouseEvent) override;
  void mouseMoveEvent(QMouseEvent *mouseEvent) override;
};
