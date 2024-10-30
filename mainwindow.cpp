#include <QCamera>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QVideoSink>
#include <QVideoFrame>
#include <QTimer>
#include <QMessageBox>
#include <QInputDialog>
#if QT_CONFIG(permissions)
#include <QPermissions>
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"

CentralWidget::CentralWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CentralWidget)
    , camera(nullptr)
    , capture(new QMediaCaptureSession(this))
    , videoSink(new QVideoSink(this)) {
    ui->setupUi(this);

    checkCameraPermission();
    checkAuthorizationStatus();

    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (!cameras.isEmpty()) {
        QStringList cameraList;
        foreach(const QCameraDevice &device, cameras) {
            cameraList.append(device.description());
        }
        int index = QInputDialog::getItem(this, "选择摄像头", "摄像头", cameraList, 0, false).toInt();
        camera = new QCamera(cameras.at(index), this);
        faceSwapThread = new QFaceFusionThread(MODEL_PATH, this);
        capture->setCamera(camera);
        capture->setVideoSink(videoSink);

        connect(faceSwapThread, &QFaceFusionThread::loadModelState, this, [this](uint32_t state) {
            if(state) {
                camera->start();
            }
        });
        connect(faceSwapThread, &QFaceFusionThread::swapFinished, this, [this](bool ok, const QImage& target, const QImage& output, const QStringList &args, uint32_t findFace, QList<QList<float>> findSimilarity){
            if(ok) {
                srcImg = QPixmap::fromImage(target);
                outImg = QPixmap::fromImage(output);
            } else {
                srcImg = QPixmap::fromImage(target);
                outImg = srcImg;
            }
            m_index++;
        });
        connect(videoSink, &QVideoSink::videoFrameChanged, this, [&](const QVideoFrame &frame){
            if(faceSwapThread->currentProgress() < 2)
                faceSwapThread->setDetect(frame.toImage(),{"detectimg"},0.5f,0.3f);
        });

        faceSwapThread->start();
    } else {
        qDebug() << "No camera devices found.";
    }

    ui->labelSrc->setEnableScal(true);
    ui->labelOut->setEnableScal(true);
    connect(ui->labelSrc, &AspectRatioPixmapLabel::scalChange, ui->labelOut, &AspectRatioPixmapLabel::setScal);
    connect(ui->labelOut, &AspectRatioPixmapLabel::scalChange, ui->labelSrc, &AspectRatioPixmapLabel::setScal);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
        ui->labelSrc->setPixmap(srcImg);
        ui->labelOut->setPixmap(outImg);
    });
    timer->start(1000/30);

    timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, this, [this](){
        ui->labelFPS->setText(QString("fps:%1").arg(m_index/5.0));
        m_index = 0;
    });
    timer2->start(5000);
}

CentralWidget::~CentralWidget() {
    delete ui;
    if (camera) {
        camera->stop();
        delete camera;
    }
}

void CentralWidget::checkCameraPermission(void) {
#if QT_CONFIG(permissions)
    QCameraPermission cameraPermission;
    switch (qApp->checkPermission(cameraPermission)) {
        case Qt::PermissionStatus::Undetermined:
            qApp->requestPermission(cameraPermission, this, &CentralWidget::checkCameraPermission);
            return;
        case Qt::PermissionStatus::Denied:
            qDebug() << "Camera permission is not granted!";
            return;
        case Qt::PermissionStatus::Granted:
            qDebug() << "Camera permission is granted!";
            break;
    }
#endif
}

void CentralWidget::checkAuthorizationStatus(void) {
#if QT_CONFIG(permissions)
    QCameraPermission cameraPermission;
    Qt::PermissionStatus auth_status = Qt::PermissionStatus::Undetermined;
    while(true) {
        QThread::msleep(1);
        auth_status = qApp->checkPermission(cameraPermission);
        if(auth_status == Qt::PermissionStatus::Undetermined)
            continue;
        break;
    }
#endif
}

void CentralWidget::on_actionAbout_triggered() {
    QMessageBox::about(this, "关于", 
        "FaceDetect\n\n"
        "FaceDetect是一个基于yoloface_8n的人脸检测工具");
}

void CentralWidget::checkCloseEvent(QCloseEvent *event) {
    if(donotCheckCloseEvent) {
        event->accept();
        return;
    }
    QMessageBox::StandardButton ret = QMessageBox::question(this, "退出", "确定要退出吗？", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}

MainWindow::MainWindow(bool isDark, QWidget *parent)
    : QGoodWindow(parent) {
    m_central_widget = new CentralWidget(this);
    m_central_widget->setWindowFlags(Qt::Widget);

    m_good_central_widget = new QGoodCentralWidget(this);

#ifdef Q_OS_MAC
    //macOS uses global menu bar
    if(QApplication::testAttribute(Qt::AA_DontUseNativeMenuBar)) {
#else
    if(true) {
#endif
        m_menu_bar = m_central_widget->menuBar();

        //Set font of menu bar
        QFont font = m_menu_bar->font();
    #ifdef Q_OS_WIN
        font.setFamily("Segoe UI");
    #else
        font.setFamily(qApp->font().family());
    #endif
        m_menu_bar->setFont(font);

        QTimer::singleShot(0, this, [&]{
            const int title_bar_height = m_good_central_widget->titleBarHeight();
            m_menu_bar->setStyleSheet(QString("QMenuBar {height: %0px;}").arg(title_bar_height));
        });

        connect(m_good_central_widget,&QGoodCentralWidget::windowActiveChanged,this, [&](bool active){
            m_menu_bar->setEnabled(active);
        #ifdef Q_OS_MACOS
            fixWhenShowQuardCRTTabPreviewIssue();
        #endif
        });

        m_good_central_widget->setLeftTitleBarWidget(m_menu_bar);
        setNativeCaptionButtonsVisibleOnMac(false);
    } else {
        setNativeCaptionButtonsVisibleOnMac(true);
    }

    connect(qGoodStateHolder, &QGoodStateHolder::currentThemeChanged, this, [](){
        if (qGoodStateHolder->isCurrentThemeDark())
            QGoodWindow::setAppDarkTheme();
        else
            QGoodWindow::setAppLightTheme();
    });
    connect(this, &QGoodWindow::systemThemeChanged, this, [&]{
        qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());
    });
    qGoodStateHolder->setCurrentThemeDark(isDark);

    m_good_central_widget->setCentralWidget(m_central_widget);
    setCentralWidget(m_good_central_widget);

    setWindowIcon(QIcon(":/icons/icons/ico.svg"));
    setWindowTitle(m_central_widget->windowTitle());

    m_good_central_widget->setTitleAlignment(Qt::AlignCenter);

    resize(650,500);
}

MainWindow::~MainWindow() {
    delete m_central_widget;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    m_central_widget->checkCloseEvent(event);
}
