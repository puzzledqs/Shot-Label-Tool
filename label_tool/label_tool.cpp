#include "label_tool.h"

const int label_tool::BBOX_AREA_CHANGE_STEP = 5;
const int label_tool::BBOX_LOC_CHANGE_STEP = 5;
const int label_tool::bk_label_num = 3;
const int label_tool::attr_label_num = 10;
const Qt::GlobalColor ClipInfo::Colors[] = {Qt::green, Qt::red, Qt::white};


label_tool::label_tool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
    this->move(200, 200);

    /*QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    this->setSizePolicy(sizePolicy);*/
    

    //----------------------------------------- global grid layout
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QGridLayout *grid = new QGridLayout;
    grid->setSpacing(5);
    centralWidget->setLayout(grid);
    //grid->setSizeConstraint(QLayout::SetFixedSize);
    //-----------------------------------------

	//----------------------------------- folder selection
	folder_name_lineEdit = new QLineEdit(centralWidget);
    folder_name_lineEdit->setText("");
    grid->addWidget(folder_name_lineEdit, 0, 0);

	open_button = new QPushButton(centralWidget);
	open_button->setText(tr("Load videos"));
    grid->addWidget(open_button, 0, 1);

	connect(open_button, SIGNAL(clicked()),this, SLOT(openButtonClicked()));
    //------------------------------------

    //-------------------------------- video navigation panel
    file_name_label = new QLabel(centralWidget);
	next_button = new QPushButton(QString("Next >"), centralWidget);
    prev_button = new QPushButton(QString("< Prev"), centralWidget);
    video_num_label = new QLabel(QString("   /    "), centralWidget);
    video_idx_lineEdit = new QLineEdit(centralWidget);
    goto_button = new QPushButton(QString("Go"), centralWidget);
    // layout
    QHBoxLayout *video_nvg_panel = new QHBoxLayout;
    video_nvg_panel->addStretch(1);
    video_nvg_panel->addWidget(file_name_label, 0, Qt::AlignCenter);
    video_nvg_panel->addWidget(prev_button, 0, Qt::AlignCenter);
    video_nvg_panel->addWidget(next_button, 0, Qt::AlignCenter); 
    video_nvg_panel->addWidget(video_num_label, 0, Qt::AlignCenter);
    video_nvg_panel->addWidget(video_idx_lineEdit, 0, Qt::AlignCenter);
    video_nvg_panel->addWidget(goto_button, 0, Qt::AlignCenter);
    video_nvg_panel->addStretch(1);
    grid->addLayout(video_nvg_panel, 1, 0, 1, 2);
    // callbacks
	connect(next_button, SIGNAL(clicked()), this, SLOT(nextButtonClicked()));
    connect(prev_button, SIGNAL(clicked()), this, SLOT(prevButtonClicked()));
    connect(goto_button, SIGNAL(clicked()), this, SLOT(gotoButtonClicked()));
    // inits
    video_idx_lineEdit->setFixedWidth(40);
    //-------------------------------------

    //-----------------------------------------  image display
	image_label = new myQLabel(centralWidget);
    //image_label->setFixedSize(QSize(300, 200));
	image_label->setMouseTracking(true);
    speed_slider_label = new QLabel(QString::fromLocal8Bit("播放速度:"), centralWidget);
    speed_slider = new QSlider(Qt::Vertical, centralWidget);
    dummy_space_label = new QLabel(" ");
    threshold_label = new QLabel(QString::fromLocal8Bit("自动分割阈值:"), centralWidget);
    threshold_lineEdit = new QLineEdit(centralWidget);
    QVBoxLayout *v_panel = new QVBoxLayout;
    v_panel->addStretch(1);
    v_panel->addWidget(speed_slider_label);
    v_panel->addWidget(speed_slider);
    v_panel->addWidget(dummy_space_label);
    v_panel->addWidget(threshold_label);
    v_panel->addWidget(threshold_lineEdit);
    QHBoxLayout *h_panel = new QHBoxLayout;
    h_panel->addWidget(image_label, 1);
    h_panel->addLayout(v_panel);
    grid->addLayout(h_panel, 2, 0);
    //callbacks
    connect(threshold_lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(threshold_lineEdit_change(const QString &)));
	connect(speed_slider,SIGNAL(valueChanged(int)),this,SLOT(speedSliderChange()));
    //inits
    speed_slider->setMinimum(1);
	speed_slider->setMaximum(10);
    speed_slider->setSingleStep(1);
    speed_slider->setPageStep(5);
	speed_slider->setValue(1);
    threshold_lineEdit->setText(QString("0.3"));
    speed_slider->setValue(1);
    //----------------------------------------

	//------------------------------------------
	connect(&show_timer, &QTimer::timeout, this, &label_tool::updateImage);
	slider = new QSlider(Qt::Horizontal, centralWidget);
    slider_label = new QLabel(QString("/"), centralWidget);
    slider_lineEdit = new QLineEdit(centralWidget);
	QHBoxLayout* h_panel2 = new QHBoxLayout;
    h_panel2->addWidget(slider_lineEdit);
    h_panel2->addWidget(slider_label);
    h_panel2->addWidget(slider, 1);
    grid->addLayout(h_panel2, 3, 0);
    slider->setMinimum(0);
    slider_lineEdit->setFixedWidth(50);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChange(int)));
	connect(slider_lineEdit,SIGNAL(textChanged(const QString &)), this, SLOT(sliderLineEditChange(const QString &)));
    //--------------------------------------------	

    //------------------------------------------
    play_button = new QPushButton(QString("Play"), centralWidget);
    back_button = new QPushButton(QString("<"), centralWidget);
    fast_back_button = new QPushButton(QString("<<"), centralWidget);
    forward_button = new QPushButton(QString(">"), centralWidget);
    fast_forward_button = new QPushButton(QString(">>"), centralWidget);
    QHBoxLayout* h_panel3 = new QHBoxLayout;
    h_panel3->addStretch(1);
    h_panel3->addWidget(fast_back_button, 0, Qt::AlignCenter);
    h_panel3->addWidget(back_button, 0, Qt::AlignCenter);
    h_panel3->addWidget(play_button, 0, Qt::AlignCenter);
    h_panel3->addWidget(forward_button, 0, Qt::AlignCenter);
    h_panel3->addWidget(fast_forward_button, 0, Qt::AlignCenter);
    h_panel3->addStretch(1);
    grid->addLayout(h_panel3, 4, 0);
    //callbacks
    connect(play_button, SIGNAL(clicked()), this, SLOT(playButtonClicked()));
    connect(back_button, SIGNAL(clicked()), this, SLOT(backButtonClicked()));
    connect(fast_back_button, SIGNAL(clicked()), this, SLOT(fastBackButtonClicked()));
    connect(forward_button, SIGNAL(clicked()), this, SLOT(forwardButtonClicked()));
    connect(fast_forward_button, SIGNAL(clicked()), this, SLOT(fastForwardButtonClicked()));
    //----------------------------------------

    //----------------------------------------  Clip List & Label Panel
    clip_list = new QListWidget(centralWidget);
    clip_list->setFixedWidth(180);
    connect(clip_list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(clipListClicked(QListWidgetItem*)));
    connect(clip_list, SIGNAL(currentRowChanged(int)), this, SLOT(clipListSelectionChanged(int)));

    QString str;
    QListWidgetItem* tmp_item = new QListWidgetItem(QString::fromLocal8Bit("Hello, world！"));
    tmp_item->setForeground(Qt::red);
    clip_list->addItem(tmp_item);
    //-----------
    char bk_texts[bk_label_num][10] = {"否", "是", "可疑"};
    bk_groupbox = new QGroupBox(QString::fromLocal8Bit("是否暴恐镜头？"));
    QHBoxLayout* h_panel21 = new QHBoxLayout;
    for (int i = 0; i < 3; i++) {
        bk_buttons[i] = new QRadioButton(QString::fromLocal8Bit(bk_texts[i]));
        h_panel21->addWidget(bk_buttons[i]);
    }
    bk_groupbox->setLayout(h_panel21);
    //------------
    char attr_texts[attr_label_num][256] = {"1.单人演讲", "2.演练示威", 
                                           "3.使用武器", "4.符号文字",
                                           "5.处置人质", "6.尸体", 
                                           "7.打砸抢", "8.群体聚集",
                                           "9.爆炸", "10.其他"  };
    attr_groupbox = new QGroupBox(QString::fromLocal8Bit("类型:"));                      
    QGridLayout* g_panel = new QGridLayout;
    for (int i = 0; i < attr_label_num; i++) {
        attr_buttons[i] = new QRadioButton(QString::fromLocal8Bit(attr_texts[i]));
        g_panel->addWidget(attr_buttons[i], i / 2, i % 2);
    }
    attr_groupbox->setLayout(g_panel);
    QVBoxLayout* v_panel21 = new QVBoxLayout;
    v_panel21->addWidget(clip_list);
    v_panel21->addWidget(bk_groupbox);
    v_panel21->addWidget(attr_groupbox);
    grid->addLayout(v_panel21, 2, 1, 4, 1);   
    //--callbacks                                                             
    connect(bk_buttons[0], SIGNAL(clicked()), this, SLOT(disableAttrs()));
    connect(bk_buttons[1], SIGNAL(clicked()), this, SLOT(enableAttrs()));
    connect(bk_buttons[2], SIGNAL(clicked()), this, SLOT(enableAttrs()));
    for (int i = 0; i < bk_label_num; i++)
        connect(bk_buttons[i], SIGNAL(clicked()), this, SLOT(bkButtonsClicked()));
    for (int i = 0; i < attr_label_num; i++)
        connect(attr_buttons[i], SIGNAL(clicked()), this, SLOT(attrButtonsClicked()));


    //-------------------------------------------------------
	cut_button = new QPushButton(centralWidget);
	cut_button->setText("Cut");
	delete_button = new QPushButton(centralWidget);
	delete_button->setText("Delete");
    // panel
    QHBoxLayout* h_panel5 = new QHBoxLayout;
    h_panel5->addStretch(1);
    h_panel5->addWidget(cut_button, 0, Qt::AlignCenter);
    h_panel5->addWidget(delete_button, 0, Qt::AlignCenter);
    h_panel5->addStretch(1);
    grid->addLayout(h_panel5, 5, 0);
    // callbacks
    connect(cut_button, SIGNAL(clicked()), this, SLOT(cutButtonClicked()));
	connect(delete_button, SIGNAL(clicked()), this, SLOT(deleteButtonClicked()));
    //--------------------------------------------------------

    //---------------------------------- Global shortcuts
    back_button->setShortcut(Qt::Key_S);
    forward_button->setShortcut(Qt::Key_D);
    fast_back_button->setShortcut(Qt::Key_A);
    fast_forward_button->setShortcut(Qt::Key_F);
    QShortcut *shortcut = new QShortcut(Qt::Key_Space, this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(playButtonClicked()));
    //----------------------------------------

	h_bins = 50; 
	s_bins = 60;
	histSize = new int[2];
	histSize[0] = h_bins;
	histSize[1] = s_bins;

	// hue varies from 0 to 256, saturation from 0 to 180
	h_ranges = new float[2];
	s_ranges = new float[2];
	h_ranges[0] =  0;
	h_ranges[1] = 256;
	s_ranges[0] =  0;
	s_ranges[1] = 180;

	ranges = new const float*[2];
	ranges[0] = h_ranges;
	ranges[1] = s_ranges;

	// Use the o-th and 1-st channels
	channels = new int[2];
	channels[0] = 0;
	channels[1] = 1;

    //init();
    batchSetButtonStatus1(false);
    batchSetButtonStatus2(false);
}

