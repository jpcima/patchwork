#include "pwdial.h"
#include "qutil/qutil.h"
#include <boost/optional.hpp>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QToolTip>
#include <QTimer>
#include <QDebug>
#include <cmath>

class PwDialPrivate {
  Q_DECLARE_PUBLIC(PwDial);

  explicit PwDialPrivate(PwDial *p) : q_ptr(p) {}
  PwDial *q_ptr {};

  QLabel *dial {};
  QLabel *label {};
  QString labelText;
  double value = 0.0;
  double minimum = 0.0;
  double maximum = 1.0;
  double defaultValue = 0.0;
  double singleStep = 0.02;
  double pageStep = 0.2;
  bool tracking = true;
  QVector<QPixmap> skin;
  boost::optional<QPoint> grabPoint;

  void loadSkin(const QString &path);
  void updateValueDisplay(double value);
  void showValueTip(double value);
  double valueForMousePos(QPoint pos) const;
  double inRange(double value) const;
};

PwDial::PwDial(QWidget *parent)
    : QWidget(parent), d_ptr(new PwDialPrivate(this)) {
  Q_D(PwDial);

  Q_INIT_RESOURCE(patchwork);

  d->dial = new QLabel;

  d->label = new QLabel;
  d->label->setAlignment(Qt::AlignCenter);
  d->label->setFrameShape(QFrame::StyledPanel);
  d->label->setFrameShadow(QFrame::Sunken);

  QVBoxLayout *l = qutil::vboxNoMargins();
  QHBoxLayout *h = qutil::hboxNoMargins();
  h->addStretch();
  h->addWidget(d->dial);
  h->addStretch();
  l->addLayout(h);
  l->addWidget(d->label);
  this->setLayout(l);

  d->loadSkin(":/patchwork/images/pwdial-skin.png");
  d->updateValueDisplay(d->value);
}

PwDial::~PwDial() {
}

double PwDial::value() const {
  Q_D(const PwDial);

  return d->value;
}

void PwDial::setValue(double value) {
  Q_D(PwDial);

  value = d->inRange(value);
  if (d->value == value)
    return;
  d->value = value;
  d->updateValueDisplay(value);
  emit valueChanged(value);
}

double PwDial::minimum() const {
  Q_D(const PwDial);

  return d->minimum;
}

double PwDial::maximum() const {
  Q_D(const PwDial);

  return d->maximum;
}

void PwDial::setMinimum(double min) {
  Q_D(PwDial);

  if (d->minimum == min)
    return;

  d->minimum = min;
  if (d->minimum > d->maximum)
    d->maximum = d->minimum;
  double nv = d->inRange(d->value);
  bool valueChange = nv != d->value;
  d->value = nv;
  d->updateValueDisplay(nv);
  emit rangeChanged(min, d->maximum);
  if (valueChange)
    emit valueChanged(nv);
}

void PwDial::setMaximum(double max) {
  Q_D(PwDial);

  if (d->maximum == max)
    return;

  d->maximum = max;
  if (d->minimum > d->maximum)
    d->minimum = d->maximum;
  double nv = d->inRange(d->value);
  bool valueChange = nv != d->value;
  d->value = nv;
  d->updateValueDisplay(nv);
  emit rangeChanged(d->minimum, max);
  if (valueChange)
    emit valueChanged(nv);
}

void PwDial::setRange(double min, double max) {
  Q_D(PwDial);

  if (max < min)
    max = min;
  if (d->maximum == max && d->minimum == min)
    return;

  d->minimum = min;
  d->maximum = max;
  double nv = d->inRange(d->value);
  bool valueChange = nv != d->value;
  d->value = nv;
  d->updateValueDisplay(nv);
  emit rangeChanged(d->minimum, max);
  if (valueChange)
    emit valueChanged(nv);
}

double PwDial::defaultValue() const {
  Q_D(const PwDial);

  return d->defaultValue;
}

void PwDial::setDefaultValue(double value) {
  Q_D(PwDial);

  d->defaultValue = value;
}

double PwDial::singleStep() const {
  Q_D(const PwDial);

  return d->singleStep;
}

void PwDial::setSingleStep(double step) {
  Q_D(PwDial);

  if (step <= 0)
    return;

  d->singleStep = step;
}

double PwDial::pageStep() const {
  Q_D(const PwDial);

  return d->pageStep;
}

void PwDial::setPageStep(double step) {
  Q_D(PwDial);

  if (step <= 0)
    return;

  d->pageStep = step;
}

bool PwDial::tracking() const {
  Q_D(const PwDial);

  return d->tracking;
}

void PwDial::setTracking(bool tracking) {
  Q_D(PwDial);

  d->tracking = tracking;
}

void PwDial::setLabelVisible(bool visible) {
  Q_D(PwDial);

  d->label->setVisible(visible);
}

const QString &PwDial::labelText() {
  Q_D(PwDial);

  return d->labelText;
}

void PwDial::setLabelText(const QString &label) {
  Q_D(PwDial);

  d->labelText = label;
  d->label->setText(label);
}

