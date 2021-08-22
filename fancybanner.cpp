#include "fancybanner.h"

#include <qpainter.h>

#include <QMouseEvent>
#include <qdebug.h>

static QColor s_frontColor = QColor(220, 0, 0);
static QColor s_backColor = QColor(200, 200, 200);

static double PAGE_WIDTH = 500;
static double PAGE_HEIGHT = 200;
static double PAGE_WIDTH_OFFSET = 130;
static double PAGE_HEIGHT_OFFSET = 12;


/*!
 * @brief 指示器
 */
class FancyBannerIndicator : public QWidget {
  Q_OBJECT;

 public:
  explicit FancyBannerIndicator(QWidget* parent = nullptr);
  ~FancyBannerIndicator();

  void select(bool selected);

 signals:
  void entered();

 protected:
  virtual void enterEvent(QEvent* event) override;

  virtual void paintEvent(QPaintEvent* event) override;

 private:
  bool selected_;
};

FancyBannerIndicator::FancyBannerIndicator(QWidget* parent)
    : QWidget(parent), selected_(false) {}

FancyBannerIndicator::~FancyBannerIndicator() {}

void FancyBannerIndicator::select(bool selected) {
  this->selected_ = selected;
  update();
}

void FancyBannerIndicator::enterEvent(QEvent* /*event*/) { emit entered(); }

void FancyBannerIndicator::paintEvent(QPaintEvent* /*event*/) {
  QPainter painter(this);
  painter.setPen(Qt::NoPen);
  if (this->selected_) {
    painter.setBrush(s_frontColor);
  } else {
    painter.setBrush(s_backColor);
  }

  painter.drawRect(this->rect());
}

#include <qdebug.h>

/*!
 * @brief 箭头
 */
class FancyBannerArrow : public QWidget {
  Q_OBJECT;

 public:
  enum ArrowType { Left, Right, None};
  explicit FancyBannerArrow(ArrowType type, QWidget* parent = nullptr);
  ~FancyBannerArrow();

 signals:
  void clicked();

 protected:
  virtual void enterEvent(QEvent* event) override;

  virtual void leaveEvent(QEvent* event) override;

  virtual void mouseReleaseEvent(QMouseEvent* event) override;

  virtual void paintEvent(QPaintEvent* event) override;

 private:
  bool mouseHover_;
  ArrowType type_;
};

FancyBannerArrow::FancyBannerArrow(ArrowType type, QWidget* parent)
    : QWidget(parent), mouseHover_(false), type_(type) {
  setFixedSize(12, 20);
}

FancyBannerArrow::~FancyBannerArrow() {}

void FancyBannerArrow::enterEvent(QEvent* /*event*/) {
  mouseHover_ = true;
  update();
}

void FancyBannerArrow::leaveEvent(QEvent* /*event*/) {
  mouseHover_ = false;
  update();
}

void FancyBannerArrow::mouseReleaseEvent(QMouseEvent* event) {
  const QRect rect(this->mapToGlobal(QPoint(0, 0)), this->size());
  if (event->button() == Qt::LeftButton && rect.contains(QCursor::pos())) {
    emit clicked();
  }
}

void FancyBannerArrow::paintEvent(QPaintEvent* event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  QPen pen;
  pen.setWidth(3);
  if (mouseHover_) {
    pen.setBrush(QColor(255, 255, 255));
  } else {
    pen.setBrush(QColor(150, 150, 150));
  }
  painter.setPen(pen);

  const int margin = 2;
  switch (type_) {
    case Right: {
      painter.drawLine(QPoint(margin, margin),
                       QPoint(this->width() - margin, this->height() / 2));
      painter.drawLine(QPoint(margin, this->height() - margin),
                       QPoint(this->width() - margin, this->height() / 2));
    } break;
    case Left: {
      painter.drawLine(QPoint(this->width() - margin, margin),
                       QPoint(margin, this->height() / 2));
      painter.drawLine(QPoint(this->width() - margin, this->height() - margin),
                       QPoint(margin, this->height() / 2));
    } break;
    default:
      break;
  }

  QWidget::paintEvent(event);
}

#include <qlabel.h>

/*!
 * @brief 一个展示页
 */
class FancyBannerPage : public QLabel {
  Q_OBJECT;

