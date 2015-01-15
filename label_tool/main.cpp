#include "label_tool.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	label_tool w;
	w.show();
    QFont font = a.font();
    font.setPointSize(10);
    a.setFont(font);
	return a.exec();
}