void PwDial::mouseMoveEvent(QMouseEvent *e) {
  Q_D(PwDial);

  if (d->grabPoint) {
    double nv = d->valueForMousePos(e->pos());
    if (d->tracking) {
      this->setValue(nv);
    } else {
      d->updateValueDisplay(d->inRange(nv));
    }
    d->showValueTip(d->value);
    return;
  }
  QWidget::mouseMoveEvent(e);
}

void PwDial::mousePressEvent(QMouseEvent *e) {
  Q_D(PwDial);

  this->setFocus();

  if (e->button() == Qt::MiddleButton) {
    this->setValue(d->defaultValue);
    return;
  }

  if (e->button() == Qt::LeftButton) {
    d->grabPoint = e->pos();
    d->showValueTip(d->value);
    return;
  }
  QWidget::mousePressEvent(e);
}

void PwDial::mouseReleaseEvent(QMouseEvent *e) {
  Q_D(PwDial);

  if (d->grabPoint) {
    double nv = d->valueForMousePos(e->pos());
    this->setValue(nv);
    d->showValueTip(d->value);
    d->grabPoint.reset();
    return;
  }
  QWidget::mouseReleaseEvent(e);
}

void PwDial::wheelEvent(QWheelEvent *e) {
  Q_D(PwDial);

  double delta = 0.5 * e->angleDelta().y() / 8;
  double nv = d->value + delta * d->singleStep;
  this->setValue(nv);
  d->showValueTip(d->value);
}

void PwDial::keyPressEvent(QKeyEvent *e) {
  Q_D(PwDial);

  switch (e->key()) {
    case Qt::Key_Up:
    case Qt::Key_Right:
      this->setValue(d->value + d->singleStep);
      d->showValueTip(d->value);
      return;
    case Qt::Key_Down:
    case Qt::Key_Left:
      this->setValue(d->value - d->singleStep);
      d->showValueTip(d->value);
      return;
    case Qt::Key_PageUp:
      this->setValue(d->value + d->pageStep);
      d->showValueTip(d->value);
      return;
    case Qt::Key_PageDown:
      this->setValue(d->value - d->pageStep);
      d->showValueTip(d->value);
      return;
    default:
      break;
  }
  QWidget::keyPressEvent(e);
}

void PwDialPrivate::loadSkin(const QString &path) {
  QVector<QPixmap> skin;
  QPixmap all;
  if (!all.load(path)) {
    qWarning() << "PwDial could not load skin file" << path;
    return;
  }
  unsigned dim = all.width();
  unsigned frameCount = all.height() / dim;
  skin.resize(frameCount);
  for (unsigned i = 0; i < frameCount; ++i)
    skin[i] = all.copy(0, i * dim, dim, dim);

  // minimize size by cropping
  QRectF boundsf;
  if (frameCount > 0) {
    boundsf = qutil::getOpaqueBoundingRect(skin[0]);
    for (unsigned i = 1; i < frameCount; ++i)
      boundsf = boundsf.intersected(qutil::getOpaqueBoundingRect(skin[i]));
  }
  QRect bounds(boundsf.x(), boundsf.y(),
               std::ceil(boundsf.width()), std::ceil(boundsf.height()));
  for (unsigned i = 0; i < frameCount; ++i)
    skin[i] = skin[i].copy(bounds);

  this->skin = std::move(skin);
}

void PwDialPrivate::updateValueDisplay(double value) {
  if (this->skin.empty()) {
    this->dial->setPixmap(QPixmap());
    return;
  }
  double num = value - this->minimum;
  double den = this->maximum - this->minimum;
  double r = (den == 0) ? 0.0 : (num / den);
  unsigned long n = this->skin.size();
  unsigned long i = std::lround(r * n);
  i = (long(i) < 0) ? 0 : (i >= n) ? (n - 1) : i;
  this->dial->setPixmap(this->skin[i]);
}

void PwDialPrivate::showValueTip(double value) {
  Q_Q(PwDial);

  QTimer::singleShot(0, q, [this, value]() {
      int yoff = QFontMetrics(QToolTip::font()).height();
      QPoint toolTipPos = this->dial->mapToGlobal(QPoint(0, 0));
      toolTipPos.ry() -= this->dial->height() / 2 + yoff;
      QString text = QString::number(value, 'f', 3);
      QToolTip::showText(toolTipPos, text, this->dial, QRect(), 500);
    });
}

double PwDialPrivate::valueForMousePos(QPoint pos) const {
  Q_Q(const PwDial);

  pos = this->dial->mapFromGlobal(q->mapToGlobal(pos));
  QRect rect = this->dial->rect();

  QPoint center = rect.center();
  QPoint diff = pos - center;
  double angle = std::atan2(diff.y(), diff.x()) + M_PI * 3 / 2;
  angle = std::fmod(angle, 2 * M_PI);

  const double dead_angle = M_PI / 4;
  angle -= dead_angle / 2;
  angle = (angle < 0) ? 0 : (angle > 2 * M_PI) ? (2 * M_PI) : angle;

  double r = angle / (2 * M_PI - dead_angle);
  return this->inRange(this->minimum + (this->maximum - this->minimum) * r);
}

double PwDialPrivate::inRange(double value) const {
  return (value < this->minimum) ? this->minimum :
      (value > this->maximum) ? this->maximum : value;
}