 public:
  explicit FancyBannerPage(QWidget* parent = nullptr);
  ~FancyBannerPage();

  void setActive(bool active);
  FancyBannerArrow::ArrowType getArrowType();
  void setArrowType(FancyBannerArrow::ArrowType arrowType);

 signals:
  void clicked();

 protected:
  virtual void mouseReleaseEvent(QMouseEvent* event) override;

  virtual void paintEvent(QPaintEvent* event) override;

 private:
  bool active_;
  FancyBannerArrow::ArrowType arrowType_;
};

FancyBannerPage::FancyBannerPage(QWidget* parent)
    : QLabel(parent), active_(false), arrowType_(FancyBannerArrow::None) {}

FancyBannerPage::~FancyBannerPage() {}

void FancyBannerPage::setActive(bool active) { this->active_ = active; }

FancyBannerArrow::ArrowType FancyBannerPage::getArrowType() {
  return this->arrowType_;
}

void FancyBannerPage::setArrowType(FancyBannerArrow::ArrowType arrowType) {
  this->arrowType_ = arrowType;
}

void FancyBannerPage::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    QRect rect = QRect(this->mapToGlobal(QPoint(0, 0)), this->size());
    if (this->arrowType_ == FancyBannerArrow::Left) {
      rect = QRect(this->mapToGlobal(QPoint(0, 0)),
                   QSize(PAGE_WIDTH_OFFSET, this->height() - PAGE_HEIGHT_OFFSET));
    } else if (this->arrowType_ == FancyBannerArrow::Right) {
      rect =
          QRect(this->mapToGlobal(QPoint(this->width() - PAGE_WIDTH_OFFSET, 0)),
                QSize(PAGE_WIDTH_OFFSET, this->height() - PAGE_HEIGHT_OFFSET));
    }

    if (rect.contains(QCursor::pos())) {
      emit clicked();
    }
  }
}

void FancyBannerPage::paintEvent(QPaintEvent* event) {
  QLabel::paintEvent(event);

  if (!active_) {
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.drawRect(this->rect());
  }
}

#include <qpropertyanimation.h>

/*!
 * @brief banner view
 */
class FancyBannerView : public QWidget {
  Q_OBJECT;

 public:
  explicit FancyBannerView(QWidget* parent = nullptr);
  ~FancyBannerView();

  void setLeftPixmap(const QPixmap& pixmap);
  void setCenterPixmap(const QPixmap& pixmap);
  void setRightPixmap(const QPixmap& pixmap);

  void startAnimation(int direction);

 signals:
  void clicked(int flag);

 private:
  void setPagePixMap(FancyBannerPage* page, const QPixmap& pixmap);
  void setArrowHidden(bool hidden);

 private slots:
  void slotPageClicked();
  void slotArrowClicked();

 protected:
  virtual void enterEvent(QEvent* event) override;

  virtual void leaveEvent(QEvent* event) override;

  virtual void paintEvent(QPaintEvent* event) override;

 private:
  QScopedPointer<FancyBannerPage> leftPage_;
  QScopedPointer<FancyBannerPage> centerPage_;
  QScopedPointer<FancyBannerPage> rightPage_;

  QScopedPointer<FancyBannerArrow> leftArrow_;
  QScopedPointer<FancyBannerArrow> rightArrow_;

  QRect centerRect_;
  QRect leftRect_;
  QRect rightRect_;
};

