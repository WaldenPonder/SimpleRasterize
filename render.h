#ifndef RENDER_H
#define RENDER_H

#include <QWidget>
#include <QPaintEvent>
#include <QDebug>

class Render : public QWidget
{
    Q_OBJECT

public:
    explicit Render(QWidget *parent = 0);
    ~Render();

     void paintEvent(QPaintEvent*) override;

private:

};

#endif // RENDER_H
