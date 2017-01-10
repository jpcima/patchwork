#include "pwboxitem.h"
#include "pwboxtitle.h"
#include "qutil/qutil.h"
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneHoverEvent>
#include <QVBoxLayout>
#include <QFrame>
#include <QTimer>
#include <QDebug>
#include <boost/cast.hpp>

class PwBoxItemContainer;
///

class PwBoxItemPrivate {
  Q_DECLARE_PUBLIC(PwBoxItem);

  PwBoxItemPrivate(PwBoxItem *p) : q_ptr(p) {}
  PwBoxItem *q_ptr {};
  PwBoxTitle *title {};
  QGraphicsProxyWidget *proxy {};
  QWidget *centralWidget {};
  QWidget *centralWidgetWrapper {};
  PwBoxItemContainer *frame() const;
  void updateRect();
  void adjustWidthToTitle();
};

class PwBoxItemContainer : public QFrame {
 public:
  explicit PwBoxItemContainer(QWidget *parent = nullptr)
      : QFrame(parent) {}
  void setEndpointBoundingSize(QSize size) {
    epsize_ = size;
  }
  QSize sizeHint() const override {
    QSize orig = QFrame::sizeHint();
    return { qMax(epsize_.width(), orig.width()),
             qMax(epsize_.height(), orig.height()) };
  }
  QSize minimumSizeHint() const override {
    QSize orig = QFrame::minimumSizeHint();
    return { qMax(epsize_.width(), orig.width()),
             qMax(epsize_.height(), orig.height()) };
  }
 private:
  QSize epsize_;
};

PwBoxItem::PwBoxItem(QGraphicsItem *parent)
    : QGraphicsRectItem(parent), d_ptr(new PwBoxItemPrivate(this)) {
  Q_D(PwBoxItem);

  Q_INIT_RESOURCE(patchwork);

  d->proxy = new QGraphicsProxyWidget(this);

  QVBoxLayout *l = qutil::vboxNoMargins();
  d->title = new PwBoxTitle;
  d->title->setSizePolicy(
      QSizePolicy::Preferred, QSizePolicy::Maximum);
  l->addWidget(d->title);

  d->centralWidgetWrapper = new QWidget;
  l->addWidget(d->centralWidgetWrapper);
  this->setCentralWidget(new QWidget);

  PwBoxItemContainer *w = new PwBoxItemContainer;
  w->setObjectName("PwBoxItemFrame");
  w->setLayout(l);
  d->proxy->setWidget(w);
  Q_ASSERT(!w->parent());

  this->setTitle({});

  qutil::setStyleSheetFromFile(w, ":/patchwork/style/pwboxitem.css");

  d->updateRect();
  qutil::installFunctionalEventFilter(w, [this, d](QEvent *ev) -> bool {
    if (ev->type() == QEvent::LayoutRequest)
      QTimer::singleShot(0, this, [d]() { d->updateRect(); });
    return false;
  });

  qutil::installFunctionalEventFilter(d->title, [this](QEvent *ev) -> bool {
      if (ev->type() == QEvent::Enter)
        this->setOpacity(0.7);
      else if (ev->type() == QEvent::Leave)
        this->setOpacity(1.0);
      return false;
    });

  this->setFlag(QGraphicsItem::ItemIsMovable);
  this->setFlag(QGraphicsItem::ItemIsSelectable);
  this->setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

PwBoxItem::~PwBoxItem() {
  Q_D(const PwBoxItem);

  QWidget *w = d->frame();
  d->proxy->setWidget(nullptr);
  delete w;
}

void PwBoxItem::setTitle(const QString &title) {
  Q_D(PwBoxItem);

  if (title.isNull()) {
    d->title->setVisible(false);
  } else {
    d->title->setVisible(true);
    d->title->setTitle(title);
    d->adjustWidthToTitle();
  }
}

void PwBoxItem::setCentralWidget(QWidget *w) {
  Q_D(PwBoxItem);

  QLayout *l = d->centralWidgetWrapper->layout();
  if (l) {
    delete l->takeAt(0);
  } else {
    l = qutil::vboxNoMargins();
    d->centralWidgetWrapper->setLayout(l);
  }
  l->addWidget(w);
  d->centralWidget = w;
  w->setObjectName("PwBoxItemCentralWidget");
}

QWidget *PwBoxItem::centralWidget() const {
  Q_D(const PwBoxItem);

  return d->centralWidget;
}

void PwBoxItem::installContainerEventFilter(QObject *filter) {
  Q_D(const PwBoxItem);

  d->frame()->installEventFilter(filter);
}

void PwBoxItem::setEndpointBoundingSize(QSize size) {
  Q_D(const PwBoxItem);

  d->frame()->setEndpointBoundingSize(size);
}

void PwBoxItem::updateContainerSize() {
  Q_D(const PwBoxItem);

  d->frame()->updateGeometry();
  d->frame()->adjustSize();
}

QVariant PwBoxItem::itemChange(
    GraphicsItemChange change, const QVariant &value) {
  if (change == ItemPositionChange)
    emit this->itemPositionChanged(value.toPointF());
  return QGraphicsItem::itemChange(change, value);
}

PwBoxItemContainer *PwBoxItemPrivate::frame() const {
  return boost::polymorphic_downcast<PwBoxItemContainer *>(
      this->proxy->widget());
}

void PwBoxItemPrivate::updateRect() {
  Q_Q(PwBoxItem);

  QRectF rect = this->proxy->rect();
  q->setRect(rect);
}

void PwBoxItemPrivate::adjustWidthToTitle() {
  // require the minimum width allowing the title to fit
  QFontMetrics metrics(this->proxy->font());
  this->proxy->setMinimumWidth(metrics.width(this->title->title()));
}
