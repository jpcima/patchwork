#pragma once
#include <QFrame>

class PwBoxTitlePrivate;
class PwBoxTitle : public QFrame {
  Q_OBJECT;

  Q_DECLARE_PRIVATE(PwBoxTitle);
  QScopedPointer<PwBoxTitlePrivate> d_ptr;

 public:
  explicit PwBoxTitle(QWidget *parent = nullptr);
  ~PwBoxTitle();

  QString title();
  void setTitle(const QString &title);
};
