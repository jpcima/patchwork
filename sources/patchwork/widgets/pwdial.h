#pragma once
#include <QWidget>

class PwDialPrivate;
class PwDial : public QWidget {
  Q_OBJECT;

  Q_PROPERTY(double value READ value WRITE setValue);
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum);
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum);
  Q_PROPERTY(double defaultValue READ defaultValue WRITE setDefaultValue);
  Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep);
  Q_PROPERTY(double pageStep READ pageStep WRITE setPageStep);
  Q_PROPERTY(bool tracking READ tracking WRITE setTracking);
  Q_PROPERTY(QString labelText READ labelText WRITE setLabelText);

  Q_DECLARE_PRIVATE(PwDial);
  QScopedPointer<PwDialPrivate> d_ptr;

 public:
  explicit PwDial(QWidget *parent = nullptr);
  ~PwDial();

  double value() const;
  void setValue(double value);

  double minimum() const;
  double maximum() const;
  void setMinimum(double min);
  void setMaximum(double max);
  void setRange(double min, double max);

  double defaultValue() const;
  void setDefaultValue(double value);

  double singleStep() const;
  void setSingleStep(double step);

  double pageStep() const;
  void setPageStep(double step);

  bool tracking() const;
  void setTracking(bool tracking);

  void setLabelVisible(bool visible);

  const QString &labelText();
  void setLabelText(const QString &label);

 signals:
  void valueChanged(double value);
  void rangeChanged(double min, double max);

 protected:
  void mouseMoveEvent(QMouseEvent *e) override;
  void mousePressEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void wheelEvent(QWheelEvent *e) override;
  void keyPressEvent(QKeyEvent *e) override;
};