FancyBannerView::FancyBannerView(QWidget* parent) : QWidget(parent) {
  leftPage_.reset(new FancyBannerPage(this));
  centerPage_.reset(new FancyBannerPage(this));
  rightPage_.reset(new FancyBannerPage(this));

  centerPage_->setActive(true);

  leftPage_->setArrowType(FancyBannerArrow::Left);
  centerPage_->setArrowType(FancyBannerArrow::None);
  rightPage_->setArrowType(FancyBannerArrow::Right);

  connect(leftPage_.get(), &FancyBannerPage::clicked, this,
          &FancyBannerView::slotPageClicked);
  connect(centerPage_.get(), &FancyBannerPage::clicked, this,
          &FancyBannerView::slotPageClicked);
  connect(rightPage_.get(), &FancyBannerPage::clicked, this,
          &FancyBannerView::slotPageClicked);

  this->setMinimumWidth(PAGE_WIDTH + PAGE_WIDTH_OFFSET * 2 + 10);
  this->setFixedHeight(PAGE_HEIGHT);

  centerPage_->setFixedSize(PAGE_WIDTH, PAGE_HEIGHT);

  leftPage_->setFixedWidth(PAGE_WIDTH);
  leftPage_->setFixedHeight(PAGE_HEIGHT - PAGE_HEIGHT_OFFSET);

  rightPage_->setFixedWidth(PAGE_WIDTH);
  rightPage_->setFixedHeight(PAGE_HEIGHT - PAGE_HEIGHT_OFFSET);

  leftArrow_.reset(new FancyBannerArrow(FancyBannerArrow::Left, this));
  rightArrow_.reset(new FancyBannerArrow(FancyBannerArrow::Right, this));
  connect(leftArrow_.get(), &FancyBannerArrow::clicked, this,
          &FancyBannerView::slotArrowClicked);
  connect(rightArrow_.get(), &FancyBannerArrow::clicked, this,
          &FancyBannerView::slotArrowClicked);

  setArrowHidden(true);

  const int x =
      this->x() + this->width() / 2 - PAGE_WIDTH / 2 - PAGE_WIDTH_OFFSET;
  const int y = this->y();

  leftPage_->move(x, y + PAGE_HEIGHT_OFFSET);
  centerPage_->move(x + PAGE_WIDTH_OFFSET, y);
  rightPage_->move(x + PAGE_WIDTH_OFFSET * 2, y + PAGE_HEIGHT_OFFSET);
  centerPage_->raise();

  centerRect_ = centerPage_->frameGeometry();
  leftRect_ = leftPage_->frameGeometry();
  rightRect_ = rightPage_->frameGeometry();
}

FancyBannerView::~FancyBannerView() {}

void FancyBannerView::setLeftPixmap(const QPixmap& pixmap) {
  setPagePixMap(leftPage_.get(), pixmap);
}

void FancyBannerView::setCenterPixmap(const QPixmap& pixmap) {
  setPagePixMap(centerPage_.get(), pixmap);
}

void FancyBannerView::setRightPixmap(const QPixmap& pixmap) {
  setPagePixMap(rightPage_.get(), pixmap);
}

