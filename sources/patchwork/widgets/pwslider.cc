#include "pwslider.h"
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

class PwSliderPrivate {
  Q_DECLARE_PUBLIC(PwSlider);

  explicit PwSliderPrivate(PwSlider *p) : q_ptr(p) {}
  PwSlider *q_ptr {};

  QLabel *slider {};
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

PwSlider::PwSlider(QWidget *parent)
    : QWidget(parent), d_ptr(new PwSliderPrivate(this)) {
  Q_D(PwSlider);

  Q_INIT_RESOURCE(patchwork);

  d->slider = new QLabel;

  d->label = new QLabel;
  d->label->setAlignment(Qt::AlignCenter);
  d->label->setFrameShape(QFrame::StyledPanel);
  d->label->setFrameShadow(QFrame::Sunken);

  QVBoxLayout *l = qutil::vboxNoMargins();
  QHBoxLayout *h = qutil::hboxNoMargins();
  h->addStretch();
  h->addWidget(d->slider);
  h->addStretch();
  l->addLayout(h);
  l->addWidget(d->label);
  this->setLayout(l);

  d->loadSkin(":/patchwork/images/pwslider-skin.png");
  d->updateValueDisplay(d->value);
}

PwSlider::~PwSlider() {
}

double PwSlider::value() const {
  Q_D(const PwSlider);

  return d->value;
}

void PwSlider::setValue(double value) {
  Q_D(PwSlider);

  value = d->inRange(value);
  if (d->value == value)
    return;
  d->value = value;
  d->updateValueDisplay(value);
  emit valueChanged(value);
}

double PwSlider::minimum() const {
  Q_D(const PwSlider);

  return d->minimum;
}

double PwSlider::maximum() const {
  Q_D(const PwSlider);

  return d->maximum;
}

void PwSlider::setMinimum(double min) {
  Q_D(PwSlider);

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

void PwSlider::setMaximum(double max) {
  Q_D(PwSlider);

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

void PwSlider::setRange(double min, double max) {
  Q_D(PwSlider);

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

double PwSlider::defaultValue() const {
  Q_D(const PwSlider);

  return d->defaultValue;
}

void PwSlider::setDefaultValue(double value) {
  Q_D(PwSlider);

  d->defaultValue = value;
}

double PwSlider::singleStep() const {
  Q_D(const PwSlider);

  return d->singleStep;
}

void PwSlider::setSingleStep(double step) {
  Q_D(PwSlider);

  if (step <= 0)
    return;

  d->singleStep = step;
}

double PwSlider::pageStep() const {
  Q_D(const PwSlider);

  return d->pageStep;
}

void PwSlider::setPageStep(double step) {
  Q_D(PwSlider);

  if (step <= 0)
    return;

  d->pageStep = step;
}

bool PwSlider::tracking() const {
  Q_D(const PwSlider);

  return d->tracking;
}

void PwSlider::setTracking(bool tracking) {
  Q_D(PwSlider);

  d->tracking = tracking;
}

void PwSlider::setLabelVisible(bool visible) {
  Q_D(PwSlider);

  d->label->setVisible(visible);
}

const QString &PwSlider::labelText() {
  Q_D(PwSlider);

  return d->labelText;
}

void PwSlider::setLabelText(const QString &label) {
  Q_D(PwSlider);

  d->labelText = label;
  d->label->setText(label);
}

void PwSlider::mouseMoveEvent(QMouseEvent *e) {
  Q_D(PwSlider);

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

void PwSlider::mousePressEvent(QMouseEvent *e) {
  Q_D(PwSlider);

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

void PwSlider::mouseReleaseEvent(QMouseEvent *e) {
  Q_D(PwSlider);

  if (d->grabPoint) {
    double nv = d->valueForMousePos(e->pos());
    this->setValue(nv);
    d->grabPoint.reset();
    return;
  }
  QWidget::mouseReleaseEvent(e);
}

void PwSlider::wheelEvent(QWheelEvent *e) {
  Q_D(PwSlider);

  double delta = 0.5 * e->angleDelta().y() / 8;
  double nv = d->value + delta * d->singleStep;
  this->setValue(nv);
  d->showValueTip(d->value);
}

void PwSlider::keyPressEvent(QKeyEvent *e) {
  Q_D(PwSlider);

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

void PwSliderPrivate::loadSkin(const QString &path) {
  QVector<QPixmap> skin;
  QPixmap all;
  if (!all.load(path)) {
    qWarning() << "PwSlider could not load skin file" << path;
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

void PwSliderPrivate::updateValueDisplay(double value) {
  if (this->skin.empty()) {
    this->slider->setPixmap(QPixmap());
    return;
  }
  double num = value - this->minimum;
  double den = this->maximum - this->minimum;
  double r = (den == 0) ? 0.0 : (num / den);
  unsigned long n = this->skin.size();
  unsigned long i = std::lround(r * n);
  i = (long(i) < 0) ? 0 : (i >= n) ? (n - 1) : i;
  this->slider->setPixmap(this->skin[i]);
}

void PwSliderPrivate::showValueTip(double value) {
  Q_Q(PwSlider);

  QTimer::singleShot(0, q, [this, value]() {
      int yoff = QFontMetrics(QToolTip::font()).height();
      QPoint toolTipPos = this->slider->mapToGlobal(QPoint(0, 0));
      toolTipPos.ry() -= this->slider->height() / 2 + yoff;
      QString text = QString::number(value, 'f', 3);
      QToolTip::showText(toolTipPos, text, this->slider, QRect(), 500);
    });
}

double PwSliderPrivate::valueForMousePos(QPoint pos) const {
  Q_Q(const PwSlider);

  pos = this->slider->mapFromGlobal(q->mapToGlobal(pos));
  QRect rect = this->slider->rect();

  double r = double(rect.height() - 1 - pos.y()) / (rect.height() - 1);
  return this->inRange(this->minimum + (this->maximum - this->minimum) * r);
}

double PwSliderPrivate::inRange(double value) const {
  return (value < this->minimum) ? this->minimum :
      (value > this->maximum) ? this->maximum : value;
}
