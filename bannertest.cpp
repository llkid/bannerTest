#include "bannertest.h"

#include "fancybanner.h"
#include "stdafx.h"

#include <qdebug.h>

#pragma execution_character_set("utf-8")

bannerTest::bannerTest(QWidget *parent) : QMainWindow(parent) {
  // ui.setupUi(this);

  auto banner = new FancyBanner(this);
  for (int i = 0; i < 10; i++) {
    banner->addPage(QPixmap(QString(":/resource/main/%1").arg(i)));
  }

  this->setCentralWidget(banner);
  banner->setDelayTime(5000, true);

  resize(300, 80);
  connect(banner, &FancyBanner::currentClicked,
          [](int index) { qDebug() << index; });
}
