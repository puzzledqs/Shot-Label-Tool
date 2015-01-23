#include "myQLabel.h"
#include <QMouseEvent>

// mark: public:--------------------------------------------------
myQLabel::myQLabel(QWidget *parent):QLabel(parent)
{
	
}

// mark: protected:------------------------------------------------
void myQLabel::mouseMoveEvent(QMouseEvent *ev)
{
	emit moved(ev->pos().x(),ev->pos().y());//(this);
}

void myQLabel::mousePressEvent(QMouseEvent *ev)
{
	emit clicked(ev->pos().x(), ev->pos().y());//(this);
}

void myQLabel::mouseReleaseEvent(QMouseEvent *ev)
{
	emit released(ev->pos().x(), ev->pos().y());
}