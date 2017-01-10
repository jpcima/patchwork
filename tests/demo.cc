#include "patchwork.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

void createDemo(PwAreaScene *scene);
QWidget *createComplexWidget();
///

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  PwArea *area = new PwArea;
  PwAreaScene *scene = area->scene();

  QMainWindow *win = new QMainWindow;
  win->setCentralWidget(area);

  QToolBar *tb = new QToolBar;
  win->addToolBar(tb);

  QAction *aAutoLayout = new QAction("Auto layout", tb);
  aAutoLayout->setShortcut(QKeySequence("Ctrl+L"));
  tb->addAction(aAutoLayout);

  QObject::connect(aAutoLayout, &QAction::triggered,
                   aAutoLayout, [scene]() { scene->autoLayout(); });

  win->resize(800, 800);
  createDemo(scene);
  win->show();

  return app.exec();
}

void createDemo(PwAreaScene *scene) {
  PwEndpoint *ep;

  PwNode *c1 = scene->createNode("c1");
  c1->setTitle("Component 1");
  ep = c1->createEndpoint("i1", PwEndpoint::Input);
  ep = c1->createEndpoint("i2", PwEndpoint::Input);
  ep = c1->createEndpoint("i3", PwEndpoint::Input);
  ep = c1->createEndpoint("i4", PwEndpoint::Input);
  ep = c1->createEndpoint("o1", PwEndpoint::Output);
  ep = c1->createEndpoint("o2", PwEndpoint::Output);

  PwNode *c2 = scene->createNode("c2");
  c2->setTitle("Component 2");
  ep = c2->createEndpoint("i1", PwEndpoint::Input);
  ep = c2->createEndpoint("i2", PwEndpoint::Input);
  ep = c2->createEndpoint("o1", PwEndpoint::Output);
  ep->setPlacement(PwEndpoint::RightPlacement);
  ep = c2->createEndpoint("o2", PwEndpoint::Output);
  ep->setPlacement(PwEndpoint::RightPlacement);
  ep = c2->createEndpoint("o3", PwEndpoint::Output);
  ep->setPlacement(PwEndpoint::RightPlacement);
  ep = c2->createEndpoint("o4", PwEndpoint::Output);
  ep->setPlacement(PwEndpoint::RightPlacement);

  PwNode *t1 = scene->createNode("e1");
  t1->setTitle("Expression");
  ep = t1->createEndpoint("i1", PwEndpoint::Input);
  ep = t1->createEndpoint("i2", PwEndpoint::Input);
  ep = t1->createEndpoint("o1", PwEndpoint::Output);
  t1->setCentralWidget(new QLineEdit("a+2*b"));

  PwNode *c3 = scene->createNode("c3");
  c3->setTitle("Component 3");
  ep = c3->createEndpoint("i1", PwEndpoint::Input);
  ep = c3->createEndpoint("o1", PwEndpoint::Output);

  PwNode *c4 = scene->createNode("c4");
  c4->setTitle("Component 4");
  ep = c4->createEndpoint("i1", PwEndpoint::Input);
  ep = c4->createEndpoint("i2", PwEndpoint::Input);
  ep = c4->createEndpoint("o1", PwEndpoint::Output);
  c4->setCentralWidget(createComplexWidget());

  scene->createConnector(c1->getEndpoint("o1"), c2->getEndpoint("i1"));
  scene->createConnector(t1->getEndpoint("o1"), c3->getEndpoint("i1"));
  scene->createConnector(c3->getEndpoint("o1"), c4->getEndpoint("i1"));
  scene->createConnector(c2->getEndpoint("o1"), c4->getEndpoint("i2"));
}

QWidget *createComplexWidget() {
  PwSlider *slider;
  PwDial *dial;

  QFrame *frame = new QFrame;
  QVBoxLayout *v = new QVBoxLayout;
  frame->setLayout(v);
  QHBoxLayout *h1 = new QHBoxLayout;
  slider = new PwSlider;
  slider->setLabelText("Attack");
  h1->addWidget(slider);
  slider = new PwSlider;
  slider->setLabelText("Decay");
  h1->addWidget(slider);
  slider = new PwSlider;
  slider->setLabelText("Release");
  h1->addWidget(slider);
  v->addLayout(h1);
  QHBoxLayout *h2 = new QHBoxLayout;
  dial = new PwDial;
  dial->setLabelText("Rate");
  h2->addWidget(dial);
  dial = new PwDial;
  dial->setLabelText("Depth");
  h2->addWidget(dial);
  v->addLayout(h2);
  return frame;
}
