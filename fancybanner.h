#pragma once

#include <QWidget>

class FancyBannerPrivate;
class FancyBanner : public QWidget {
  Q_OBJECT
  Q_DECLARE_PRIVATE(FancyBanner)

 public:
  FancyBanner(QWidget* parent = nullptr);
  ~FancyBanner();

  void addPage(QPixmap pixmap);
  void addPages(QList<QPixmap> pixmaps);

  void startAutoPlay();
  void stopAutoPlay();
  void setDelayTime(int msec, bool start = false);

  void setIndicatorFrontColor(QColor color);
  void setIndicatorBackColor(QColor color);

 signals:
  void currentClicked(int index);

 protected:
  virtual void paintEvent(QPaintEvent* event) override;

  virtual void enterEvent(QEvent* event) override;

  virtual void leaveEvent(QEvent* event) override;

  virtual void resizeEvent(QResizeEvent* event) override;

 private:
  QScopedPointer<FancyBannerPrivate> d_ptr;
};
