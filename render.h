#ifndef RENDER_H
#define RENDER_H

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QDebug>

class Render : public QWidget
{
    Q_OBJECT

public:
    explicit Render(QWidget *parent = 0);
    ~Render();

     virtual void paintEvent(QPaintEvent*) override;
	 virtual void resizeEvent(QResizeEvent *event) override;
	 virtual void keyPressEvent(QKeyEvent *event) override;

private:
	struct Impl;
	Impl* impl;
};

#endif // RENDER_H