label_tool::~label_tool()
{

}

void label_tool::init() {
    openButtonClicked();
}

void label_tool::draw_bbox()
{
	frame_cur.copyTo(frame_show);
    // may add other operations to frame_show here

    Mat img_rgb;
	QImage qimg;
	if(frame_cur.channels() == 3)
	{
		cvtColor(frame_show, img_rgb, CV_BGR2RGB);
		qimg = QImage((const unsigned char*)(img_rgb.data),
			img_rgb.cols,
			img_rgb.rows,
			img_rgb.cols*img_rgb.channels(),
			QImage::Format_RGB888);
	}
	else
	{
		qimg = QImage((const unsigned char*)(frame_show.data),
			frame_show.rows,
			frame_show.cols,
			frame_show.cols*frame_show.channels(),
			QImage::Format_RGB888);
	}
	image_label->setPixmap(QPixmap::fromImage(qimg));
	image_label->resize(image_label->pixmap()->size());
	this->update();
}

void label_tool::getSubFolderVideos(QString path, vector<string>& videolist) {
    QDir dir(path);
    if(!dir.exists())  return;

    QStringList filters;
    filters << QString("*.mp4") 
            << QString("*.avi") << QString("*.AVI") 
            << QString("*.flv") << QString("*.FLV") 
            << QString("*.wmv") << QString("*.WMV") 
            << QString("*.3gp") << QString("*.3GP")
            << QString("*.mpg") << QString("*.MPG")
            << QString("*.mov") << QString("*.MOV");
    QDirIterator dir_iterator(path, filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (dir_iterator.hasNext()) {
        dir_iterator.next();
        QFileInfo fi = dir_iterator.fileInfo();
        QString abs_path = fi.absoluteFilePath();
        videolist.push_back(abs_path.toStdString());
    }
}

void label_tool::updateImage()
{
    if (frame_count_now < frame_count - 1) {
        frame_next.copyTo(frame_cur);
	    frame_count_now++;
    }

	if (frame_count_now == frame_count - 1)
	{
		stopButtonClicked();
        isPlay = false;
        addClip(false);
	}

    if (frame_count_now < frame_count - 1) {
	    if(!capture.read(frame_next)) {
		    QMessageBox::warning(this,"warning","Fail to grab frame!");
            stopButtonClicked();
            isPlay = false;
            isError = true;
		    return;
	    }
    }

	//BGR to HSV conversion
	//frame_pre.copyTo(hsv_frame_pre);
	cvtColor(frame_cur, hsv_frame, CV_BGR2HSV);
	//frame.copyTo(hsv_frame);
	cvtColor(frame_next, hsv_frame_next, CV_BGR2HSV);


	//Histogram Calculation
	calcHist(&hsv_frame, 1, channels, Mat(), hist_frame, 2, histSize, ranges, true, false);
	normalize(hist_frame, hist_frame, 0, 1, NORM_MINMAX, -1, Mat() );

	calcHist( &hsv_frame_next, 1, channels, Mat(), hist_frame_next, 2, histSize, ranges, true, false );
	normalize( hist_frame_next, hist_frame_next, 0, 1, NORM_MINMAX, -1, Mat() );

	//Histogram Difference Calculation
	double hist_diff = compareHist( hist_frame_next, hist_frame, 3 );

	if (hist_diff > diff_threshold)
	{
		stopButtonClicked();
        isPlay = false;
        addClip(false);
	}

	slider_lineEdit->setText(QString::number(frame_count_now));
	//画图
	draw_bbox();
}




void label_tool::keyPressEvent(QKeyEvent *e) {

}

void label_tool::sliderChange(int value){
    slider_lineEdit->setText(QString::number(value));
}

void label_tool::speedSliderChange() {
	speed = speed_slider->value();
	if(show_timer.isActive()) {	
		stopButtonClicked();
        goonButtonClicked();
	}
}

void label_tool::openButtonClicked() {
	videolist.clear();
	QMessageBox::StandardButton rb_open = QMessageBox::Yes;
	if(rb_open == QMessageBox::Yes) { 
		QString foldername = QFileDialog::getExistingDirectory(this,"Open folder", "../");
		folder_name_lineEdit->setText(foldername);
		if (foldername != "") {
			//获取该路径下的所有文件
			//traverse_folder(foldername.toStdString(), videolist);
            getSubFolderVideos(foldername, videolist);
			//选取其中的视频文件写入txt
			int size = videolist.size();
			QMessageBox::information(this,"info", QString::number(size) + " video have been detected!");
		}
		if (videolist.size() > 0)
		{
			goToVideo(0);
		}
	}
}

void label_tool::goToVideo(int n) {
    if (n < 0 || n >= videolist.size()) return;

    video_count_now = n;
    string fileName = videolist[video_count_now];
    int pos = fileName.rfind('/') + 1;
    file_name_label->setText(QString::fromLocal8Bit(fileName.substr(pos, fileName.length() - pos).c_str()));
    //更新视频数label
	video_num_label->setText(QString::number(video_count_now + 1) + "/" + QString::number(videolist.size()) + "   ");

	capture.release();
	capture.open(fileName);
    isError = false;	

	if(!capture.isOpened())
	{
		QMessageBox::warning(this,"warning","Fail to open video!");
        isError = true;
		return;
	}
    if(!capture.read(frame_next)) {
		QMessageBox::warning(this,"warning","Fail to grab frame!");
        isError = true;
		return;
	}
	frame_count = capture.get(CV_CAP_PROP_FRAME_COUNT);
	frame_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	frame_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	rate = capture.get(CV_CAP_PROP_FPS);
	frame_count_now = -1;

	if (true || frame_width <= 800 && frame_height <= 450) {
		image_label->setGeometry(QRect(20+(800-frame_width)/2,50+(450-frame_height)/2,frame_width,frame_height));
	}
	else {
		QMessageBox::warning(this,"warning","The size of the video is too large!");
		return;
	}

	//设置滚动条的值
	slider->setMaximum(frame_count - 1);
	slider_label->setText("/" + QString::number(frame_count - 1) );
    updateImage();
    // ------------------ init for the current video
    batchSetButtonStatus1(true);
    batchSetButtonStatus2(true);
    isPlay = false;
    bk_buttons[1]->setChecked(true);
    attr_buttons[0]->setChecked(true);
    load();
    initClipList();
}

void label_tool::nextButtonClicked() {
	save();
    goToVideo(video_count_now + 1);
}

void label_tool::prevButtonClicked() {
	save();
    goToVideo(video_count_now - 1);
}

void label_tool::gotoButtonClicked() {
    int num = atoi(video_idx_lineEdit->text().toLocal8Bit().constData()) - 1;
    goToVideo(num);
}

string label_tool::getResultFileName() {
    //输出结果txt，命名为：原视频名+_result
    return videolist[video_count_now] + "_result.txt";
}

void label_tool::save() {
    //写入数据
	ofstream fout_result(getResultFileName());

    //-------header
    if (isError)
	    fout_result << "!!Error Occurred!! ## video clip labeling result v1.0 ##" << endl;
    else
        fout_result << "## video clip labeling result v1.0 ##" << endl;
	fout_result << frame_count  <<" " << frame_width << " " << frame_height << endl;

    //--------iterate over clips
    fout_result << infos.size() << endl;
    for (vector<ClipInfo>::iterator iter = infos.begin(); iter != infos.end(); iter++) {
        fout_result << iter->isHumanCut << " " << iter->start << " " << iter->end << " " 
                    << int(iter->bk) << " " << iter->attr << endl;
    }
    //--------also append the final position of clip_start
    fout_result << clip_start << endl;

	fout_result.close();
}

void label_tool::load() {
    infos.clear();
    string result_filename = getResultFileName();
    QFile fi(result_filename.c_str());
    if (fi.exists()) {
        ifstream fin(result_filename);
        string str;
        getline(fin, str);
        int c, w, h;
        fin >> c >> w >> h;
        if (c != frame_count || w != frame_width || h != frame_height) {
            QMessageBox::critical(this, "Error", "frames/width/height mismatch!");
            this->close();    
        }
        int cnt;
        fin >> cnt;
        getline(fin, str); // extract&discard '\n' after cnt
        for (int i = 0; i < cnt; i++) {
            ClipInfo ci;
            getline(fin, str);
            ci.parseFromString(str);
            infos.push_back(ci);
        }
        fin.close();
    }
    // set the position for clip_start
    clip_start = infos.size() > 0 ? infos[infos.size() - 1].end + 1 : 0;
}

void label_tool::playButtonClicked() {
    if (isPlay)
        stopButtonClicked();
    else
        goonButtonClicked();
    isPlay = !isPlay;
}


void label_tool::goonButtonClicked() {
	//if(frame_count_now < frame_count - 1)
	{
        double interval = 1000 / (speed * rate);
		show_timer.start(interval);
        play_button->setText("Stop");
        batchSetButtonStatus1(false);
	}
}

void label_tool::stopButtonClicked() {
	show_timer.stop();
    play_button->setText("Play");
    batchSetButtonStatus1(true);
}

void label_tool::batchSetButtonStatus1(bool status) {
    cut_button->setEnabled(status);
    delete_button->setEnabled(status);
    next_button->setEnabled(status);
    prev_button->setEnabled(status);
    goto_button->setEnabled(status);
}

void label_tool::batchSetButtonStatus2(bool status) {
    play_button->setEnabled(status);
    back_button->setEnabled(status);
    fast_back_button->setEnabled(status);
    forward_button->setEnabled(status);
    fast_forward_button->setEnabled(status);
    clip_list->setEnabled(status);
    slider->setEnabled(status);
    slider_lineEdit->setEnabled(status);
}

void label_tool::backButtonClicked() {
    slider_lineEdit->setText(QString::number(frame_count_now - 1));
}

void label_tool::fastBackButtonClicked() {
    slider_lineEdit->setText(QString::number(frame_count_now - 10));
}

void label_tool::forwardButtonClicked() {
    slider_lineEdit->setText(QString::number(frame_count_now + 1));
}

void label_tool::fastForwardButtonClicked() {
    slider_lineEdit->setText(QString::number(frame_count_now + 10));
}


void label_tool::threshold_lineEdit_change(const QString &new_threshold)
{
	diff_threshold = new_threshold.toDouble();
}

void label_tool::sliderLineEditChange(const QString &new_text) {
    int value = new_text.toInt();
    if (value < 0) {
        slider_lineEdit->setText(QString::number(0));
        return;
    }
    if (value >= frame_count) {
        slider_lineEdit->setText(QString::number(frame_count - 1));
        return;
    }
    slider->setValue(value);
    if (value != frame_count_now) { // jump
        stopButtonClicked();
        isPlay = false;
        frame_count_now = value;
        capture.set(CV_CAP_PROP_POS_FRAMES, frame_count_now);
		if(!capture.read(frame_cur))
		{
			QMessageBox::warning(this,"warning","Fail to grab frame!");
            isError = true;
			return;
		}
        if (frame_count_now < frame_count - 1) {
            if(!capture.read(frame_next))
		    {
			    QMessageBox::warning(this,"warning","Fail to grab frame!");
                isError = true;
			    return;
		    }
        }
		//画图
		draw_bbox();
    }

    // update the clip list
    int idx = getCurrentClipIdx();
    clip_list->setCurrentRow(idx);
}

void label_tool::initClipList() {
    clip_list->clear();
    char tmp[100];
    int idx = 0;
    for (vector<ClipInfo>::iterator iter = infos.begin(); iter != infos.end(); iter++, idx++) {
        insertClipListItem(*iter, idx);
    }
    sprintf(tmp, "%06d -->", clip_start);
    clip_list->addItem(tmp);
    clip_list->setCurrentRow(0);
}

void label_tool::addClip(bool isHuman) {
    int idx = getCurrentClipIdx();
    ClipInfo ci;
    if (idx == infos.size()) {// add a new clip
        ci.start = clip_start;
        ci.end = frame_count_now;
        ci.isHumanCut = isHuman;
        setLabelForClip(ci);
        infos.push_back(ci);
        // modify clip start
        clip_start = frame_count_now + 1;
        updateClipStartItem();
        // insert new item to clip_list
        insertClipListItem(ci, idx); 
        clip_list->setCurrentRow(idx);
    } 
    else { // split from current clip
        if (frame_count_now == infos[idx].end) {
            return;
        }
        ci.start = frame_count_now + 1;
        ci.end = infos[idx].end;
        ci.isHumanCut = isHuman;
        ci.copyLabelFrom(infos[idx]);

        infos[idx].end = frame_count_now;
        setLabelForClip(infos[idx]);
        modifyClipListItem(infos[idx], idx); // modify affecte item
        
        idx++;
        infos.insert(infos.begin() + idx, ci); // insert new clip_info
        insertClipListItem(ci, idx); // insert new item to clip_list
        clip_list->setCurrentRow(idx - 1);
    }
}


void label_tool::modifyClipListItem(ClipInfo& ci, int idx) {
    char tmp[100];
    ci.getStringForDisplay(tmp);
    QListWidgetItem* item = clip_list->item(idx);
    item->setText(tmp);
    colorClipListItem(ci, idx);
}

void label_tool::insertClipListItem(ClipInfo& ci, int idx) {
    char tmp[100];
    ci.getStringForDisplay(tmp);
    clip_list->insertItem(idx, tmp);
    colorClipListItem(ci, idx);
}

void label_tool::colorClipListItem(ClipInfo& ci, int idx) {
    QListWidgetItem* item = clip_list->item(idx);
    item->setBackgroundColor(ci.getColor());
}

void label_tool::updateClipStartItem() {
     char tmp[100];
     sprintf(tmp, "%06d -->", clip_start);                         
     QListWidgetItem* item = clip_list->item(clip_list->count() - 1);  
     item->setText(tmp);
}

int label_tool::getCurrentClipIdx() {
    int left = 0, right = infos.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (frame_count_now <= infos[mid].end)
            right = mid - 1;
        else
            left = mid + 1;
    }
    return left;
}

