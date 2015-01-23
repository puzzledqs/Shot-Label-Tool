#ifndef LABEL_TOOL_H
#define LABEL_TOOL_H

#include <QtWidgets/QMainWindow>
#include "ui_label_tool.h"
#include "myQLabel.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QWaitCondition>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QSlider>
#include <QLineEdit>
#include <QDirIterator>
#include <QFile>
#include <QGridLayout>
#include <QTextEdit>
#include <QListWidget>
#include <QGroupBox>
#include <QRadioButton>
#include <QSizePolicy>
#include <QShortcut>

#include <vector>
#include <io.h>
#include <fstream>
#include <sstream>


using namespace std;
using namespace cv;

class ClipInfo {
public:
    enum BKLabel {No = 0, Yes, Unclear} bk;
    static const Qt::GlobalColor Colors[];
    int start;
    int end;
    bool isHumanCut;
    int attr;

    void copyLabelFrom(ClipInfo src) {
        bk = src.bk;
        attr = src.attr;
    }

    ClipInfo() {
        start = end = 0;
        attr = 0;
        isHumanCut = false;
        bk = No;

    }

    void getStringForDisplay(char* out_str) {
        sprintf(out_str, "%06d --> %06d, %d, %d", start, end, (int)bk, attr + 1);
    }

    string getStringForFile() {
        ostringstream os;
        os << isHumanCut << " " << start << " " << end << " " << (int)bk << " " << attr;
        return os.str();
    }

    void parseFromString(const string& str) {
        istringstream is(str);
        int _bk;
        is >> isHumanCut >> start >> end >> _bk >> attr;
        bk = BKLabel(_bk);
    }

    Qt::GlobalColor getColor() {
        return Colors[(int)bk];        
    }
};


class label_tool : public QMainWindow
{
	Q_OBJECT

public:
	label_tool(QWidget *parent = 0);
	~label_tool();

    void init();

private:

	static const int BBOX_AREA_CHANGE_STEP;
	static const int BBOX_LOC_CHANGE_STEP;
    static const int MAX_WIDTH;
    static const int MAX_HEIGHT;

	Ui::label_toolClass ui;

	//存储视频路径
	vector<string> videolist;
	int video_count_now;

	//视频变量
	VideoCapture capture;
	int frame_count;
	int frame_width;
	int frame_height;
	double rate;

	//图片变量
	Mat frame_cur;
	Mat frame_next;
	Mat frame_show;
	QImage qimg;

	int frame_count_now;//记录当前是第几帧

	//统计直方图用
	Mat hsv_frame_next, hsv_frame;
	int h_bins; 
	int s_bins;
	int *histSize;
	// hue varies from 0 to 256, saturation from 0 to 180
	float *h_ranges ;
	float *s_ranges;
	const float** ranges;
	// Use the o-th and 1-st channels
	int *channels;
	MatND hist_frame_next;
	MatND hist_frame;

	double diff_threshold;

	int speed;//播放速度
    bool isPlay;
    bool isError;
    bool isLarge;

    QLabel *folder_name_label;
    QPushButton *open_button;//打开按钮
    QLineEdit *folder_name_lineEdit;//显示文件列表
	
    QLabel *file_name_label;
    QPushButton *prev_button;
    QPushButton *next_button;//下一个视频按钮
    QLabel *video_num_label;//显示当前是第几个视频
    QLineEdit *video_idx_lineEdit;
    QPushButton *goto_button;

    myQLabel *image_label;
	QSlider *speed_slider;//播放速率
    QLabel *dummy_space_label;
	QLabel *speed_slider_label;
    QLabel *threshold_label;
    QLineEdit *threshold_lineEdit;//显示


	QTimer  show_timer;//显示视频的计时器
	QSlider *slider;//进度条
	QLabel *slider_label;
	QLineEdit *slider_lineEdit;


	QPushButton *play_button;//停止
    QPushButton *back_button;
    QPushButton *fast_back_button;
    QPushButton *forward_button;
    QPushButton *fast_forward_button;

	QPushButton *cut_button;//cut a clip
	QPushButton *delete_button;//delete and merge with previous

    QGroupBox* bk_groupbox;
    QGroupBox* attr_groupbox;
    QRadioButton* bk_buttons[3];
    QRadioButton* attr_buttons[11];

    QListWidget *clip_list;
    vector<ClipInfo> infos;
    int clip_start;

    static const int bk_label_num;
    static const int attr_label_num;

protected:
    void draw_bbox();//画图
    void getSubFolderVideos(QString path, vector<string>& videolist);
	void keyPressEvent(QKeyEvent *e);//按键响应
	void goToVideo(int n);
    void initClipList();
    void addClip(bool isHuman);
    void updateClipListSelection();
    int getCurrentClipIdx();
    void modifyClipListItem(ClipInfo& ci, int idx);
    void insertClipListItem(ClipInfo& ci, int idx);
    void updateClipStartItem();
    void colorClipListItem(ClipInfo& ci, int idx);
    void batchSetButtonStatus1(bool status);
    void batchSetButtonStatus2(bool status);
    void setLabelForClip(ClipInfo& ci);
    void save();
    void load();
    string getResultFileName();
    string replaceAll(string std, const string& from, const string& to);

private slots:
	//图片鼠标响应

	void openButtonClicked();//打开文件夹中的视频
	void nextButtonClicked();//下一个视频
    void prevButtonClicked();
    void gotoButtonClicked();

	void cutButtonClicked();//新疆框
	void deleteButtonClicked();//删除框

	void goonButtonClicked();//继续
	void stopButtonClicked();//停止
    void playButtonClicked();
    void backButtonClicked();
    void fastBackButtonClicked();
    void forwardButtonClicked();
    void fastForwardButtonClicked();

	void updateImage();//更新图像
	
	void threshold_lineEdit_change(const QString &);//更改id
	void speedSliderChange();

    void sliderChange(int value);//进度条响应
    void sliderLineEditChange(const QString &text);
    void clipListClicked(QListWidgetItem *item);
    void clipListSelectionChanged(int idx);

    void disableAttrs();
    void enableAttrs();
    void bkButtonsClicked();
    void attrButtonsClicked();    
};

#endif // LABEL_TOOL_H

