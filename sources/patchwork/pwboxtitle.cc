#include "pwboxtitle.h"
#include "qutil/qutil.h"
#include <QHBoxLayout>
#include <QLabel>

class PwBoxTitlePrivate {
  Q_DECLARE_PUBLIC(PwBoxTitle);

  explicit PwBoxTitlePrivate(PwBoxTitle *p) : q_ptr(p) {}
  PwBoxTitle *q_ptr {};

  QLabel *titleLabel {};
};

PwBoxTitle::PwBoxTitle(QWidget *parent)
    : QFrame(parent), d_ptr(new PwBoxTitlePrivate(this)) {
  Q_D(PwBoxTitle);

  Q_INIT_RESOURCE(patchwork);

  QHBoxLayout *l = qutil::hboxNoMargins();
  d->titleLabel = new QLabel("<untitled>");
  d->titleLabel->setObjectName("PwBoxTitleLabel");
  l->addWidget(d->titleLabel);
  this->setLayout(l);

  qutil::setStyleSheetFromFile(this, ":/patchwork/style/pwboxtitle.css");
}

PwBoxTitle::~PwBoxTitle() {
}

QString PwBoxTitle::title() {
  Q_D(PwBoxTitle);

  return d->titleLabel->text();
}

void PwBoxTitle::setTitle(const QString &title) {
  Q_D(PwBoxTitle);

  d->titleLabel->setText(title);
}