void label_tool::updateClipListSelection() {
    int idx = getCurrentClipIdx();
    clip_list->setCurrentRow(idx);
}

void label_tool::clipListClicked(QListWidgetItem *item) {
    int idx = clip_list->currentRow();
    if (idx < infos.size())
        slider_lineEdit->setText(QString::number(infos[idx].start));
    else
        slider_lineEdit->setText(QString::number(clip_start));
}

void label_tool::cutButtonClicked() {
    addClip(true);
}

void label_tool::deleteButtonClicked() {
    int idx = getCurrentClipIdx();
//    int tmp = frame_count_now;
    if (idx > 0) {                      
        if (idx == infos.size()) { // modify the clip_start
            clip_start = infos[idx - 1].start;
            updateClipStartItem();
            clip_list->takeItem(idx - 1);
            infos.erase(infos.begin() + idx - 1);
            clip_list->setCurrentRow(idx - 1);
        }
        else { // merge current with previous
            infos[idx - 1].end = infos[idx].end;
            modifyClipListItem(infos[idx - 1], idx - 1);
            clip_list->takeItem(idx);
            infos.erase(infos.begin() + idx);
            clip_list->setCurrentRow(idx - 1);
        }
    }
}

void label_tool::disableAttrs() {
    attr_groupbox->setEnabled(false);
}