void FancyBannerView::startAnimation(int direction) {
  QRect tmpRect(centerRect_.x(), centerRect_.y() + PAGE_HEIGHT_OFFSET,
                centerRect_.width(), centerRect_.height() - PAGE_HEIGHT_OFFSET);

  QPropertyAnimation* leftAnimation =
      new QPropertyAnimation(leftPage_.get(), "geometry", this);
  QPropertyAnimation* centerAnimation =
      new QPropertyAnimation(centerPage_.get(), "geometry", this);
  QPropertyAnimation* rightAnimation =
      new QPropertyAnimation(rightPage_.get(), "geometry", this);

  leftAnimation->setDuration(200);
  centerAnimation->setDuration(250);
  rightAnimation->setDuration(200);

  if (direction < 0) {
    centerAnimation->setStartValue(leftRect_);
    centerAnimation->setEndValue(centerRect_);
    rightPage_->raise();
  } else if (direction > 0) {
    centerAnimation->setStartValue(rightRect_);
    centerAnimation->setEndValue(centerRect_);
    leftPage_->raise();
  }

  leftAnimation->setStartValue(tmpRect);
  leftAnimation->setEndValue(leftRect_);

  rightAnimation->setStartValue(tmpRect);
  rightAnimation->setEndValue(rightRect_);

  centerPage_->raise();

  leftArrow_->raise();
  rightArrow_->raise();

  leftAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  centerAnimation->start(QAbstractAnimation::DeleteWhenStopped);
  rightAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void FancyBannerView::setPagePixMap(FancyBannerPage* page,
                                    const QPixmap& pixmap) {
  if (page) {
    page->setPixmap(pixmap.scaled(QSize(PAGE_WIDTH, PAGE_HEIGHT),
                                  Qt::IgnoreAspectRatio,
                                  Qt::SmoothTransformation));
  }
}

void FancyBannerView::setArrowHidden(bool hidden) {
  leftArrow_->setHidden(hidden);
  rightArrow_->setHidden(hidden);
  if (!hidden) {
    leftArrow_->move(
        leftRect_.x() + 8,
        leftRect_.y() + (leftRect_.height() - leftArrow_->height()) / 2);
    rightArrow_->move(
        rightRect_.x() + rightRect_.width() - rightArrow_->width() - 8,
        rightRect_.y() + (rightRect_.height() - rightArrow_->height()) / 2);

    leftArrow_->raise();
    rightArrow_->raise();
  }
}

void FancyBannerView::slotPageClicked() {
  FancyBannerPage* page = qobject_cast<FancyBannerPage*>(sender());
  if (page) {
    int flag = 0;
    if (page == leftPage_.get()) {
      flag = -1;
    } else if (page == rightPage_.get()) {
      flag = 1;
    }

    emit clicked(flag);
  }
}

void FancyBannerView::slotArrowClicked() {
  FancyBannerArrow* arrow = qobject_cast<FancyBannerArrow*>(sender());
  if (arrow) {
    if (arrow == leftArrow_.get()) {
      emit clicked(-1);
    } else if (arrow == rightArrow_.get()) {
      emit clicked(1);
    }
  }
}

void FancyBannerView::enterEvent(QEvent* /*event*/) { setArrowHidden(false); }

void FancyBannerView::leaveEvent(QEvent* /*event*/) { setArrowHidden(true); }

void FancyBannerView::paintEvent(QPaintEvent* event) {
  QWidget::paintEvent(event);
}

#include <qtimer.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

/*!
 * @brief 接口集合
 */
class FancyBannerPrivate : public QObject {
  Q_OBJECT
  Q_DECLARE_PUBLIC(FancyBanner)

 public:
  FancyBannerPrivate();
  ~FancyBannerPrivate();

  void init();
  void addPage(QPixmap pixmap);
  void addPages(QList<QPixmap> pixmap);
  void play(int index);

 public slots:
  void switchIndicator();
  void switchPage(int flag);
  void slotTimeOut();

 private:
  FancyBanner* q_ptr;
  QScopedPointer<FancyBannerView> view_;
  QHBoxLayout* indicatorLayout_;
  QList<QPixmap> pixmaps_;
  QList<FancyBannerIndicator*> indicators_;
  int currentIndex_;
  QTimer* timer_;
  int interval_;
};

FancyBannerPrivate::FancyBannerPrivate()
    : q_ptr(nullptr),
      view_(nullptr),
      indicatorLayout_(nullptr),
      currentIndex_(-1),
      timer_(nullptr),
      interval_(2000) {}

FancyBannerPrivate::~FancyBannerPrivate() {
  delete indicatorLayout_;
  indicatorLayout_ = nullptr;

  qDeleteAll(indicators_);

  if (timer_->isActive()) {
    timer_->stop();
  }
}

void FancyBannerPrivate::init() {
  Q_Q(FancyBanner);

  indicatorLayout_ = new QHBoxLayout;
  indicatorLayout_->setSpacing(5);
  view_.reset(new FancyBannerView);
  connect(view_.get(), &FancyBannerView::clicked, this,
          &FancyBannerPrivate::switchPage);

  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &FancyBannerPrivate::slotTimeOut);

  QHBoxLayout* bottomLayout = new QHBoxLayout;
  bottomLayout->setSpacing(0);
  bottomLayout->setMargin(0);
  bottomLayout->addStretch();
  bottomLayout->addLayout(indicatorLayout_);
  bottomLayout->addStretch();

  QHBoxLayout* viewLayout = new QHBoxLayout;
  viewLayout->setSpacing(0);
  viewLayout->setMargin(0);
  viewLayout->addStretch();
  viewLayout->addWidget(view_.get());
  viewLayout->addStretch();

  QVBoxLayout* mainLayout = new QVBoxLayout;
  mainLayout->addStretch();
  mainLayout->addLayout(viewLayout);
  mainLayout->addLayout(bottomLayout);
  mainLayout->addStretch();
  q->setLayout(mainLayout);
}

