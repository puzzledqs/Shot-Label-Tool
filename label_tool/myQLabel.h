#ifndef MYQLABEL_H
#define MYQLABEL_H
#include <QLabel>

/*************************************************
Class Name： myQLable
Description: 具有单击相应的标签类
*************************************************/
class myQLabel : public QLabel
{
	Q_OBJECT
public:
	explicit myQLabel(QWidget *parent = 0);
signals:
	// 鼠标单击信号
	void clicked(int, int);
	void moved(int, int);
	void released(int , int);

protected:
	void mouseMoveEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	public slots:

};

#endif // MYQLABEL_H