void label_tool::enableAttrs() {
    attr_groupbox->setEnabled(true);
}

void label_tool::setLabelForClip(ClipInfo& ci) {
    for (int i = 0; i < bk_label_num; i++) {
        if (bk_buttons[i]->isChecked()) {
            ci.bk = ClipInfo::BKLabel(i);
            break;
        }
    }
    for (int i = 0; i < attr_label_num; i++) {
        if (attr_buttons[i]->isChecked()) {
            ci.attr = i;
            break;
        }        
    }
}

void label_tool::clipListSelectionChanged(int idx) {
    if (idx >= infos.size() || idx < 0)
        return;
    int bk = infos[idx].bk;
    int attr = infos[idx].attr;
    bk_buttons[bk]->setChecked(true);
    if (bk == 0)
        disableAttrs();
    else
        enableAttrs();
    attr_buttons[attr]->setChecked(true);
}

void label_tool::bkButtonsClicked() {
    int idx = getCurrentClipIdx();
    if (idx == infos.size())
        return;

    setLabelForClip(infos[idx]);
    modifyClipListItem(infos[idx], idx);
}

void label_tool::attrButtonsClicked() {
    int idx = getCurrentClipIdx();
    if (idx == infos.size())
        return;

    setLabelForClip(infos[idx]);
    modifyClipListItem(infos[idx], idx);
}