void FancyBannerPrivate::addPage(QPixmap pixmap) {
  pixmaps_.append(pixmap);

  auto indicator = new FancyBannerIndicator;
  indicators_.append(indicator);
  indicator->setFixedSize(18, 3);
  connect(indicator, &FancyBannerIndicator::entered, this,
          &FancyBannerPrivate::switchIndicator);
  indicatorLayout_->addWidget(indicator);

  if (currentIndex_ == -1) {
    currentIndex_ = 0;
    indicators_.at(currentIndex_)->select(true);
  }

  play(currentIndex_);
}

void FancyBannerPrivate::addPages(QList<QPixmap> pixmap) {
  for each (QPixmap pixmap in pixmaps_) {
    addPage(pixmap);
  }
}

void FancyBannerPrivate::play(int index) {
  if (index < 0) return;

  int count = indicators_.count();
  int leftIndex = 0, rightIndex = 0;
  if (count > 1) {
    if (index == 0) {
      leftIndex = count - 1;
      rightIndex = index + 1;
    } else if (index == count - 1) {
      leftIndex = index - 1;
      rightIndex = 0;
    } else {
      leftIndex = index - 1;
      rightIndex = index + 1;
    }
  }

  view_->setLeftPixmap(pixmaps_.at(leftIndex));
  view_->setCenterPixmap(pixmaps_.at(index));
  view_->setRightPixmap(pixmaps_.at(rightIndex));
}

void FancyBannerPrivate::switchIndicator() {
  auto indicator = qobject_cast<FancyBannerIndicator*>(sender());
  if (indicator == nullptr) return;

  int index = indicators_.indexOf(indicator);
  if (index == currentIndex_) return;

  if (currentIndex_ != -1) {
    indicators_.at(currentIndex_)->select(false);
  }

  int flag = 0;
  if (currentIndex_ < index) {
    flag = 1;
  } else if (currentIndex_ > index) {
    flag = -1;
  }

  currentIndex_ = index;
  indicators_.at(currentIndex_)->select(true);

  play(currentIndex_);
  view_->startAnimation(flag);
}

void FancyBannerPrivate::switchPage(int flag) {
  Q_Q(FancyBanner);

  if (timer_->isActive()) {
    timer_->start(interval_);
  }

  if (currentIndex_ == -1) return;

  if (flag == 0) {
    emit q->currentClicked(currentIndex_);
    return;
  }

  int count = indicators_.count();
  if (count < 2) return;

  indicators_.at(currentIndex_)->select(false);
  currentIndex_ += flag;
  if (currentIndex_ == -1) {
    currentIndex_ = count - 1;
  } else if (currentIndex_ == count) {
    currentIndex_ = 0;
  }

  indicators_.at(currentIndex_)->select(true);
  play(currentIndex_);
  view_->startAnimation(flag);
}

void FancyBannerPrivate::slotTimeOut() { switchPage(1); }

static QRect BannerRect;

FancyBanner::FancyBanner(QWidget* parent)
    : QWidget(parent), d_ptr(new FancyBannerPrivate) {
  d_ptr->q_ptr = this;

  Q_D(FancyBanner);

  d->init();

  BannerRect = this->geometry();
}

FancyBanner::~FancyBanner() {}

void FancyBanner::addPage(QPixmap pixmap) {
  Q_D(FancyBanner);
  d->addPage(pixmap);
}

void FancyBanner::addPages(QList<QPixmap> pixmaps) {
  Q_D(FancyBanner);
  d->addPages(pixmaps);
}

void FancyBanner::startAutoPlay() {
  Q_D(FancyBanner);
  d->timer_->start(d->interval_);
}

void FancyBanner::stopAutoPlay() {
  Q_D(FancyBanner);
  d->timer_->stop();
}

void FancyBanner::setDelayTime(int msec, bool start) {
  if (msec < 500 || msec > 10000) return;
  Q_D(FancyBanner);

  d->interval_ = msec;

  if (start) {
    startAutoPlay();
  }
}

void FancyBanner::setIndicatorFrontColor(QColor color) { s_frontColor = color; }

void FancyBanner::setIndicatorBackColor(QColor color) { s_backColor = color; }

void FancyBanner::paintEvent(QPaintEvent* event) { QWidget::paintEvent(event); }

void FancyBanner::enterEvent(QEvent* event) { stopAutoPlay(); }

void FancyBanner::leaveEvent(QEvent* event) { startAutoPlay(); }

void FancyBanner::resizeEvent(QResizeEvent* event) {}

#include "fancybanner.moc"